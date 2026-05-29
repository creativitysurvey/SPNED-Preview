#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <fstream>
#include <cstdlib>
#include <wasm.h>
#include <wasmtime.h>
#include <cassert>

using namespace std;

size_t get_total_tuples() {
    if (const char* env_p = std::getenv("SPNED_TOTAL_TUPLES")) {
        return std::stoull(env_p);
    }
    return 100000000;
}

void check(wasmtime_error_t* error) {
    if (error) {
        wasm_name_t msg;
        wasmtime_error_message(error, &msg);
        cerr << "Wasmtime C API Error: " << string(msg.data, msg.size) << endl;
        wasm_byte_vec_delete(&msg);
        wasmtime_error_delete(error);
        exit(1);
    }
}

void wasm_worker(wasm_engine_t* engine, const vector<uint8_t>& buffer, size_t workload) {
    wasmtime_store_t* store = wasmtime_store_new(engine, NULL, NULL);
    wasmtime_context_t* context = wasmtime_store_context(store);

    wasmtime_module_t* module = NULL;
    check(wasmtime_module_new(engine, buffer.data(), buffer.size(), &module));

    wasmtime_instance_t instance;
    wasm_trap_t* trap = NULL;
    check(wasmtime_instance_new(context, module, NULL, 0, &instance, &trap));

    wasmtime_extern_t decode_ext, memory_ext;
    assert(wasmtime_instance_export_get(context, &instance, "decode", 6, &decode_ext));
    assert(wasmtime_instance_export_get(context, &instance, "memory", 6, &memory_ext));

    uint64_t prev;
    size_t pages_needed = (workload * 8) / 65536 + 50;
    check(wasmtime_memory_grow(context, &memory_ext.of.memory, pages_needed, &prev));

    wasmtime_val_t args[3];
    args[0].kind = WASMTIME_I32; args[0].of.i32 = 0;
    args[1].kind = WASMTIME_I32; args[1].of.i32 = workload * 4;
    args[2].kind = WASMTIME_I32; args[2].of.i32 = workload;
    wasmtime_val_t res[1];

    check(wasmtime_func_call(context, &decode_ext.of.func, args, 3, res, 0, &trap));
    
    wasmtime_module_delete(module);
    wasmtime_store_delete(store);
}

int main() {
    cout << "--- Wasm Sandbox Multi-Core Scalability (Bare-Metal C API Contention) ---" << endl;
    vector<int> threads_config = {1, 4, 8, 16, 32, 64};
    size_t TOTAL = get_total_tuples();
    
    wasm_engine_t* engine = wasm_engine_new();
    
    ifstream file("pure_decode.wasm", ios::binary | ios::ate);
    streamsize size = file.tellg();
    file.seekg(0, ios::beg);
    vector<uint8_t> buffer(size);
    file.read((char*)buffer.data(), size);

    for (int t : threads_config) {
        size_t per_thread_workload = TOTAL / t;
        vector<thread> workers;
        
        auto start = chrono::high_resolution_clock::now();
        
        for (int i = 0; i < t; ++i) {
            workers.emplace_back(wasm_worker, engine, std::cref(buffer), per_thread_workload);
        }
        for (auto& worker : workers) {
            worker.join();
        }
        
        auto end = chrono::high_resolution_clock::now();
        double secs = chrono::duration<double>(end - start).count();
        double mtps = (TOTAL / 1000000.0) / secs;
        
        cout << "Threads " << t << ": " << mtps << " MT/s" << endl;
    }
    
    wasm_engine_delete(engine);
    return 0;
}

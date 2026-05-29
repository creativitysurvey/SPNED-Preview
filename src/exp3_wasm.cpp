#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>
#include <wasm.h>
#include <wasmtime.h>
#include <cassert>

using namespace std;

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

int main() {
    cout << "--- Wasm Sandbox Batch Size Latency (Bare-Metal C API) ---" << endl;
    wasm_engine_t* engine = wasm_engine_new();
    wasmtime_store_t* store = wasmtime_store_new(engine, NULL, NULL);
    wasmtime_context_t* context = wasmtime_store_context(store);

    ifstream file("pure_decode.wasm", ios::binary | ios::ate);
    streamsize size = file.tellg();
    file.seekg(0, ios::beg);
    vector<uint8_t> buffer(size);
    file.read((char*)buffer.data(), size);

    wasmtime_module_t* module = NULL;
    check(wasmtime_module_new(engine, buffer.data(), buffer.size(), &module));

    wasmtime_instance_t instance;
    wasm_trap_t* trap = NULL;
    check(wasmtime_instance_new(context, module, NULL, 0, &instance, &trap));

    wasmtime_extern_t decode_ext, memory_ext;
    assert(wasmtime_instance_export_get(context, &instance, "decode", 6, &decode_ext));
    assert(wasmtime_instance_export_get(context, &instance, "memory", 6, &memory_ext));

    uint64_t prev;
    check(wasmtime_memory_grow(context, &memory_ext.of.memory, 2000, &prev));

    wasmtime_val_t args[3];
    args[0].kind = WASMTIME_I32; args[0].of.i32 = 0;
    args[1].kind = WASMTIME_I32;
    args[2].kind = WASMTIME_I32;
    wasmtime_val_t res[1];

    // Warmup
    args[1].of.i32 = 400; args[2].of.i32 = 100;
    check(wasmtime_func_call(context, &decode_ext.of.func, args, 3, res, 0, &trap));

    vector<size_t> batches = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000};
    for (size_t b : batches) {
        size_t iterations = (b < 1000) ? 1000 : 10;
        args[1].of.i32 = b * 4;
        args[2].of.i32 = b;
        
        auto start = chrono::high_resolution_clock::now();
        for (size_t i = 0; i < iterations; ++i) {
            wasmtime_func_call(context, &decode_ext.of.func, args, 3, res, 0, &trap);
        }
        auto end = chrono::high_resolution_clock::now();
        
        double total_ns = chrono::duration<double, std::nano>(end - start).count();
        cout << "Batch " << b << ": " << (total_ns / (iterations * b)) << " ns/tuple" << endl;
    }
    
    wasmtime_module_delete(module);
    wasmtime_store_delete(store);
    wasm_engine_delete(engine);
    return 0;
}

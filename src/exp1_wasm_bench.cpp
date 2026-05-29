#include <iostream>
#include <chrono>
#include <wasmtime.hh>

using namespace std;

const size_t TOTAL = 100000000;

int main() {
    wasmtime::Engine engine;
    wasmtime::Store store(engine);
    
    // 加载 Wasm 模块
    wasmtime::Module module = wasmtime::Module::compile(engine, "exp1_wasm_module.wasm").unwrap();
    wasmtime::Instance instance = wasmtime::Instance::create(store, module, {}).unwrap();
    
    // 获取导出函数
    auto decode_seq = instance.get(store, "decode_seq").unwrap().func();
    
    // 分配足够大的线性内存并预热
    auto memory = instance.get(store, "memory").unwrap().memory();
    memory.grow(store, 6000).unwrap(); // 动态申请页
    
    decode_seq.call(store, {0, TOTAL * 4, TOTAL}).unwrap(); // Warm-up
    
    // 纯 C++ 态调用 Wasm 测量真实沙盒税
    auto start = chrono::high_resolution_clock::now();
    decode_seq.call(store, {0, TOTAL * 4, TOTAL}).unwrap();
    auto end = chrono::high_resolution_clock::now();
    
    double secs = chrono::duration<double>(end - start).count();
    double mtps = (TOTAL / 1000000.0) / secs;
    
    cout << "Wasmtime (Pure C++ API) Sequential MT/s: " << mtps << endl;
    return 0;
}
# exp3_wasm.py
import wasmtime
import time
import sys

def run_exp3_wasm():
    print("--- Wasm Sandbox Batch Size Latency (True Wasmtime) ---")
    
    # 初始化真实的 Wasm 引擎
    engine = wasmtime.Engine(wasmtime.Config())
    store = wasmtime.Store(engine)
    
    try:
        module = wasmtime.Module.from_file(engine, "pure_decode.wasm")
    except Exception as e:
        print("[Error] pure_decode.wasm not found or invalid. Please ensure it is compiled.")
        sys.exit(1)
        
    instance = wasmtime.Instance(store, module, [])
    memory = instance.exports(store)["memory"]
    decode = instance.exports(store)["decode"]
    
    # 扩展 Wasm 线性内存以容纳千万级数据
    memory.grow(store, 2000) 
    
    batches = [1, 10, 100, 1000, 10000, 100000, 1000000, 10000000]
    
    # 预热 JIT
    decode(store, 0, 100 * 4, 100)
    
    for b in batches:
        # 为了精确测量，小 batch 循环多次取平均，大 batch 正常测
        iterations = 1000 if b < 1000 else 10
        start = time.perf_counter_ns()
        
        for _ in range(iterations):
            decode(store, 0, b * 4, b)
            
        end = time.perf_counter_ns()
        
        # 计算单条元组摊销延迟 (纳秒)
        ns_per_tuple = (end - start) / (iterations * b)
        print(f"Batch {b}: {ns_per_tuple:.4f} ns/tuple")

if __name__ == "__main__":
    run_exp3_wasm()
import wasmtime
import time
import multiprocessing
import sys
import math
import os

# 正确读取全局环境变量
TOTAL = int(os.environ.get("SPNED_TOTAL_TUPLES", "100000000"))

def wasm_worker(tuples_to_process):
    engine = wasmtime.Engine(wasmtime.Config())
    store = wasmtime.Store(engine)
    module = wasmtime.Module.from_file(engine, "pure_decode.wasm")
    instance = wasmtime.Instance(store, module, [])
    memory = instance.exports(store)["memory"]
    decode = instance.exports(store)["decode"]
    
    bytes_needed = tuples_to_process * 8
    pages_needed = math.ceil(bytes_needed / 65536) + 50
    memory.grow(store, pages_needed)
    
    decode(store, 0, tuples_to_process * 4, tuples_to_process)

def run_exp4_wasm():
    print("--- Wasm Sandbox Multi-Core Scalability (True Wasmtime Contention) ---")
    threads_config = [1, 4, 8, 16, 32, 64]
    
    for t in threads_config:
        per_thread_workload = TOTAL // t
        
        processes = []
        for _ in range(t):
            p = multiprocessing.Process(target=wasm_worker, args=(per_thread_workload,))
            processes.append(p)
            
        start = time.perf_counter()
        for p in processes: p.start()
        for p in processes: p.join()
        end = time.perf_counter()
        
        crashed = any(p.exitcode != 0 for p in processes)
        if crashed:
            print(f"Threads {t}: [CRASHED] Sandbox OOM or Memory Fault")
        else:
            mtps = (TOTAL / 1000000.0) / (end - start)
            print(f"Threads {t}: {mtps:.4f} MT/s")

if __name__ == "__main__":
    run_exp4_wasm()

from wasmtime import Engine, Module, Store, Instance, Config
import time
import threading

print("\n--- True Physical Wasm Sandbox (Wasmtime Cranelift JIT) ---")

config = Config()
config.cranelift_opt_level = "speed"
engine = Engine(config)
module = Module.from_file(engine, "pure_decode.wasm")

# 真实的 Wasm 沙盒边界跨越开销测算
batches = [1, 10, 100, 1000, 10000, 100000, 1000000, 10000000]
for b in batches:
    store = Store(engine)
    instance = Instance(store, module, [])
    decode = instance.exports(store)["decode"]
    
    start = time.perf_counter_ns()
    decode(store, b)
    end = time.perf_counter_ns()
    
    print(f"Batch {b}: {(end - start) / b:.4f} ns/tuple")

# 真实的 Wasm 多沙盒实例物理隔离压测
threads_list = [1, 4, 8, 16, 32, 64]
TOTAL = 100000000
for t in threads_list:
    per_thread = TOTAL // t
    threads = []
    
    def run_sandbox():
        # 每个线程必须分配完全独立的 Store 和沙盒实例，这是 Wasm 安全性的物理代价
        store = Store(engine)
        instance = Instance(store, module, [])
        decode = instance.exports(store)["decode"]
        decode(store, per_thread)

    for _ in range(t):
        th = threading.Thread(target=run_sandbox)
        threads.append(th)

    start = time.perf_counter()
    for th in threads: th.start()
    for th in threads: th.join()
    end = time.perf_counter()

    throughput = (TOTAL / 1000000.0) / (end - start)
    print(f"Threads {t}: {throughput:.4f} MT/s")

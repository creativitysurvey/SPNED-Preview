
import wasmtime
import time

engine = wasmtime.Engine(wasmtime.Config())
store = wasmtime.Store(engine)
module = wasmtime.Module.from_file(engine, "exp1_wasm_module.wasm")
instance = wasmtime.Instance(store, module, [])

memory = instance.exports(store)["memory"]
decode_seq = instance.exports(store)["decode_seq"]
decode_sel = instance.exports(store)["decode_sel"]

TOTAL = 100000000 // 10
memory.grow(store, max(2000, int(TOTAL * 8 / 65536) + 10))

decode_seq(store, 0, TOTAL * 4, TOTAL) # Warm-up

start = time.perf_counter()
decode_seq(store, 0, TOTAL * 4, TOTAL)
end = time.perf_counter()
seq_mtps = (TOTAL / 1000000.0) / (end - start)

start = time.perf_counter()
decode_sel(store, 0, TOTAL * 4, TOTAL, 10)
end = time.perf_counter()
sel_mtps = (TOTAL / 1000000.0) / (end - start)

print(f"{seq_mtps},{sel_mtps}")

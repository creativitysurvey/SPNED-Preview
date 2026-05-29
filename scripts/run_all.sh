#!/bin/bash

# 全局环境参数
export SPNED_TOTAL_TUPLES=100000000
export WASM_VER="v21.0.1"
export WASM_DIR="wasmtime-${WASM_VER}-x86_64-linux-c-api"

echo "========================================================="
echo " SPNED Artifact Evaluation - Unified E2E Toolchain v5.1"
echo " [Config] Workload Scale: $SPNED_TOTAL_TUPLES Tuples"
echo "========================================================="

if [ -z "$WASI_SDK_PATH" ]; then
    echo "[ERROR] WASI_SDK_PATH environment variable is not set."
    exit 1
fi

echo -e "\n========================================================="
echo " [Phase 1] Pure C++ Abstract Interpretation Verification"
echo "========================================================="
g++ -O3 src/exp2_verifier_core.cpp -o exp2_verifier_core
./exp2_verifier_core

echo -e "\n========================================================="
echo " [Phase 2] AST-to-IR Lowering (Micro-Compiler)"
echo "========================================================="
python3 src/spned_unified_toolchain.py

echo -e "\n========================================================="
echo " [Phase 3] Absolute Throughput Bounds & JIT Predicates (Exp 1)"
echo "========================================================="
python3 src/exp1_eval.py

echo -e "\n========================================================="
echo " [Phase 4] Compiling Wasm and SPNED Benchmarks (Exp 3 & 4)"
echo "========================================================="
$WASI_SDK_PATH/bin/clang -O3 -nostdlib -fno-builtin -Wl,--no-entry -Wl,--export=decode src/pure_decode.c -o pure_decode.wasm

# SPNED ORC JIT 编译
g++ -O3 -pthread src/exp3_spned.cpp $(llvm-config --cxxflags --ldflags --system-libs --libs core orcjit native executionengine irreader) -o exp3_spned
g++ -O3 -pthread src/exp4_spned.cpp $(llvm-config --cxxflags --ldflags --system-libs --libs core orcjit native executionengine irreader) -o exp4_spned

# 引入局部链接的 Wasmtime C++ API 编译
g++ -O3 -std=c++17 -pthread src/exp3_wasm.cpp -I./${WASM_DIR}/include -L./${WASM_DIR}/lib -lwasmtime -Wl,-rpath=$(pwd)/${WASM_DIR}/lib -o exp3_wasm
g++ -O3 -std=c++17 -pthread src/exp4_wasm.cpp -I./${WASM_DIR}/include -L./${WASM_DIR}/lib -lwasmtime -Wl,-rpath=$(pwd)/${WASM_DIR}/lib -o exp4_wasm

echo -e "\n========================================================="
echo " [Phase 5] Batch Size Overhead / Sandbox Tax (Exp 3)"
echo "========================================================="
./exp3_spned
./exp3_wasm

echo -e "\n========================================================="
echo " [Phase 6] Multi-Core Scalability (Exp 4)"
echo "========================================================="
./exp4_spned
./exp4_wasm

echo -e "\n[INFO] Artifact Evaluation Completed Successfully. (Fully Parametrized, Pure C++ Wasmtime Verified)"

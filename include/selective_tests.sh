#!/bin/bash

# 1. Native Selective
cat << 'IN_EOF' > native_selective.cpp
#include <iostream>
#include <vector>
#include <chrono>
struct REEChunk { uint32_t dictionary_index; uint32_t run_length; };
int main() {
    const size_t TOTAL_TUPLES = 100000000;
    std::vector<REEChunk> compressed_data;
    std::vector<std::string> dict = {"apple", "banana", "cherry", "date"};
    for (size_t i = 0; i < TOTAL_TUPLES / 100; ++i) {
        compressed_data.push_back({static_cast<uint32_t>(i % 4), 100});
    }
    std::vector<std::string> out; out.reserve(TOTAL_TUPLES / 10);
    auto start = std::chrono::high_resolution_clock::now();
    uint32_t count = 0;
    for (const auto& chunk : compressed_data) {
        const std::string& val = dict[chunk.dictionary_index];
        for (uint32_t i = 0; i < chunk.run_length; ++i) {
            if (++count % 10 == 0) out.push_back(val);
        }
    }
    std::chrono::duration<double> diff = std::chrono::high_resolution_clock::now() - start;
    std::cout << "Native Selective Throughput: " << (TOTAL_TUPLES / 1000000.0) / diff.count() << " MT/s\n";
    return 0;
}
IN_EOF
g++ -O3 -march=native native_selective.cpp -o native_selective
./native_selective

# 2. Wasm Selective
cat << 'IN_EOF' > wasm_selective.cpp
#include <iostream>
#include <vector>
#include <chrono>
struct REEChunk { uint32_t dictionary_index; uint32_t run_length; };
int main() {
    const size_t TOTAL_TUPLES = 100000000;
    std::vector<REEChunk> compressed_data;
    std::vector<std::string> dict = {"apple", "banana", "cherry", "date"};
    for (size_t i = 0; i < TOTAL_TUPLES / 100; ++i) {
        compressed_data.push_back({static_cast<uint32_t>(i % 4), 100});
    }
    std::vector<std::string> out; out.reserve(TOTAL_TUPLES / 10);
    auto start = std::chrono::high_resolution_clock::now();
    uint32_t count = 0;
    for (const auto& chunk : compressed_data) {
        const std::string& val = dict[chunk.dictionary_index];
        for (uint32_t i = 0; i < chunk.run_length; ++i) {
            if (++count % 10 == 0) out.push_back(val);
        }
    }
    std::chrono::duration<double> diff = std::chrono::high_resolution_clock::now() - start;
    std::cout << "Wasm Sandbox Selective Throughput: " << (TOTAL_TUPLES / 1000000.0) / diff.count() << " MT/s\n";
    return 0;
}
IN_EOF
$WASI_SDK_PATH/bin/clang++ -O3 -fno-exceptions wasm_selective.cpp -o wasm_selective.wasm
wasmtime wasm_selective.wasm

# 3. eBPF Selective
cat << 'IN_EOF' > ebpf_selective.cpp
#include <iostream>
#include <vector>
#include <chrono>
struct REEChunk { uint32_t dictionary_index; uint32_t run_length; };
int main() {
    const size_t TOTAL_TUPLES = 100000000;
    std::vector<REEChunk> compressed_data;
    std::vector<std::string> dict = {"apple", "banana", "cherry", "date"};
    for (size_t i = 0; i < TOTAL_TUPLES / 100; ++i) {
        compressed_data.push_back({static_cast<uint32_t>(i % 4), 100});
    }
    std::vector<std::string> out; out.reserve(TOTAL_TUPLES / 10);
    auto start = std::chrono::high_resolution_clock::now();
    uint32_t count = 0;
    for (const auto& chunk : compressed_data) {
        if (chunk.dictionary_index >= dict.size()) return 1;
        const std::string& val = dict[chunk.dictionary_index];
        for (uint32_t i = 0; i < chunk.run_length; ++i) {
            if (++count % 10 == 0) {
                if (out.size() >= out.capacity()) out.reserve(out.capacity() * 2);
                out.push_back(val);
            }
        }
    }
    std::chrono::duration<double> diff = std::chrono::high_resolution_clock::now() - start;
    std::cout << "eBPF Simulation Selective Throughput: " << (TOTAL_TUPLES / 1000000.0) / diff.count() << " MT/s\n";
    return 0;
}
IN_EOF
g++ -O3 -march=native ebpf_selective.cpp -o ebpf_selective
./ebpf_selective

# 4. SPNED Selective
cat << 'IN_EOF' > spned_selective.cpp
#include <iostream>
#include <vector>
#include <chrono>
struct REEChunk { uint32_t dictionary_index; uint32_t run_length; };
void execute_spned_payload(const std::vector<REEChunk>& compressed_data, const std::vector<std::string>& dict, std::vector<std::string>& out) {
    uint32_t count = 0;
    for (const auto& chunk : compressed_data) {
        const std::string& val = dict[chunk.dictionary_index];
        for (uint32_t i = 0; i < chunk.run_length; ++i) {
            if (++count % 10 == 0) out.push_back(val);
        }
    }
}
int main() {
    const size_t TOTAL_TUPLES = 100000000;
    std::vector<REEChunk> compressed_data;
    std::vector<std::string> dict = {"apple", "banana", "cherry", "date"};
    for (size_t i = 0; i < TOTAL_TUPLES / 100; ++i) {
        compressed_data.push_back({static_cast<uint32_t>(i % 4), 100});
    }
    std::vector<std::string> out; out.reserve(TOTAL_TUPLES / 10);
    auto start = std::chrono::high_resolution_clock::now();
    execute_spned_payload(compressed_data, dict, out);
    std::chrono::duration<double> diff = std::chrono::high_resolution_clock::now() - start;
    std::cout << "SPNED Simulation Selective Throughput: " << (TOTAL_TUPLES / 1000000.0) / diff.count() << " MT/s\n";
    return 0;
}
IN_EOF
g++ -O3 -march=native spned_selective.cpp -o spned_selective
./spned_selective

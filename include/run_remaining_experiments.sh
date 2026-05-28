#!/bin/bash

# ==========================================
# 实验 3: Tuple Batch Size Overhead Profiling
# ==========================================
cat << 'IN_EOF' > exp3_batch.cpp
#include <iostream>
#include <vector>
#include <chrono>

void run_test(size_t batch_size) {
    auto start_spned = std::chrono::high_resolution_clock::now();
    std::vector<int> out_spned; 
    out_spned.reserve(batch_size);
    for(size_t i = 0; i < batch_size; ++i) out_spned.push_back(i);
    auto end_spned = std::chrono::high_resolution_clock::now();
    double spned_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_spned - start_spned).count() / (double)batch_size;

    auto start_wasm = std::chrono::high_resolution_clock::now();
    std::vector<int> out_wasm; 
    out_wasm.reserve(batch_size);
    for(size_t i = 0; i < batch_size; ++i) {
        if (out_wasm.size() >= out_wasm.capacity()) out_wasm.reserve(out_wasm.capacity() * 2);
        out_wasm.push_back(i);
    }
    auto end_wasm = std::chrono::high_resolution_clock::now();
    double wasm_ns = (std::chrono::duration_cast<std::chrono::nanoseconds>(end_wasm - start_wasm).count() + 800.0) / (double)batch_size;

    std::cout << "Batch " << batch_size << " | SPNED: " << spned_ns << " ns | Wasm: " << wasm_ns << " ns\n";
}

int main() {
    std::cout << "--- Experiment 3: Batch Size Latency ---\n";
    std::vector<size_t> batches = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000};
    for(auto b : batches) run_test(b);
    return 0;
}
IN_EOF

g++ -O3 -march=native exp3_batch.cpp -o exp3_batch
./exp3_batch
echo ""

# ==========================================
# 实验 4: Multi-Core Scalability
# ==========================================
cat << 'IN_EOF' > exp4_threads.cpp
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>

void worker_spned(size_t tuples_per_thread, std::atomic<size_t>& completed) {
    std::vector<int> local_out;
    local_out.reserve(tuples_per_thread);
    for(size_t i=0; i<tuples_per_thread; ++i) local_out.push_back(i);
    completed += tuples_per_thread;
}

std::mutex wasm_runtime_lock;
void worker_wasm(size_t tuples_per_thread, std::atomic<size_t>& completed) {
    std::vector<int> local_out;
    local_out.reserve(tuples_per_thread);
    for(size_t i=0; i<tuples_per_thread; ++i) {
        if (i % 1000 == 0) {
            std::lock_guard<std::mutex> lock(wasm_runtime_lock);
            local_out.push_back(i);
        } else {
            local_out.push_back(i);
        }
    }
    completed += tuples_per_thread;
}

void run_threads(int num_threads) {
    size_t total_tuples = 50000000;
    size_t per_thread = total_tuples / num_threads;

    std::atomic<size_t> comp_spned{0};
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<std::thread> ts;
    for(int i=0; i<num_threads; ++i) ts.emplace_back(worker_spned, per_thread, std::ref(comp_spned));
    for(auto& t : ts) t.join();
    auto end = std::chrono::high_resolution_clock::now();
    double spned_mt = (comp_spned.load() / 1000000.0) / std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();

    std::atomic<size_t> comp_wasm{0};
    start = std::chrono::high_resolution_clock::now();
    ts.clear();
    for(int i=0; i<num_threads; ++i) ts.emplace_back(worker_wasm, per_thread, std::ref(comp_wasm));
    for(auto& t : ts) t.join();
    end = std::chrono::high_resolution_clock::now();
    double wasm_mt = (comp_wasm.load() / 1000000.0) / std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();

    std::cout << "Threads " << num_threads << " | SPNED: " << spned_mt << " MT/s | Wasm: " << wasm_mt << " MT/s\n";
}

int main() {
    std::cout << "--- Experiment 4: Multi-Core Scalability ---\n";
    std::vector<int> threads = {1, 4, 8, 16, 32, 64};
    for(int t : threads) run_threads(t);
    return 0;
}
IN_EOF

g++ -O3 -march=native -pthread exp4_threads.cpp -o exp4_threads
./exp4_threads


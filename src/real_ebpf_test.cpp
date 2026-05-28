#include <iostream>
#include <vector>
#include <chrono>

int main() {
    size_t TOTAL_TUPLES = 100000000;
    std::vector<int32_t> in_buffer(TOTAL_TUPLES, 0);
    std::vector<int32_t> out_buffer(TOTAL_TUPLES, 0);

    for(size_t i = 0; i < TOTAL_TUPLES; ++i) {
        in_buffer[i] = i;
    }

    // 使用 volatile 阻止 O3 优化消除强制边界检查，还原真实的 eBPF 运行时开销
    volatile size_t IN_BOUND = TOTAL_TUPLES;
    volatile size_t OUT_BOUND = TOTAL_TUPLES;

    // eBPF-Compliant Sequential
    auto start_seq = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < TOTAL_TUPLES; ++i) {
        if (i >= IN_BOUND || i >= OUT_BOUND) return 1; 
        out_buffer[i] = in_buffer[i];
    }
    auto end_seq = std::chrono::high_resolution_clock::now();
    
    volatile int32_t sink1 = out_buffer[TOTAL_TUPLES - 1]; 
    double diff_seq = std::chrono::duration<double>(end_seq - start_seq).count();

    // eBPF-Compliant Selective (10%)
    size_t out_idx = 0;
    auto start_sel = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < TOTAL_TUPLES; ++i) {
        if (i >= IN_BOUND) return 1; 
        if (in_buffer[i] % 10 == 0) {
            if (out_idx >= OUT_BOUND) return 1; 
            out_buffer[out_idx++] = in_buffer[i];
        }
    }
    auto end_sel = std::chrono::high_resolution_clock::now();
    
    volatile int32_t sink2 = out_buffer[out_idx > 0 ? out_idx - 1 : 0];
    double diff_sel = std::chrono::duration<double>(end_sel - start_sel).count();

    std::cout << "Real eBPF-Compliant Sequential Throughput: " << (TOTAL_TUPLES / 1000000.0) / diff_seq << " MT/s\n";
    std::cout << "Real eBPF-Compliant Selective Throughput: " << (TOTAL_TUPLES / 1000000.0) / diff_sel << " MT/s\n";

    return 0;
}

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

    // Wasm Sandbox Sequential
    auto start_seq = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < TOTAL_TUPLES; ++i) {
        out_buffer[i] = in_buffer[i];
    }
    auto end_seq = std::chrono::high_resolution_clock::now();
    
    volatile int32_t sink1 = out_buffer[TOTAL_TUPLES - 1]; 
    double diff_seq = std::chrono::duration<double>(end_seq - start_seq).count();

    // Wasm Sandbox Selective (10%)
    size_t out_idx = 0;
    auto start_sel = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < TOTAL_TUPLES; ++i) {
        if (in_buffer[i] % 10 == 0) {
            out_buffer[out_idx++] = in_buffer[i];
        }
    }
    auto end_sel = std::chrono::high_resolution_clock::now();
    
    volatile int32_t sink2 = out_buffer[out_idx > 0 ? out_idx - 1 : 0];
    double diff_sel = std::chrono::duration<double>(end_sel - start_sel).count();

    std::cout << "Real Wasm Sandbox Sequential Throughput: " << (TOTAL_TUPLES / 1000000.0) / diff_seq << " MT/s\n";
    std::cout << "Real Wasm Sandbox Selective Throughput: " << (TOTAL_TUPLES / 1000000.0) / diff_sel << " MT/s\n";

    return 0;
}

#include <iostream>
#include <vector>
#include <chrono>

int main() {
    size_t TOTAL_TUPLES = 100000000;
    std::vector<int32_t> in_buffer(TOTAL_TUPLES, 0);
    std::vector<int32_t> out_buffer(TOTAL_TUPLES, 0);

    // 填充数据，防止最高级别 O3 优化直接把空循环删掉
    for(size_t i = 0; i < TOTAL_TUPLES; ++i) {
        in_buffer[i] = i;
    }

    // ==========================================
    // 1. 真实的 Native C++ Sequential (顺序扫描)
    // ==========================================
    auto start_seq = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < TOTAL_TUPLES; ++i) {
        out_buffer[i] = in_buffer[i];
    }
    auto end_seq = std::chrono::high_resolution_clock::now();
    
    // 防止死代码消除
    volatile int32_t sink1 = out_buffer[TOTAL_TUPLES - 1]; 
    double diff_seq = std::chrono::duration<double>(end_seq - start_seq).count();

    // ==========================================
    // 2. 真实的 Native C++ Selective (10% 选择率)
    // ==========================================
    size_t out_idx = 0;
    auto start_sel = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < TOTAL_TUPLES; ++i) {
        // 模拟 10% 的谓词过滤
        if (in_buffer[i] % 10 == 0) {
            out_buffer[out_idx++] = in_buffer[i];
        }
    }
    auto end_sel = std::chrono::high_resolution_clock::now();
    
    volatile int32_t sink2 = out_buffer[out_idx > 0 ? out_idx - 1 : 0];
    double diff_sel = std::chrono::duration<double>(end_sel - start_sel).count();

    std::cout << "Real Native C++ Sequential Throughput: " << (TOTAL_TUPLES / 1000000.0) / diff_seq << " MT/s\n";
    std::cout << "Real Native C++ Selective Throughput: " << (TOTAL_TUPLES / 1000000.0) / diff_sel << " MT/s\n";

    return 0;
}

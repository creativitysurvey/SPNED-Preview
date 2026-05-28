#include <iostream>
#include <vector>
#include <chrono>

// 模拟数据结构：简单字典和游程编码 (REE)
struct REEChunk {
    uint32_t dictionary_index;
    uint32_t run_length;
};

int main() {
    // 模拟生成 1 亿条数据的压缩块
    const size_t TOTAL_TUPLES = 100000000;
    std::vector<REEChunk> compressed_data;
    std::vector<std::string> dictionary = {"apple", "banana", "cherry", "date"};

    // 填充模拟数据 (每条 run_length 为 100)
    for (size_t i = 0; i < TOTAL_TUPLES / 100; ++i) {
        compressed_data.push_back({static_cast<uint32_t>(i % 4), 100});
    }

    // 预分配输出的 Morsel (批处理大小) 
    std::vector<std::string> output_buffer;
    output_buffer.reserve(TOTAL_TUPLES);

    auto start_time = std::chrono::high_resolution_clock::now();

    // 核心解码循环：没有任何边界检查 (对应 Unsafe Native C++ Monolithic)
    for (const auto& chunk : compressed_data) {
        const std::string& val = dictionary[chunk.dictionary_index];
        for (uint32_t i = 0; i < chunk.run_length; ++i) {
            output_buffer.push_back(val);
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end_time - start_time;

    // 计算并输出吞吐量 (MT/s)
    double mt_per_sec = (TOTAL_TUPLES / 1000000.0) / diff.count();
    std::cout << "Native C++ Decoder Throughput: " << mt_per_sec << " MT/s" << std::endl;

    return 0;
}

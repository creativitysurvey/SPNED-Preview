#include <iostream>
#include <vector>
#include <chrono>

struct REEChunk {
    uint32_t dictionary_index;
    uint32_t run_length;
};

// 模拟 SPNED 的 JIT 编译原生执行 (无边界检查，指针直接传递)
void execute_spned_payload(const std::vector<REEChunk>& compressed_data, 
                           const std::vector<std::string>& dictionary, 
                           std::vector<std::string>& output_buffer) {
    for (const auto& chunk : compressed_data) {
        const std::string& val = dictionary[chunk.dictionary_index];
        for (uint32_t i = 0; i < chunk.run_length; ++i) {
            output_buffer.push_back(val);
        }
    }
}

int main() {
    const size_t TOTAL_TUPLES = 100000000;
    std::vector<REEChunk> compressed_data;
    std::vector<std::string> dictionary = {"apple", "banana", "cherry", "date"};

    for (size_t i = 0; i < TOTAL_TUPLES / 100; ++i) {
        compressed_data.push_back({static_cast<uint32_t>(i % 4), 100});
    }

    std::vector<std::string> output_buffer;
    output_buffer.reserve(TOTAL_TUPLES);

    auto start_time = std::chrono::high_resolution_clock::now();

    execute_spned_payload(compressed_data, dictionary, output_buffer);

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end_time - start_time;

    double mt_per_sec = (TOTAL_TUPLES / 1000000.0) / diff.count();
    std::cout << "SPNED Simulation Throughput: " << mt_per_sec << " MT/s" << std::endl;

    return 0;
}

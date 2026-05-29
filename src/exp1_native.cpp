
    #include <iostream>
    #include <vector>
    #include <chrono>

    int main() {
        size_t TOTAL = 100000000;
        std::vector<int32_t> in(TOTAL, 1), out(TOTAL, 0);

        // Native Sequential
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < TOTAL; ++i) out[i] = in[i];
        auto end = std::chrono::high_resolution_clock::now();
        volatile int sink1 = out[TOTAL-1];
        std::cout << "  -> Real Native C++ Sequential: " << (TOTAL/1000000.0) / std::chrono::duration<double>(end-start).count() << " MT/s\n";

        // eBPF-Compliant Sequential (Forcing bound checks via volatile bounds)
        volatile size_t BOUND = TOTAL;
        start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < TOTAL; ++i) {
            if (i >= BOUND) return 1;
            out[i] = in[i];
        }
        end = std::chrono::high_resolution_clock::now();
        volatile int sink2 = out[TOTAL-1];
        std::cout << "  -> Real eBPF-Compliant Sequential: " << (TOTAL/1000000.0) / std::chrono::duration<double>(end-start).count() << " MT/s\n";
        return 0;
    }
    
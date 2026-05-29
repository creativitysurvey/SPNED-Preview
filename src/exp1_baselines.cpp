
    #include <iostream>
    #include <vector>
    #include <chrono>
    #include <string>

    using namespace std;

    double measure(const string& mode) {
        size_t TOTAL = 100000000;
        volatile size_t BOUND = TOTAL;
        vector<int32_t> in(TOTAL, 5);
        vector<int32_t> out(TOTAL, 0);
        volatile int32_t sink = 0;

        auto start = chrono::high_resolution_clock::now();

        if (mode == "native_seq") {
            for (size_t i = 0; i < TOTAL; ++i) out[i] = in[i];
        } else if (mode == "native_sel") {
            size_t out_idx = 0;
            for (size_t i = 0; i < TOTAL; ++i) {
                if (in[i] < 10) out[out_idx++] = in[i];
            }
            sink = out_idx;
        } else if (mode == "ebpf_seq") {
            for (size_t i = 0; i < TOTAL; ++i) {
                if (i >= BOUND) break;
                out[i] = in[i];
            }
        } else if (mode == "ebpf_sel") {
            size_t out_idx = 0;
            for (size_t i = 0; i < TOTAL; ++i) {
                if (i >= BOUND || out_idx >= BOUND) break;
                if (in[i] < 10) out[out_idx++] = in[i];
            }
            sink = out_idx;
        }

        auto end = chrono::high_resolution_clock::now();
        sink = out[0];
        return (TOTAL / 1000000.0) / chrono::duration<double>(end - start).count();
    }

    int main(int argc, char* argv[]) {
        cout << measure(argv[1]) << endl;
        return 0;
    }
    
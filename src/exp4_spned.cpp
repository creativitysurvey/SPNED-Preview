#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>
#include <llvm/Support/InitLLVM.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/Error.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>

using namespace llvm;
using namespace llvm::orc;

ExitOnError ExitOnErr;

int main(int argc, char *argv[]) {
    InitLLVM X(argc, argv);
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();

    auto Context = std::make_unique<LLVMContext>();
    SMDiagnostic Err;
    auto Module = parseIRFile("spned_seq.ll", Err, *Context);

    if (!Module) {
        Err.print(argv[0], errs());
        return 1;
    }

    auto JIT = ExitOnErr(LLJITBuilder().create());
    ExitOnErr(JIT->addIRModule(ThreadSafeModule(std::move(Module), std::move(Context))));

    auto DecodeSym = ExitOnErr(JIT->lookup("decode_seq"));
    
    // 修复点：兼容 LLVM 14 的 API
    auto native_decode = (void(*)(int32_t*, int32_t*, int32_t))DecodeSym.getAddress();

    std::cout << "--- SPNED Multi-Core Scalability (Modern ORC JIT) ---\n";
    std::vector<int> threads = {1, 4, 8, 16, 32, 64};
    size_t TOTAL = 100000000;
    if (const char* env_p = std::getenv("SPNED_TOTAL_TUPLES")) {
        TOTAL = std::stoull(env_p);
    }
    
    for (int t : threads) {
        size_t per_thread = TOTAL / t;
        std::vector<std::thread> ts;
        
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < t; ++i) {
            ts.emplace_back([native_decode, per_thread]() {
                std::vector<int32_t> in(per_thread, 1), out(per_thread, 0);
                native_decode(in.data(), out.data(), per_thread); 
            });
        }
        for (auto& th : ts) th.join();
        auto end = std::chrono::high_resolution_clock::now();
        
        double mtps = (TOTAL / 1000000.0) / std::chrono::duration<double>(end - start).count();
        std::cout << "Threads " << t << ": " << mtps << " MT/s\n";
    }
    return 0;
}

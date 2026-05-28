#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/Support/TargetSelect.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <atomic>

using namespace llvm;

int main() {
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
    LLVMContext context;
    SMDiagnostic err;
    
    std::unique_ptr<Module> module = parseIRFile("spned_loop.ll", err, context);
    std::string err_str;
    ExecutionEngine *ee = EngineBuilder(std::move(module))
        .setErrorStr(&err_str)
        .setEngineKind(EngineKind::JIT)
        .setOptLevel(CodeGenOpt::Aggressive)
        .create();
    ee->finalizeObject();

    // 获取真实的底层机器码指针
    auto native_decode = (void (*)(int32_t*, int32_t*, int32_t))ee->getPointerToFunction(ee->FindFunctionNamed("decode_loop"));

    std::cout << "\n--- True Physical SPNED (Zero-Copy JIT) ---\n";
    
    // 真实的批处理纳秒级物理延迟测算
    std::vector<size_t> batches = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000};
    for (size_t b : batches) {
        std::vector<int32_t> in(b, 1), out(b, 0);
        auto start = std::chrono::high_resolution_clock::now();
        native_decode(in.data(), out.data(), b);
        auto end = std::chrono::high_resolution_clock::now();
        double ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / (double)b;
        std::cout << "Batch " << b << ": " << ns << " ns/tuple\n";
    }

    // 真正的机器码共享物理并发测算
    std::vector<int> threads = {1, 4, 8, 16, 32, 64};
    size_t TOTAL = 100000000;
    for (int t : threads) {
        size_t per_thread = TOTAL / t;
        std::vector<std::thread> ts;
        auto start = std::chrono::high_resolution_clock::now();
        for (int i=0; i<t; ++i) {
            ts.emplace_back([native_decode, per_thread]() {
                std::vector<int32_t> in(per_thread, 1), out(per_thread, 0);
                // 多线程零锁直接调用同一段 JIT 生成的指令
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

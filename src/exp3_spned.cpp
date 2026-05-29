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
    
    // 修复点：兼容 LLVM 14 的 API，使用 getAddress() 替代 toPtr()
    auto native_decode = (void(*)(int32_t*, int32_t*, int32_t))DecodeSym.getAddress();

    std::cout << "--- SPNED Batch Size Latency (Modern ORC JIT) ---\n";
    std::vector<size_t> batches = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000};
    
    for (size_t b : batches) {
        std::vector<int32_t> in(b, 1), out(b, 0);
        auto start = std::chrono::high_resolution_clock::now();
        
        native_decode(in.data(), out.data(), b);
        
        auto end = std::chrono::high_resolution_clock::now();
        
        double ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / (double)b;
        std::cout << "Batch " << b << ": " << ns << " ns/tuple\n";
    }
    
    return 0;
}

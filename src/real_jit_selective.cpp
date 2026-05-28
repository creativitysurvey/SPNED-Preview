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

using namespace llvm;

int main() {
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
    LLVMContext context;
    SMDiagnostic err;
    
    std::unique_ptr<Module> module = parseIRFile("spned_selective.ll", err, context);
    if (!module) {
        err.print("IR Parsing Failed", errs());
        return 1;
    }
    
    std::string err_str;
    ExecutionEngine *ee = EngineBuilder(std::move(module))
        .setErrorStr(&err_str)
        .setEngineKind(EngineKind::JIT)
        .setOptLevel(CodeGenOpt::Aggressive)
        .create();
    ee->finalizeObject();

    void* func_addr = ee->getPointerToFunction(ee->FindFunctionNamed("decode_selective"));
    auto native_decode = (void (*)(int32_t*, int32_t*, int32_t))func_addr;

    size_t TOTAL_TUPLES = 100000000;
    std::vector<int32_t> in_buffer(TOTAL_TUPLES, 0);
    std::vector<int32_t> out_buffer(TOTAL_TUPLES, 0);

    for(size_t i = 0; i < TOTAL_TUPLES; ++i) {
        in_buffer[i] = i;
    }

    auto start = std::chrono::high_resolution_clock::now();
    native_decode(in_buffer.data(), out_buffer.data(), TOTAL_TUPLES);
    auto end = std::chrono::high_resolution_clock::now();

    double diff = std::chrono::duration<double>(end - start).count();
    std::cout << "Real SPNED JIT Selective Throughput: " << (TOTAL_TUPLES / 1000000.0) / diff << " MT/s\n";
    return 0;
}

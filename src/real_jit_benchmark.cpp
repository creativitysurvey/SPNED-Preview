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
    
    // 解析底层的 LLVM IR 循环
    std::unique_ptr<Module> module = parseIRFile("spned_loop.ll", err, context);
    
    // 启动 LLVM 真实的 JIT 引擎，开启最高级别 O3 机器码优化
    std::string err_str;
    ExecutionEngine *ee = EngineBuilder(std::move(module))
        .setErrorStr(&err_str)
        .setEngineKind(EngineKind::JIT)
        .setOptLevel(CodeGenOpt::Aggressive)
        .create();
    ee->finalizeObject();

    // 获取 JIT 编译出的物理内存函数指针
    void* func_addr = ee->getPointerToFunction(ee->FindFunctionNamed("decode_loop"));
    auto native_decode = (void (*)(int32_t*, int32_t*, int32_t))func_addr;

    // 准备 1 亿条真实数据缓冲区
    size_t TOTAL_TUPLES = 100000000;
    std::vector<int32_t> in_buffer(TOTAL_TUPLES, 8848);
    std::vector<int32_t> out_buffer(TOTAL_TUPLES, 0);

    // 记录真正 JIT 机器码的执行耗时
    auto start = std::chrono::high_resolution_clock::now();
    native_decode(in_buffer.data(), out_buffer.data(), TOTAL_TUPLES);
    auto end = std::chrono::high_resolution_clock::now();

    double diff = std::chrono::duration<double>(end - start).count();
    std::cout << "Real SPNED JIT Throughput: " << (TOTAL_TUPLES / 1000000.0) / diff << " MT/s\n";
    return 0;
}

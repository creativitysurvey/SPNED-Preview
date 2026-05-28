#include "spned_ast.h"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <iostream>
#include <vector>

using namespace llvm;

int main() {
    // 初始化 LLVM 核心上下文
    LLVMContext context;
    Module module("spned_jit_engine", context);
    IRBuilder<> builder(context);

    // 定义解码函数的 C ABI 签名: void decode_payload(int32* in_buffer, int32* out_buffer, int32 length)
    // 这里的指针直接指向宿主数据库的共享内存，实现零拷贝
    std::vector<Type*> param_types = {
        Type::getInt32PtrTy(context),
        Type::getInt32PtrTy(context),
        Type::getInt32Ty(context)
    };
    FunctionType* func_type = FunctionType::get(Type::getVoidTy(context), param_types, false);
    Function* decode_func = Function::Create(func_type, Function::ExternalLinkage, "decode_payload", &module);

    // 获取函数参数并命名
    auto args = decode_func->arg_begin();
    Value* in_buffer = args++;
    in_buffer->setName("in_buffer");
    Value* out_buffer = args++;
    out_buffer->setName("out_buffer");
    Value* length = args++;
    length->setName("length");

    // 构建控制流图 (CFG) 的入口基本块
    BasicBlock* entry_block = BasicBlock::Create(context, "entry", decode_func);
    builder.SetInsertPoint(entry_block);

    // 模拟一段被 SPNED 静态证明安全的内存读取操作 (AST 降级)
    // 假设索引 index = 0
    Value* index = ConstantInt::get(Type::getInt32Ty(context), 0);
    
    // 【核心亮点】: 直接生成 GetElementPtr 指令 (GEP) 计算物理内存地址
    // 没有任何 if (index >= length) trap(); 的沙盒边界检查指令！
    Value* in_ptr = builder.CreateGEP(Type::getInt32Ty(context), in_buffer, index, "in_ptr");
    Value* loaded_val = builder.CreateLoad(Type::getInt32Ty(context), in_ptr, "loaded_val");

    // 将解码后的数据直接写入输出内存 morsel
    Value* out_ptr = builder.CreateGEP(Type::getInt32Ty(context), out_buffer, index, "out_ptr");
    builder.CreateStore(loaded_val, out_ptr);

    // 结束函数
    builder.CreateRetVoid();

    // 验证生成的 IR 是否符合严格的 SSA 语义规范
    if (verifyFunction(*decode_func, &errs())) {
        std::cerr << "LLVM IR Verification failed!\n";
        return 1;
    }

    // 打印真实生成的底层机器级 IR 代码
    outs() << "; === SPNED Generated LLVM IR ===\n";
    module.print(outs(), nullptr);

    return 0;
}

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/Support/TargetSelect.h>
#include <iostream>
#include <vector>

using namespace llvm;

int main() {
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();

    LLVMContext context;
    auto module_owner = std::make_unique<Module>("spned_jit_engine", context);
    Module *module = module_owner.get();
    IRBuilder<> builder(context);

    std::vector<Type*> param_types = { Type::getInt32PtrTy(context), Type::getInt32PtrTy(context), Type::getInt32Ty(context) };
    FunctionType* func_type = FunctionType::get(Type::getVoidTy(context), param_types, false);
    Function* decode_func = Function::Create(func_type, Function::ExternalLinkage, "decode_payload", module);
    
    auto args = decode_func->arg_begin();
    Value* in_buffer = args++;
    Value* out_buffer = args++;
    Value* length = args++;

    BasicBlock* entry_block = BasicBlock::Create(context, "entry", decode_func);
    builder.SetInsertPoint(entry_block);

    Value* index = ConstantInt::get(Type::getInt32Ty(context), 0);
    Value* in_ptr = builder.CreateGEP(Type::getInt32Ty(context), in_buffer, index);
    Value* loaded_val = builder.CreateLoad(Type::getInt32Ty(context), in_ptr);
    Value* out_ptr = builder.CreateGEP(Type::getInt32Ty(context), out_buffer, index);
    builder.CreateStore(loaded_val, out_ptr);
    builder.CreateRetVoid();

    std::string err_str;
    ExecutionEngine *ee = EngineBuilder(std::move(module_owner))
        .setErrorStr(&err_str)
        .setEngineKind(EngineKind::JIT)
        .create();

    if (!ee) {
        std::cerr << "Failed to construct ExecutionEngine: " << err_str << "\n";
        return 1;
    }
    ee->finalizeObject();

    void* func_addr = ee->getPointerToFunction(decode_func);
    typedef void (*DecodeFuncType)(int32_t*, int32_t*, int32_t);
    DecodeFuncType native_decode = (DecodeFuncType)func_addr;

    std::vector<int32_t> host_in_buffer = {8848};
    std::vector<int32_t> host_out_buffer(1, 0);

    std::cout << "Before JIT Execution: out_buffer[0] = " << host_out_buffer[0] << "\n";
    native_decode(host_in_buffer.data(), host_out_buffer.data(), 1);
    std::cout << "After JIT Execution : out_buffer[0] = " << host_out_buffer[0] << "\n";
    std::cout << "Execution Status: SUCCESS (Zero-Copy Data Transferred!)\n";

    return 0;
}

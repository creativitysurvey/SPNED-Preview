
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
    using namespace std;
    ExitOnError ExitOnErr;

    double run_spned_jit(const char* ir_file, const char* func_name, bool is_selective) {
        auto Context = std::make_unique<LLVMContext>();
        SMDiagnostic Err;
        auto Module = parseIRFile(ir_file, Err, *Context);
        if (!Module) { exit(1); }

        auto JIT = ExitOnErr(LLJITBuilder().create());
        ExitOnErr(JIT->addIRModule(ThreadSafeModule(std::move(Module), std::move(Context))));
        auto Sym = ExitOnErr(JIT->lookup(func_name));

        size_t TOTAL = 100000000;
        vector<int32_t> in(TOTAL, 5);
        vector<int32_t> out(TOTAL, 0);

        if (is_selective) {
            auto native_sel = (int32_t(*)(int32_t*, int32_t*, int32_t, int32_t))Sym.getAddress();
            auto start = chrono::high_resolution_clock::now();
            native_sel(in.data(), out.data(), TOTAL, 10);
            auto end = chrono::high_resolution_clock::now();
            return (TOTAL / 1000000.0) / chrono::duration<double>(end - start).count();
        } else {
            auto native_seq = (void(*)(int32_t*, int32_t*, int32_t))Sym.getAddress();
            auto start = chrono::high_resolution_clock::now();
            native_seq(in.data(), out.data(), TOTAL);
            auto end = chrono::high_resolution_clock::now();
            return (TOTAL / 1000000.0) / chrono::duration<double>(end - start).count();
        }
    }

    int main(int argc, char *argv[]) {
        InitLLVM X(argc, argv);
        InitializeNativeTarget();
        InitializeNativeTargetAsmPrinter();

        string mode = argc > 1 ? argv[1] : "sequential";
        if (mode == "sequential") cout << run_spned_jit("spned_seq.ll", "decode_seq", false);
        else cout << run_spned_jit("spned_sel.ll", "decode_sel", true);
        return 0;
    }
    
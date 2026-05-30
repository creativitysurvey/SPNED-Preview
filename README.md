<img width="1007" height="440" alt="image" src="https://github.com/creativitysurvey/SPNED-Preview/blob/main/cover.png" />


# 🚀 SPNED: Static-Proof Native Execution of Decoders

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Throughput](https://img.shields.io/badge/Peak%20Throughput-1200%2B%20MT%2Fs-brightgreen.svg)]()
[![Verification](https://img.shields.io/badge/Static%20Verification-0.683%20%CE%BCs-blue.svg)]()

> **Notice:** This repository contains the **Preview Edition** of the SPNED framework artifact. It includes the unified end-to-end toolchain automation scripts and pure C/C++ execution baselines. The core C++ Interval Abstract Interpretation Engine and unconstrained ORC JIT pipeline are strictly maintained in the **SPNED-Core-Pro** private repository.

## 📊 Physical Architecture Evaluation (Real-Time Artifact)

Our multi-core scaling experiments and physical measurements systematically expose the architectural limitations of WebAssembly sandboxes in memory-intensive ingestion pipelines.

<img width="1007" height="440" alt="image" src="https://github.com/creativitysurvey/SPNED-Preview/blob/main/cover.png" />

### Architectural Highlights
- **Mathematical Bound Safety:** The pure C++ abstract interpretation verification guarantees 100% structural safety and O(N) termination with a negligible **0.683 microseconds** planning latency for fundamental operators.
- **Zero-Copy JIT Bypass:** Eliminates the batch size overhead tax (Sandbox Tax). SPNED achieves latency as low as **0.38 ns/tuple** at large batch sizes, showcasing near-perfect L1/L2 cache affinity.
- **Selective Throughput Dominance:** In predicate-injected selective scanning, the unconstrained JIT engine decisively outperforms the Wasmtime sandbox context-switching limitations (e.g., 739 MT/s vs 604 MT/s).

---

## 📂 Repository Structure (Preview Edition)
- `/src`: The micro-compiler frontend (`spned_micro_compiler.py`) and pure C execution samples.
- `/include`: Core architectural headers and baseline C++ interfaces.
- `/scripts`: The unified end-to-end toolchain pipeline and automated artifact evaluation scripts (`run_all.sh`).
- `/benchmarks_logs`: Raw physical evaluation summaries demonstrating absolute throughput bounds.

---

## 💎 Unlock SPNED-Core-Pro

Designed for distributed systems researchers targeting top-tier conferences and infrastructure engineers building production-grade ingestion layers. 

**The Pro Edition Exclusively Includes:**
1. **Full C++ Source Code:** The complete Interval-Domain Abstract Interpretation Engine and Unified End-to-End Compiler (AST -> LLVM IR).
2. **Academic Technical Report:** Formal proofs, False Rejection Rate (FRR) mathematical bounds, and Morsel-driven execution strategy.
3. **Artifact Automation Scripts:** Full access to the experimental dependencies and raw dataset generators.

### 🔗 Access Protocol

To support the ongoing research in distributed systems and acquire full repository access:

* **International (USD):** https://github.com/creativitysurvey/SPNED-Preview/issues/2 - $1500
* **Domestic (CNY):** https://github.com/creativitysurvey/SPNED-Preview/issues/1 - ¥9000

Email to prof.liwei@gmail.com after your payment. Upon completing the sponsorship, you will receive a direct collaborator invitation to the private `SPNED-Core-Pro` repository within 12 hours.

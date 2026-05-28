# 🚀 SPNED: Static-Proof Native Execution of Decoders

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Throughput](https://img.shields.io/badge/Throughput-2674%20MT%2Fs-brightgreen.svg)]()
[![Verification](https://img.shields.io/badge/Static%20Verification-0.478%20%CE%BCs-blue.svg)]()

> **Notice:** This repository contains the **Preview Edition** of the SPNED framework artifact. It includes the unified end-to-end toolchain automation scripts and pure C/C++ execution baselines. The core C++ Interval Abstract Interpretation Engine and unconstrained ORC JIT pipeline are strictly maintained in the **SPNED-Core-Pro** private repository.

## 📊 Physical Architecture Evaluation (Real-Time Artifact)

Our multi-core scaling experiments systematically expose the architectural limitations of WebAssembly sandboxes in memory-intensive ingestion pipelines. 

![SPNED Real Scaling](https://github.com/user-attachments/assets/xxxx-xxxx-xxxx)

*Fig 1: Aggregated throughput comparison under increasing thread contention. While the Wasm bare-metal C API collapses after 8 threads (peaking at 812 MT/s) due to severe isolation overhead, SPNED safely shares verified machine code to achieve a massive **2674.74 MT/s at 64 threads**.*

### Architectural Highlights
- **Mathematical Bound Safety:** The pure C++ abstract interpretation verification guarantees $100\%$ structural safety and $\mathcal{O}(N)$ termination with a negligible **0.478 microseconds** planning latency.
- **Zero-Copy JIT Bypass:** Eliminates the batch size overhead tax (Sandbox Tax). SPNED achieves latency as low as **0.76 ns/tuple** at large batch sizes, decisively outperforming Wasmtime's context-switching limitations.

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
1. **Full C++ Source Code:** The complete Interval-Domain Abstract Interpretation Engine and Unified End-to-End Compiler (AST $\to$ LLVM IR).
2. **Academic Technical Report:** Formal proofs, False Rejection Rate (FRR) mathematical bounds, and Morsel-driven execution strategy.
3. **Artifact Automation Scripts:** Full access to the experimental dependencies and raw dataset generators.

### 🔗 Access Protocol

To support the ongoing research in distributed systems and acquire full repository access:

* **International (USD):** Sponsor **$150** via PayPal directly to `[您的PayPal注册邮箱]`. *(Please include your GitHub Username in the transaction note).*
* **Domestic (CNY):** [Insert your Afdian/Xianyu Link] - ¥999

Upon completing the sponsorship, you will receive a direct collaborator invitation to the private `SPNED-Core-Pro` repository within 12 hours.

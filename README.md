# x86-to-C-Interface-Programming-Project
## Group Member/s
- Kent Xavier Lopez
---
## About this project

This project implements the same numerical kernel twice — once in C and once
in hand-written x86-64 assembly — in order to directly compare their
performance and confirm that a low-level assembly implementation can call
and interoperate correctly with a C program under the Windows x64 calling
convention.

The kernel computes the Euclidean distance between corresponding points from
two 2D coordinate vectors:

```
Z[i] = sqrt( (X2[i] - X1[i])^2 + (Y2[i] - Y1[i])^2 )
```

Given scalar input `n` (the vector length) and five single-precision float
vectors `X1, X2, Y1, Y2` (inputs) and `Z` (output), both versions:

- Use **functional scalar SIMD registers** (XMM0/XMM1) and **scalar SIMD
  floating-point instructions** rather than plain scalar float arithmetic —
  the C version through SSE intrinsics (`_mm_load_ss`, `_mm_sub_ss`,
  `_mm_mul_ss`, `_mm_add_ss`, `_mm_sqrt_ss`, `_mm_store_ss`), the assembly
  version directly (`movss`, `subss`, `mulss`, `addss`, `sqrtss`).
- Are called from a single C `main()` that initializes the input vectors,
  times each kernel version independently, verifies that the assembly
  kernel's output matches the C kernel's output within a small floating-point
  tolerance, and prints the first 10 elements of `Z` for both.
- Are benchmarked across three vector sizes (2^20, 2^24, 2^28), each averaged
  over 30 runs, timing only the kernel call itself (not allocation or vector
  initialization).

The goal is both a functional demonstration (C correctly calling into
assembly, and vice versa, following the calling convention) and a
performance comparison (how much overhead, if any, the C compiler introduces
relative to hand-tuned assembly for the same scalar SIMD workload).

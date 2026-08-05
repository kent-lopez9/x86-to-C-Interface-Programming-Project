# x86-to-C-Interface-Programming-Project
## Group Member/s
- Kent Xavier Lopez
---
## About this project

This project implements the same numerical kernel twice, once in C and once
in x86-64 assembly language to directly compare their
performance and confirm that a low-level assembly implementation can call
and cooperate correctly with a C program under the Windows x64 calling
convention.

The kernel calculates the distances between the coordinate points across two vectors:

```
Z[i] = sqrt((X2[i] - X1[i])^2 + (Y2[i] - Y1[i])^2)
```

Given scalar input `n` (the vector length) and five single-precision float
vectors `X1, X2, Y1, Y2` (inputs) and `Z` (output), both versions:

- Use **functional scalar SIMD registers** (XMM0/XMM1) and **scalar SIMD
  floating-point instructions** rather than plain scalar float arithmetic.
  The C version implements this via SSE intrinsics such as `_mm_load_ss`, `_mm_sub_ss`,
  `_mm_mul_ss`, `_mm_add_ss`, `_mm_sqrt_ss`, `_mm_store_ss`. Similarly, the assembly
  version  executes these operations directly using `movss`, `subss`, `mulss`, `addss`, `sqrtss`.
- Called from a single C `main()` that initializes the input vectors,
  times each kernel version independently, verifies that the assembly
  kernel's output matches the C kernel's output within a small floating-point
  tolerance, and prints the first 10 elements of `Z` for both.
- Benchmarked across three vector sizes (2^20, 2^24, 2^28), each averaged
  over 30 runs, timing only the kernel call itself (not allocation or vector
  initialization).

The goal is both a functional demonstration (C correctly calling into
assembly, and vice versa, following the calling convention) and a
performance comparison (how much overhead, if any, the C compiler introduces
relative to hand-tuned assembly for the same scalar SIMD workload).

## How to run this project from GitHub

1. On the repository page, click the green **`<> Code`** button and choose **Open with Visual Studio**.
2. This opens Visual Studio 2022 and prompts you to **clone the repository** and make sure to change the destination path to an empty folder on your local machine before confirming the cloning.
3. Once cloned, Visual Studio will open **`LBYARCH.sln`**. In **Solution Explorer → Source Files**, you'll find `kernel_asm.asm` and `main.c` already in place.
4. Run the project (`Ctrl+F5`, or the green **Local Windows Debugger** button) — `main.c` 

> Note: NASM must be installed, and its path must match what's set in `kernel.asm`'s Custom Build Tool command line (Properties → Custom Build Tool → Command Line). If you cloned this to a machine where NASM isn't at `c:\nasm\nasm.exe`, update that path first, or the `.asm` file won't build.

## Important Files

| File | Short Description |
|---|---|
| `main.c` | C kernel |
| `kernel.asm` | x86-64 NASM kernel |
| `LBYARCH.sln` | Solution file of both C and Asm kernels |
| `LBYARCH.exe` | Execution file of the main.c output under x64/Debug Folder |

## i. Comparative Execution Time and Analysis of the Kernels' Performance
| Vector Size (n) | C Kernel Avg Time (s) | x86-64 Asm Avg Time (s) | Speedup (C / Asm) |
|---|---|---|---|
| 2^20 (1,048,576) | 0.003036 | 0.001139 | 2.67x |
| 2^24 (16,777,216) | 0.043588 | 0.017543 | 2.48x |
| 2^28 (268,435,456) | 0.718063 | 0.292320 | 2.46x |

> Note: values are based on the video because it can have a minor change in value each time it is executed.

**Correctness Validation: PASSED** at all three vector sizes. The first 10 elements
of `Z` from the C kernel and the assembly kernel match exactly at every size tested.

**Kernels' Performance Analysis**

The table above displays that the x86-64 assembly kernel consistently performs about 2.4x to 2.6x faster than the C kernel across all vector sizes. This gap exists because hand-written assembly uses the CPU's internal storage registers more efficiently. While the C compiler generates extra setup instructions and occasionally moves temporary values back and forth
between the system memory and the processor, the assembly code is more efficient at performing calculations directly. This minimal instruction count allows the processor to execute the loop with far fewer delays.

Additionally, this performance advantage remains steady even when the vector size grows to 2^28 elements. At this scale, the massive dataset exceeds the CPU's built-in cache and relies heavily on system RAM. Because the assembly version maintains its speedup without slowing down to match the C version on larger datasets, it demonstrates that the primary problem
is how fast the CPU can process the arithmetic loop itself rather than how fast it can retrieve data from memory. The compact assembly sequence keeps the CPU's computational units highly active and efficient.

Focusing on the maximum vector size, which is 2^30, running it is practically impossible since this machine doesn't support it. The vector size of 2^30 single-precision floats requires 4GB of memory because allocating the six required arrays takes over 24GB of RAM. Hence, it leads to memory allocation failures on standard 16GB personal computers like mine.
To prevent memory allocation failures and crashes, the maximum test size was set to 2^28 elements.

## ii. Screenshot: C version output + correctness check

_[Insert screenshot here]_

## iii. Screenshot: x86-64 version output + correctness check

_[Insert screenshot here — same console output covers both ii and iii since both
kernels print to the same window; crop/annotate to highlight each section if
your instructor wants them visually separated]_

## iv. Video of source code, compilation, and execution of the C and x86-64 program

[https://www.youtube.com/watch?v=OVwtbPjGtDI]

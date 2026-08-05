#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h> 

// C-linkage protection to prevent C++ linker errors in Visual Studio
#ifdef __cplusplus
extern "C" {
#endif
    void distance_c(size_t n, const float* x1, const float* x2, const float* y1, const float* y2, float* z);
    void distance_asm(size_t n, const float* x1, const float* x2, const float* y1, const float* y2, float* z);
#ifdef __cplusplus
}
#endif

// C kernel implementation
void distance_c(size_t n, const float* x1, const float* x2, const float* y1, const float* y2, float* z) {
    for (size_t i = 0; i < n; i++) {
        float diff_x = x2[i] - x1[i];
        float diff_y = y2[i] - y1[i];
        z[i] = sqrtf((diff_x * diff_x) + (diff_y * diff_y));
    }
}

// Verification helper
int verify_results(size_t n, const float* z_c, const float* z_asm) {
    for (size_t i = 0; i < n; i++) {
        if (fabsf(z_c[i] - z_asm[i]) > 1e-4f) {
            return 0;
        }
    }
    return 1;
}

int main() {
    // Sanity check using example data
    float test_x1[] = { 1.5f, 4.0f, 3.5f, 2.0f };
    float test_x2[] = { 3.0f, 2.5f, 2.5f, 1.0f };
    float test_y1[] = { 4.0f, 3.0f, 3.5f, 3.0f };
    float test_y2[] = { 2.0f, 2.5f, 1.0f, 1.5f };
    float test_z_c[4] = { 0 };
    float test_z_asm[4] = { 0 };

    distance_c(4, test_x1, test_x2, test_y1, test_y2, test_z_c);
    distance_asm(4, test_x1, test_x2, test_y1, test_y2, test_z_asm);

    printf("Sanity check with Example\n");
    printf("X1 -> 1.5, 4.0, 3.5, 2.0\n");
    printf("X2 -> 3.0, 2.5, 2.5, 1.0\n");
    printf("Y1 -> 4.0, 3.0, 3.5, 3.0\n");
    printf("Y2 -> 2.0, 2.5, 1.0, 1.5\n");
    printf("Expected Z:  2.5, 1.58113883, 2.692582404, 1.802775638\n");
    printf("C output:   ");
    for (int i = 0; i < 4; i++) printf("%.8f ", test_z_c[i]);
    printf("\nAssembly:   ");
    for (int i = 0; i < 4; i++) printf("%.8f ", test_z_asm[i]);
    printf("\n");

    // Benchmarking the sizes of 2^20, 2^24, and 2^28
    size_t sizes[] = { 1ULL << 20, 1ULL << 24, 1ULL << 28 };
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]); //get no of sizes
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency); //get tick rate 

	// loop through each size and perform benchmarking
    for (int s = 0; s < num_sizes; s++) {
        size_t n = sizes[s];
        printf("====================================================\n");
        printf("Vector Size n = %llu (2^%d)\n", n, (s == 0) ? 20 : ((s == 1) ? 24 : 28));

        float* x1 = (float*)malloc(n * sizeof(float)); // X1
        float* x2 = (float*)malloc(n * sizeof(float)); // X2
        float* y1 = (float*)malloc(n * sizeof(float)); // Y1
        float* y2 = (float*)malloc(n * sizeof(float)); // Y2
        float* z_c = (float*)malloc(n * sizeof(float)); // Z output for C
		float* z_asm = (float*)malloc(n * sizeof(float)); // Z output for Assembly

		// Display if memory allocation fails for any of the arrays
        if (!x1 || !x2 || !y1 || !y2 || !z_c || !z_asm) {
            printf("Memory allocation failed for size %llu\n", n);
            free(x1); free(x2); free(y1); free(y2); free(z_c); free(z_asm);
			continue; //move to next size if memory allocation fails
        }

        // Initialize vectors
        for (size_t i = 0; i < n; i++) {
            x1[i] = (float)rand() / RAND_MAX * 10.0f;
            x2[i] = (float)rand() / RAND_MAX * 10.0f;
            y1[i] = (float)rand() / RAND_MAX * 10.0f;
            y2[i] = (float)rand() / RAND_MAX * 10.0f;
            z_c[i] = 0.0f;
            z_asm[i] = 0.0f;
        }

        // Time C Version (30 runs)
        LARGE_INTEGER start, end;
        double total_time_c = 0.0;
        // run < 30 times to get ave. execution time
        for (int run = 0; run < 30; run++) {
            QueryPerformanceCounter(&start); //start clock
            distance_c(n, x1, x2, y1, y2, z_c);
            QueryPerformanceCounter(&end); // stop clock
            total_time_c += (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart; // add elapsed secs
        }
        double avg_time_c = total_time_c / 30.0; // average over the 30 runs

        // Time Assembly Version (30 runs)
        double total_time_asm = 0.0;
        // run < 30 times to get ave. execution time
        for (int run = 0; run < 30; run++) {
            QueryPerformanceCounter(&start); //start clock
            distance_asm(n, x1, x2, y1, y2, z_asm);
            QueryPerformanceCounter(&end); // stop clock
            total_time_asm += (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart; // add elapsed secs
        }
        double avg_time_asm = total_time_asm / 30.0; // average over the 30 runs

        //print all average execution time of C and Asm
        printf("Average execution time over 30 runs:\n");
        printf("  C Kernel: %f seconds\n", avg_time_c);
        printf("  x86-64 Assembly: %f seconds\n", avg_time_asm);

        // sanity check to check the correctness of the output
        int correct = verify_results(n, z_c, z_asm);
        printf("Correctness Validation: %s\n", correct ? "PASSED" : "FAILED");

        // Display first 10 Z (C) output
        printf("\nFirst 10 elements of Z (C):\n");
        for (int i = 0; i < 10 && i < (int)n; i++) {
            printf("  Z[%d] = %.6f\n", i, z_c[i]);
        }

        // Display first 10 Z (Asm) output
        printf("\nFirst 10 elements of Z (Assembly):\n");
        for (int i = 0; i < 10 && i < (int)n; i++) {
            printf("  Z[%d] = %.6f\n", i, z_asm[i]);
        }

        free(x1); free(x2); free(y1); free(y2); free(z_c); free(z_asm); // release size memory
    }
    return 0;
}
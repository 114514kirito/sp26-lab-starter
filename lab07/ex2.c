#include "ex2.h"
#include <omp.h>
#include <immintrin.h>
#include <stdio.h>
double dotp_naive(double* x, double* y, int arr_size) {
    double global_sum = 0.0;
    for (int i = 0; i < arr_size; i++)
        global_sum += x[i] * y[i];
    return global_sum;
}

// Critical Keyword
double dotp_critical(double* x, double* y, int arr_size) {
    double global_sum = 0.0;
    // TODO: Implement this function
    #pragma omp parallel for
    for (int i = 0; i < arr_size; i++) {
        double product = x[i] * y[i];
        #pragma omp critical
        {
            global_sum += product;
        }
    }
    // Use the critical keyword here!
    return global_sum;
}

// Reduction Keyword
double dotp_reduction(double* x, double* y, int arr_size) {
    double global_sum = 0.0;
    // TODO: Implement this functiono
    #pragma omp parallel for reduction(+:global_sum)
    for (int i = 0; i < arr_size; i++) {
        global_sum += x[i] * y[i];
    }
    // Use the reduction keyword here!
    return global_sum;
}

// Manual Reduction
double dotp_manual_reduction(double* x, double* y, int arr_size) {
    double global_sum = 0.0;
    // TODO: Implement this function
    // Do NOT use the `reduction` directive here!
    #pragma omp parallel
    {
        double local_sum =0;

        // TODO: Parallel Section
        __m256d local_sum_vec = _mm256_setzero_pd(); // Initialize a vector to hold the local sum for each thread

        #pragma omp for
        for (int i = 0; i < arr_size/4 *4; i+=4) {
            __m256d x_vec = _mm256_loadu_pd(&x[i]); // Load 4 doubles from x into a vector
            __m256d y_vec = _mm256_loadu_pd(&y[i]); // Load 4 doubles from y into a vector
            __m256d product_vec = _mm256_mul_pd(x_vec, y_vec); // Element-wise multiply the two vectors
            local_sum_vec = _mm256_add_pd(local_sum_vec, product_vec); // Accumulate the products into the local sum vector
        }
        double temp[4];
        _mm256_storeu_pd((__m256d*)temp, local_sum_vec);
         // Store the local sum vector
        local_sum += temp[0] + temp[1] + temp[2] + temp[3];
        // Horizontally add the elements in the local sum vector to get the total local sum for this thread

        for (int i = arr_size/4 *4; i < arr_size; i++) {
            local_sum += x[i] * y[i];
        }




        // TODO: Critical Section
        #pragma omp critical
        {
            global_sum += local_sum;
        }
    }
    return global_sum;
}

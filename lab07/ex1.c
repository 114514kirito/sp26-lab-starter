#include <time.h>
#include <stdio.h>
#include <x86intrin.h>
#include "ex1.h"

long long int sum(int vals[NUM_ELEMS]) {
    clock_t start = clock();

    long long int sum = 0;
    for(unsigned int w = 0; w < OUTER_ITERATIONS; w++) {
        for(unsigned int i = 0; i < NUM_ELEMS; i++) {
            if(vals[i] >= 128) {
                sum += vals[i];
            }
        }
    }
    clock_t end = clock();
    printf("Time taken: %Lf s\n", (long double)(end - start) / CLOCKS_PER_SEC);
    return sum;
}

long long int sum_unrolled(int vals[NUM_ELEMS]) {
    clock_t start = clock();
    long long int sum = 0;

    for(unsigned int w = 0; w < OUTER_ITERATIONS; w++) {
        for(unsigned int i = 0; i < NUM_ELEMS / 4 * 4; i += 4) {
            if(vals[i] >= 128) sum += vals[i];
            if(vals[i + 1] >= 128) sum += vals[i + 1];
            if(vals[i + 2] >= 128) sum += vals[i + 2];
            if(vals[i + 3] >= 128) sum += vals[i + 3];
        }

        // TAIL CASE, for when NUM_ELEMS isn't a multiple of 4
        // NUM_ELEMS / 4 * 4 is the largest multiple of 4 less than NUM_ELEMS
        // Order is important, since (NUM_ELEMS / 4) effectively rounds down first
        for(unsigned int i = NUM_ELEMS / 4 * 4; i < NUM_ELEMS; i++) {
            if (vals[i] >= 128) {
                sum += vals[i];
            }
        }
    }
    clock_t end = clock();
    printf("Time taken: %Lf s\n", (long double)(end - start) / CLOCKS_PER_SEC);
    return sum;
}

long long int sum_simd(int vals[NUM_ELEMS]) {
    clock_t start = clock();
    __m128i _127 = _mm_set1_epi32(127); // This is a vector with 127s in it... Why might you need this?
    long long int result = 0; // This is where you should put your final result!
    /* DO NOT MODIFY ANYTHING ABOVE THIS LINE (in this function) */

    for(unsigned int w = 0; w < OUTER_ITERATIONS; w++) {
        /* YOUR CODE GOES HERE */
        __m128i sum_vec = _mm_setzero_si128(); // This is a vector initialized to all 0s, you can use this to accumulate your sums in SIMD form
        for(unsigned int i = 0; i < NUM_ELEMS / 4 * 4; i += 4) {
            __m128i v = _mm_loadu_si128((__m128i*)(vals + i)); // Load 4 integers from vals into a vector
            __m128i mask = _mm_cmpgt_epi32(v, _127);
             v= _mm_and_si128 (v, mask); // Zero out elements of v that are less than 128, since we only want to sum values >= 128
            sum_vec = _mm_add_epi32(sum_vec, v); // Add the values in v to sum_vec, but only where the mask is true (i.e. where vals[i] >= 128)
             // Compare each element to 127, creating a mask

            // Now you need to horizontally add the elements in filtered and add to result
            // You can use _mm_hadd_epi32 for this, but be careful since it only adds adjacent pairs
        }
         int temp[4];
         _mm_storeu_si128((__m128i*)temp, sum_vec);
        result += temp[0] + temp[1] + temp[2] + temp[3];
        for(unsigned int i = NUM_ELEMS / 4 * 4; i < NUM_ELEMS; i++) {
            if (vals[i] >= 128) {
                result += vals[i];
            }
        }
        // Handle the tail case for when NUM_ELEMS isn't a multiple of 4

        /* Hint: you'll need a tail case. */
    }

    /* DO NOT MODIFY ANYTHING BELOW THIS LINE (in this function) */
    clock_t end = clock();
    printf("Time taken: %Lf s\n", (long double)(end - start) / CLOCKS_PER_SEC);
    return result;
}

long long int sum_simd_unrolled(int vals[NUM_ELEMS]) {
    clock_t start = clock();
    __m128i _127 = _mm_set1_epi32(127);
    long long int result = 0;
    /* DO NOT MODIFY ANYTHING ABOVE THIS LINE (in this function) */

    for(unsigned int w = 0; w < OUTER_ITERATIONS; w++) {
        __m128i sum_vec_0 = _mm_setzero_si128();
        __m128i sun_vec_1 = _mm_setzero_si128();
        __m128i sum_vec_2 = _mm_setzero_si128();
        __m128i sum_vec_3 = _mm_setzero_si128();
        /* YOUR CODE GOES HERE */
        for(unsigned int i = 0; i < NUM_ELEMS / 16 * 16; i += 16) {
            __m128i v1 = _mm_loadu_si128((__m128i*)(vals + i)); // Load 4 integers from vals into a vector
            __m128i v2 = _mm_loadu_si128((__m128i*)(vals + i + 4));
            __m128i v3 = _mm_loadu_si128((__m128i*)(vals + i + 8));
            __m128i v4 = _mm_loadu_si128((__m128i*)(vals + i + 12));
            __m128i mask1 = _mm_cmpgt_epi32(v1, _127);
            __m128i mask2 = _mm_cmpgt_epi32(v2, _127);
            __m128i mask3 = _mm_cmpgt_epi32(v3, _127);
            __m128i mask4 = _mm_cmpgt_epi32(v4, _127);
             v1= _mm_and_si128 (mask1, v1);
             v2= _mm_and_si128 (mask2, v2);
             v3= _mm_and_si128 (mask3, v3);
             v4= _mm_and_si128 (mask4, v4);
            sum_vec_0 = _mm_add_epi32(sum_vec_0, v1);
            sun_vec_1 = _mm_add_epi32(sun_vec_1, v2);
            sum_vec_2 = _mm_add_epi32(sum_vec_2, v3);
            sum_vec_3 = _mm_add_epi32(sum_vec_3, v4);
        }
         int temp[4];
        _mm_storeu_si128((__m128i*)temp, sum_vec_0);
        result += temp[0] + temp[1] + temp[2] + temp[3];
        _mm_storeu_si128((__m128i*)temp, sun_vec_1);
        result += temp[0] + temp[1] + temp[2] + temp[3];
        _mm_storeu_si128((__m128i*)temp, sum_vec_2);
        result += temp[0] + temp[1] + temp[2] + temp[3];
        _mm_storeu_si128((__m128i*)temp, sum_vec_3);
        result += temp[0] + temp[1] + temp[2] + temp[3];

         for(int i=NUM_ELEMS /16*16;i<NUM_ELEMS;i++){
            if(vals[i]>=128){
                result+=vals[i];
            }
         }
        /* Copy your sum_simd() implementation here, and unroll it */

        /* Hint: you'll need 1 or maybe 2 tail cases here. */
    }

    /* DO NOT MODIFY ANYTHING BELOW THIS LINE (in this function) */
    clock_t end = clock();
    printf("Time taken: %Lf s\n", (long double)(end - start) / CLOCKS_PER_SEC);
    return result;
}

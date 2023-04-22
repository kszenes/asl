#pragma once

#include <immintrin.h>
#include <iostream>
#define N (2)

typedef struct {
    double r;
    double i;
    double j;
    double k;
} quaternion_t;

static quaternion_t add(quaternion_t a, quaternion_t b) {
    quaternion_t r;
    r.r = a.r + b.r;
    r.i = a.i + b.i;
    r.j = a.j + b.j;
    r.k = a.k + b.k;
    return r;
}

static quaternion_t mul(quaternion_t a, quaternion_t b) {
    quaternion_t r;
    r.r = a.r*b.r - a.i*b.i - a.j*b.j - a.k*b.k;
    r.i = a.r*b.i + a.i*b.r + a.j*b.k - a.k*b.j;
    r.j = a.r*b.j - a.i*b.k + a.j*b.r + a.k*b.i;
    r.k = a.r*b.k + a.i*b.j - a.j*b.i + a.k*b.r;
    return r;
}

inline void print(__m256d a) {
    quaternion_t* q = reinterpret_cast<quaternion_t*>(&a);

    std::cout << "r = " << q->r << "; i = " << q->i << "; j = " << q->j << "; k = " << q->k << '\n';
}

static quaternion_t mul_fast(quaternion_t a, quaternion_t b) {
    quaternion_t q_r;
    // Prepare a
    // const __m256d plus = _mm256_set1_pd(1.0);
    // const __m256d minus = _mm256_set1_pd(-1.0);
    
    // const __m256d one_i = _mm256_blend_pd(plus, minus, 0b0101);
    // const __m256d one_j = _mm256_blend_pd(plus, minus, 0b1001);
    // const __m256d one_k = _mm256_blend_pd(plus, minus, 0b0011);
    // print(one_i);

    __m256d one_i = _mm256_set_pd(+1.0, -1.0, 1.0, -1.0); // use shuffle maybe
    __m256d one_j = _mm256_set_pd(-1.0, 1.0, 1.0, -1.0);
    __m256d one_k = _mm256_set_pd(1.0, 1.0, -1.0, -1.0);

    const __m256d a_r = _mm256_set1_pd(a.r);
    const __m256d a_i = _mm256_set1_pd(a.i);
    const __m256d a_j = _mm256_set1_pd(a.j);
    const __m256d a_k = _mm256_set1_pd(a.k);


    __m256d a_mul_i, a_mul_j, a_mul_k;

    a_mul_i = _mm256_mul_pd(one_i, a_i);
    a_mul_j = _mm256_mul_pd(one_j, a_j);
    a_mul_k = _mm256_mul_pd(one_k, a_k);

    // print(a_i);
    // print(a_mul_i);
    // print(a_j);
    // print(a_mul_j);
    // print(a_k);
    // print(a_mul_k);
    // exit(0);

    // Prepare b
    __m256d b_r = _mm256_load_pd(&b.r);
    __m256d b_i = _mm256_permute_pd(b_r, 0b0101);         // 8
    __m256d b_j = _mm256_permute4x64_pd(b_r, 0b01001110); // 78
    __m256d b_k = _mm256_permute4x64_pd(b_r, 0b00011011); // 27

    // print(b_r);
    // print(b_i);
    // print(b_j);
    // print(b_k);
    // exit(0);

    __m256d r = _mm256_setzero_pd();
    r = _mm256_fmadd_pd(a_r, b_r, r);
    r = _mm256_fmadd_pd(a_mul_i, b_i, r);
    r = _mm256_fmadd_pd(a_mul_j, b_j, r);
    r = _mm256_fmadd_pd(a_mul_k, b_k, r);


    _mm256_store_pd(reinterpret_cast<double*>(&q_r), r);
    return q_r;
}
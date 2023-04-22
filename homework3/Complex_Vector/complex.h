#pragma once
#include <cmath>
#include <immintrin.h>
#include <utility>
#include <tuple>

typedef struct{
    double re;
    double im;
} complex_t;

// Complex addition
static complex_t add(complex_t a, complex_t b) {
    complex_t r;
    r.re = a.re + b.re;
    r.im = a.im + b.im;
    return r;
}

// Complex multiplication
static complex_t mul(complex_t a, complex_t b) {
    complex_t r;
    r.re = a.re * b.re - a.im * b.im;
    r.im = a.re * b.im + a.im * b.re;
    return r;
}

// Get real part
static double re(complex_t a) {
    return a.re;
}

// Get imaginary part
static double im(complex_t a) {
    return a.im;
}

// Square
static complex_t sqr(complex_t a) {
    return mul(a, a);
}

// Absolute value
static double abs(complex_t a) {
    return sqrt(a.re * a.re + a.im * a.im);
}

static std::tuple<__m256d, __m256d, __m256d> get_outputs(__m256d m_re, __m256d m_im, __m256d m_two) {
    __m256d m_ore, m_oim, m_opow, m_im2, m_re2, m_min_flag, m_max_flag, m_min, m_max;
    // Real
    m_re2 = _mm256_mul_pd(m_re, m_re);
    m_im2 = _mm256_mul_pd(m_im, m_im);
    
    m_ore = _mm256_sub_pd(m_re2, m_im2);
    m_oim = _mm256_add_pd(m_two, _mm256_mul_pd(m_re, m_im));

    m_min_flag = _mm256_cmp_pd(m_ore, m_oim, _CMP_LE_OQ);
    m_max_flag = _mm256_cmp_pd(m_ore, m_oim, _CMP_GT_OQ);

    m_min = _mm256_blendv_pd(m_ore, m_oim, m_min_flag);
    m_max = _mm256_blendv_pd(m_ore, m_oim, m_max_flag);
    m_opow = _mm256_add_pd(m_re2, m_im2);

    return {m_min, m_max, m_opow};
}
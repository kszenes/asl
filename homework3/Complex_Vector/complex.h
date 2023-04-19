#pragma once
#include <cmath>

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
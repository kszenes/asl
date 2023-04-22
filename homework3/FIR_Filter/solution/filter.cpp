#include "common.h"
#include <immintrin.h>
#include <math.h>

static inline __m256d fabs(__m256d v) {
    __m256d abs_mask = _mm256_set1_pd(-0.0);
    return _mm256_andnot_pd(abs_mask, v);;
}

// Original function
// 31.9K cycles
void slow_performance1(double *x, double* h, double* y, int N, int M) {
  for (int i = 0; i < N - (M - 1); i++) {
    y[i] = 0.0;
    for (int k = 0; k < M; k++) {
      y[i] += (i + k + 1) * h[k] * fabs(x[i + (M - 1) - k]);
    }
  }
}

// Scalar opt
// 15.2K cycles, baseline for speedup
void slow_performance2(double *x, double* h, double* y, int N, int M) {
  int i = 0;
  double h0 = h[0];
  double h1 = h[1];
  double h2 = h[2];
  double h3 = h[3];
  for (; i < N - (M - 1) - 3; i+=4) {
    y[i+0] = 0.0;
    y[i+1] = 0.0;
    y[i+2] = 0.0;
    y[i+3] = 0.0;

    double x_i_0 = fabs(x[i + 0]);
    double x_i_1 = fabs(x[i + 1]);
    double x_i_2 = fabs(x[i + 2]);
    double x_i_3 = fabs(x[i + 3]);
    double x_i_4 = fabs(x[i + 4]);
    double x_i_5 = fabs(x[i + 5]);
    double x_i_6 = fabs(x[i + 6]);

    y[i+0] += (i + 1) * h0 * x_i_3;
    y[i+0] += (i + 2) * h1 * x_i_2;
    y[i+0] += (i + 3) * h2 * x_i_1;
    y[i+0] += (i + 4) * h3 * x_i_0;
    
    y[i+1] += (i + 2) * h0 * x_i_4;
    y[i+1] += (i + 3) * h1 * x_i_3;
    y[i+1] += (i + 4) * h2 * x_i_2;
    y[i+1] += (i + 5) * h3 * x_i_1;
    
    y[i+2] += (i + 3) * h0 * x_i_5;
    y[i+2] += (i + 4) * h1 * x_i_4;
    y[i+2] += (i + 5) * h2 * x_i_3;
    y[i+2] += (i + 6) * h3 * x_i_2;
    
    y[i+3] += (i + 4) * h0 * x_i_6;
    y[i+3] += (i + 5) * h1 * x_i_5;
    y[i+3] += (i + 6) * h2 * x_i_4;
    y[i+3] += (i + 7) * h3 * x_i_3;
  }
  
  /* Handle remainding */
  for (; i < N - (M - 1); i++) {
    y[i] = 0.0;
    for (int k = 0; k < M; k++) {
      y[i] += (i + k + 1) * h[k] * fabs(x[i + (M - 1) - k]);
    }
  }
}

// straightforward vectorization.
// 5K cycles, 3.05x
void slow_performance3(double *x, double* h, double* y, int N, int M) {
  __m256d h0 = _mm256_broadcast_sd(h+0);
  __m256d h1 = _mm256_broadcast_sd(h+1);
  __m256d h2 = _mm256_broadcast_sd(h+2);
  __m256d h3 = _mm256_broadcast_sd(h+3);

  __m256d i_1234 = _mm256_set_pd(4.0,  3.0,  2.0, 1.0);
  __m256d i_2345 = _mm256_set_pd(5.0,  4.0,  3.0, 2.0);
  __m256d i_3456 = _mm256_set_pd(6.0,  5.0,  4.0, 3.0);
  __m256d i_4567 = _mm256_set_pd(7.0,  6.0,  5.0, 4.0);
  
  __m256d _4 = _mm256_set1_pd(4.0);

  int i = 0;
  for (; i < N - (M - 1) - 3; i+=4) {
    __m256d y_i_0;

    __m256d x_i_0 = fabs(_mm256_load_pd(x+i+0));
    __m256d x_i_1 = fabs(_mm256_set_pd(x[i+4],x[i+3],x[i+2],x[i+1]));
    __m256d x_i_2 = fabs(_mm256_set_pd(x[i+5],x[i+4],x[i+3],x[i+2]));
    __m256d x_i_3 = fabs(_mm256_set_pd(x[i+6],x[i+5],x[i+4],x[i+3]));

    y_i_0 = i_4567*h3*x_i_0;
    y_i_0 = i_3456*h2*x_i_1 + y_i_0;
    y_i_0 = i_2345*h1*x_i_2 + y_i_0;
    y_i_0 = i_1234*h0*x_i_3 + y_i_0;

    i_1234 = _mm256_add_pd(i_1234, _4);
    i_2345 = _mm256_add_pd(i_2345, _4);
    i_3456 = _mm256_add_pd(i_3456, _4);
    i_4567 = _mm256_add_pd(i_4567, _4);

    _mm256_store_pd(y+i+0, y_i_0);
  }

  /* Handle remaining */
  for (; i < N - (M - 1); i++) {
    y[i] = 0.0;
    for (int k = 0; k < M; k++) {
      y[i] += (i + k + 1) * h[k] * fabs(x[i + (M - 1) - k]);
    }
  }
}

// Improving loads
// 3.88K cycles, 3.91x
void slow_performance4(double *x, double* h, double* y, int N, int M) {
  __m256d h0 = _mm256_broadcast_sd(h+0);
  __m256d h1 = _mm256_broadcast_sd(h+1);
  __m256d h2 = _mm256_broadcast_sd(h+2);
  __m256d h3 = _mm256_broadcast_sd(h+3);

  __m256d i_1234 = _mm256_set_pd(4.0,  3.0,  2.0, 1.0);
  __m256d i_2345 = _mm256_set_pd(5.0,  4.0,  3.0, 2.0);
  __m256d i_3456 = _mm256_set_pd(6.0,  5.0,  4.0, 3.0);
  __m256d i_4567 = _mm256_set_pd(7.0,  6.0,  5.0, 4.0);
  
  __m256d _4 = _mm256_set1_pd(4.0);

  int i = 0;
  for (; i < N - (M - 1) - 3; i+=4) {
    __m256d y_i_0;

    __m256d x_i_0 = fabs(_mm256_load_pd(x+i+0));
    __m256d x_i_4 = fabs(_mm256_load_pd(x+i+4));
    __m256d x_i_2 = _mm256_permute2f128_pd(x_i_0, x_i_4, 0b00100001);
    __m256d x_i_1 = _mm256_shuffle_pd(x_i_0, x_i_2, 0b0101);
    __m256d x_i_3 = _mm256_shuffle_pd(x_i_2, x_i_4, 0b0101);

    y_i_0 = i_4567*h3*x_i_0;
    y_i_0 = i_3456*h2*x_i_1 + y_i_0;
    y_i_0 = i_2345*h1*x_i_2 + y_i_0;
    y_i_0 = i_1234*h0*x_i_3 + y_i_0;

    i_1234 = _mm256_add_pd(i_1234, _4);
    i_2345 = _mm256_add_pd(i_2345, _4);
    i_3456 = _mm256_add_pd(i_3456, _4);
    i_4567 = _mm256_add_pd(i_4567, _4);

    _mm256_store_pd(y+i+0, y_i_0);
  }

  /* Handle remaining */
  for (; i < N - (M - 1); i++) {
    y[i] = 0.0;
    for (int k = 0; k < M; k++) {
      y[i] += (i + k + 1) * h[k] * fabs(x[i + (M - 1) - k]);
    }
  }
}

// Adding fmas
// 3.27K cycles, 4.64x
void slow_performance5(double *x, double* h, double* y, int N, int M) {
  __m256d h0 = _mm256_broadcast_sd(h+0);
  __m256d h1 = _mm256_broadcast_sd(h+1);
  __m256d h2 = _mm256_broadcast_sd(h+2);
  __m256d h3 = _mm256_broadcast_sd(h+3);

  __m256d i_1234 = _mm256_set_pd(4.0,  3.0,  2.0, 1.0);
  __m256d i_2345 = _mm256_set_pd(5.0,  4.0,  3.0, 2.0);
  __m256d i_3456 = _mm256_set_pd(6.0,  5.0,  4.0, 3.0);
  __m256d i_4567 = _mm256_set_pd(7.0,  6.0,  5.0, 4.0);
  
  __m256d _4 = _mm256_set1_pd(4.0);

  int i = 0;
  for (; i < N - (M - 1) - 3; i+=4) {
    __m256d y_i_0;

    __m256d x_i_0 = fabs(_mm256_load_pd(x+i+0));
    __m256d x_i_4 = fabs(_mm256_load_pd(x+i+4));
    __m256d x_i_2 = _mm256_permute2f128_pd(x_i_0, x_i_4, 0b00100001);
    __m256d x_i_1 = _mm256_shuffle_pd(x_i_0, x_i_2, 0b0101);
    __m256d x_i_3 = _mm256_shuffle_pd(x_i_2, x_i_4, 0b0101);

    y_i_0 = _mm256_mul_pd  (i_4567*h3, x_i_0);
    y_i_0 = _mm256_fmadd_pd(i_3456*h2, x_i_1, y_i_0);
    y_i_0 = _mm256_fmadd_pd(i_2345*h1, x_i_2, y_i_0);
    y_i_0 = _mm256_fmadd_pd(i_1234*h0, x_i_3, y_i_0);

    i_1234 = _mm256_add_pd(i_1234, _4);
    i_2345 = _mm256_add_pd(i_2345, _4);
    i_3456 = _mm256_add_pd(i_3456, _4);
    i_4567 = _mm256_add_pd(i_4567, _4);

    _mm256_store_pd(y+i+0, y_i_0);
  }

  /* Handle remaining */
  for (; i < N - (M - 1); i++) {
    y[i] = 0.0;
    for (int k = 0; k < M; k++) {
      y[i] += (i + k + 1) * h[k] * fabs(x[i + (M - 1) - k]);
    }
  }
}

// Removing unnecesary multiplications in the inner loop
// 3K cycles, 5.05x
void slow_performance6(double *x, double* h, double* y, int N, int M) {
  __m256d h0 = _mm256_broadcast_sd(h+0);
  __m256d h1 = _mm256_broadcast_sd(h+1);
  __m256d h2 = _mm256_broadcast_sd(h+2);
  __m256d h3 = _mm256_broadcast_sd(h+3);

  __m256d i_1234 = _mm256_set_pd(4.0,  3.0,  2.0, 1.0)*h0;
  __m256d i_2345 = _mm256_set_pd(5.0,  4.0,  3.0, 2.0)*h1;
  __m256d i_3456 = _mm256_set_pd(6.0,  5.0,  4.0, 3.0)*h2;
  __m256d i_4567 = _mm256_set_pd(7.0,  6.0,  5.0, 4.0)*h3;
  
  __m256d _4_h0  = _mm256_set1_pd(4.0)*h0;
  __m256d _4_h1  = _mm256_set1_pd(4.0)*h1;
  __m256d _4_h2  = _mm256_set1_pd(4.0)*h2;
  __m256d _4_h3  = _mm256_set1_pd(4.0)*h3;

  int i = 0;
  for (; i < N - (M - 1) - 3; i+=4) {
    __m256d y_i_0;

    __m256d x_i_0 = fabs(_mm256_load_pd(x+i+0));
    __m256d x_i_4 = fabs(_mm256_load_pd(x+i+4));
    __m256d x_i_2 = _mm256_permute2f128_pd(x_i_0, x_i_4, 0b00100001);
    __m256d x_i_1 = _mm256_shuffle_pd(x_i_0, x_i_2, 0b0101);
    __m256d x_i_3 = _mm256_shuffle_pd(x_i_2, x_i_4, 0b0101);

    y_i_0 = _mm256_mul_pd  (i_4567, x_i_0);
    y_i_0 = _mm256_fmadd_pd(i_3456, x_i_1, y_i_0);
    y_i_0 = _mm256_fmadd_pd(i_2345, x_i_2, y_i_0);
    y_i_0 = _mm256_fmadd_pd(i_1234, x_i_3, y_i_0);

    i_1234 = _mm256_add_pd(i_1234, _4_h0);
    i_2345 = _mm256_add_pd(i_2345, _4_h1);
    i_3456 = _mm256_add_pd(i_3456, _4_h2);
    i_4567 = _mm256_add_pd(i_4567, _4_h3);

    _mm256_store_pd(y+i+0, y_i_0);
  }

  /* Handle remaining */
  for (; i < N - (M - 1); i++) {
    y[i] = 0.0;
    for (int k = 0; k < M; k++) {
      y[i] += (i + k + 1) * h[k] * fabs(x[i + (M - 1) - k]);
    }
  }
}

// Unroll one more time to improve ILP
// 2.62K cycles, 5.8x
void slow_performance7(double *x, double* h, double* y, int N, int M) {
  __m256d h0 = _mm256_broadcast_sd(h+0);
  __m256d h1 = _mm256_broadcast_sd(h+1);
  __m256d h2 = _mm256_broadcast_sd(h+2);
  __m256d h3 = _mm256_broadcast_sd(h+3);

  __m256d i_1234 = _mm256_set_pd(4.0,  3.0,  2.0, 1.0)*h0;
  __m256d i_2345 = _mm256_set_pd(5.0,  4.0,  3.0, 2.0)*h1;
  __m256d i_3456 = _mm256_set_pd(6.0,  5.0,  4.0, 3.0)*h2;
  __m256d i_4567 = _mm256_set_pd(7.0,  6.0,  5.0, 4.0)*h3;
  __m256d i_5678 = _mm256_set_pd(8.0,  7.0,  6.0, 5.0)*h0;
  __m256d i_6789 = _mm256_set_pd(9.0,  8.0,  7.0, 6.0)*h1;
  __m256d i_789A = _mm256_set_pd(10.0, 9.0,  8.0, 7.0)*h2;
  __m256d i_89AB = _mm256_set_pd(11.0, 10.0, 9.0, 8.0)*h3;

  __m256d _8_h0  = _mm256_set1_pd(8.0)*h0;
  __m256d _8_h1  = _mm256_set1_pd(8.0)*h1;
  __m256d _8_h2  = _mm256_set1_pd(8.0)*h2;
  __m256d _8_h3  = _mm256_set1_pd(8.0)*h3;

  int i = 0;
  for (; i < N - (M - 1) - 7; i+=8) {
    __m256d y_i_0;
    __m256d y_i_4;

    __m256d x_i_0 = fabs(_mm256_load_pd(x+i+0));
    __m256d x_i_4 = fabs(_mm256_load_pd(x+i+4));
    __m256d x_i_2 = _mm256_permute2f128_pd(x_i_0, x_i_4, 0b00100001);
    __m256d x_i_1 = _mm256_shuffle_pd(x_i_0, x_i_2, 0b0101);
    __m256d x_i_3 = _mm256_shuffle_pd(x_i_2, x_i_4, 0b0101);

    __m256d x_i_8 = fabs(_mm256_load_pd(x+i+8));
    __m256d x_i_6 = _mm256_permute2f128_pd(x_i_4, x_i_8, 0b00100001);
    __m256d x_i_5 = _mm256_shuffle_pd(x_i_4, x_i_6, 0b0101);
    __m256d x_i_7 = _mm256_shuffle_pd(x_i_6, x_i_8, 0b0101);

    y_i_0 = _mm256_mul_pd  (i_4567, x_i_0);
    y_i_0 = _mm256_fmadd_pd(i_3456, x_i_1, y_i_0);
    y_i_0 = _mm256_fmadd_pd(i_2345, x_i_2, y_i_0);
    y_i_0 = _mm256_fmadd_pd(i_1234, x_i_3, y_i_0);

    y_i_4 = _mm256_mul_pd  (i_89AB, x_i_4);
    y_i_4 = _mm256_fmadd_pd(i_789A, x_i_5, y_i_4);
    y_i_4 = _mm256_fmadd_pd(i_6789, x_i_6, y_i_4);
    y_i_4 = _mm256_fmadd_pd(i_5678, x_i_7, y_i_4);

    i_1234 = _mm256_add_pd(i_1234, _8_h0);
    i_2345 = _mm256_add_pd(i_2345, _8_h1);
    i_3456 = _mm256_add_pd(i_3456, _8_h2);
    i_4567 = _mm256_add_pd(i_4567, _8_h3);
    i_5678 = _mm256_add_pd(i_5678, _8_h0);
    i_6789 = _mm256_add_pd(i_6789, _8_h1);
    i_789A = _mm256_add_pd(i_789A, _8_h2);
    i_89AB = _mm256_add_pd(i_89AB, _8_h3);

    _mm256_store_pd(y+i+0, y_i_0);
    _mm256_store_pd(y+i+4, y_i_4);
  }

  /* Handle remaining */
  for (; i < N - (M - 1); i++) {
    y[i] = 0.0;
    for (int k = 0; k < M; k++) {
      y[i] += (i + k + 1) * h[k] * fabs(x[i + (M - 1) - k]);
    }
  }
}


void maxperformance(double *x, double* h, double* y, int N, int M) {
  slow_performance7(x, h, y, N, M);
}

/*
* Called by the driver to register your functions
* Use add_function(func, description) to add your own functions
*/
void register_functions() 
{
  add_function(&slow_performance1, "slow_performance1",1);
  add_function(&slow_performance2, "slow_performance2",1);
  add_function(&slow_performance3, "slow_performance3",1);
  add_function(&slow_performance4, "slow_performance4",1);
  add_function(&slow_performance5, "slow_performance5",1);
  add_function(&slow_performance6, "slow_performance6",1);
  add_function(&slow_performance7, "slow_performance7",1);
  add_function(&maxperformance, "maxperformance",1);
}
#include <immintrin.h>
#include "common.h"
#include "complex.h"

void slow_performance1(quaternion_t x[N], quaternion_t y[N],quaternion_t A[N][N]) {
    for (int i = 0; i < N; i++) {
      for (int j = 0; j < N; j++) {
            A[i][j] = mul(x[i], y[j]);
        }
    }
}

void vectorized(quaternion_t x[N], quaternion_t y[N],quaternion_t A[N][N]) {
  /* Add your final implementation here */
  A[0][0] = mul_fast(x[0], y[0]);
  A[0][1] = mul_fast(x[0], y[1]);
  A[1][0] = mul_fast(x[1], y[0]);
  A[1][1] = mul_fast(x[1], y[1]);

}

void inlined(quaternion_t x[N], quaternion_t y[N],quaternion_t A[N][N]) {
  /* Add your final implementation here */
  // A[0][0] = mul_fast(x[0], y[0]);
  __m256d one_i = _mm256_set_pd(+1.0, -1.0, 1.0, -1.0); // use shuffle maybe
  __m256d one_j = _mm256_set_pd(-1.0, 1.0, 1.0, -1.0);
  __m256d one_k = _mm256_set_pd(1.0, 1.0, -1.0, -1.0);

  __m256d a_r = _mm256_set1_pd(x[0].r);
  __m256d a_i = _mm256_mul_pd(one_i, _mm256_set1_pd(x[0].i));
  __m256d a_j = _mm256_mul_pd(one_j, _mm256_set1_pd(x[0].j));
  __m256d a_k = _mm256_mul_pd(one_k, _mm256_set1_pd(x[0].k));

  __m256d b_r = _mm256_load_pd(&y[0].r);
  __m256d b_i = _mm256_permute_pd(b_r, 0b0101);         // 8
  __m256d b_j = _mm256_permute4x64_pd(b_r, 0b01001110); // 78
  __m256d b_k = _mm256_permute4x64_pd(b_r, 0b00011011); // 27

  __m256d r = _mm256_setzero_pd();
  r = _mm256_fmadd_pd(a_r, b_r, r);
  r = _mm256_fmadd_pd(a_i, b_i, r);
  r = _mm256_fmadd_pd(a_j, b_j, r);
  r = _mm256_fmadd_pd(a_k, b_k, r);
  _mm256_store_pd(reinterpret_cast<double*>(&A[0][0]), r);

  // A[0][1] = mul_fast(x[0], y[1]);
  b_r = _mm256_load_pd(&y[1].r);
  b_i = _mm256_permute_pd(b_r, 0b0101);         // 8
  b_j = _mm256_permute4x64_pd(b_r, 0b01001110); // 78
  b_k = _mm256_permute4x64_pd(b_r, 0b00011011); // 27
  r = _mm256_setzero_pd();
  r = _mm256_fmadd_pd(a_r, b_r, r);
  r = _mm256_fmadd_pd(a_i, b_i, r);
  r = _mm256_fmadd_pd(a_j, b_j, r);
  r = _mm256_fmadd_pd(a_k, b_k, r);
  _mm256_store_pd(reinterpret_cast<double*>(&A[0][1]), r);

  // A[1][0] = mul_fast(x[1], y[0]);
  a_r = _mm256_set1_pd(x[1].r);
  a_i = _mm256_mul_pd(one_i, _mm256_set1_pd(x[1].i));
  a_j = _mm256_mul_pd(one_j, _mm256_set1_pd(x[1].j));
  a_k = _mm256_mul_pd(one_k, _mm256_set1_pd(x[1].k));

  b_r = _mm256_load_pd(&y[0].r);
  b_i = _mm256_permute_pd(b_r, 0b0101);         // 8
  b_j = _mm256_permute4x64_pd(b_r, 0b01001110); // 78
  b_k = _mm256_permute4x64_pd(b_r, 0b00011011); // 27

  r = _mm256_setzero_pd();
  r = _mm256_fmadd_pd(a_r, b_r, r);
  r = _mm256_fmadd_pd(a_i, b_i, r);
  r = _mm256_fmadd_pd(a_j, b_j, r);
  r = _mm256_fmadd_pd(a_k, b_k, r);
  _mm256_store_pd(reinterpret_cast<double*>(&A[1][0]), r);

  // A[1][1] = mul_fast(x[1], y[1]);
  b_r = _mm256_load_pd(&y[1].r);
  b_i = _mm256_permute_pd(b_r, 0b0101);         // 8
  b_j = _mm256_permute4x64_pd(b_r, 0b01001110); // 78
  b_k = _mm256_permute4x64_pd(b_r, 0b00011011); // 27

  r = _mm256_setzero_pd();
  r = _mm256_fmadd_pd(a_r, b_r, r);
  r = _mm256_fmadd_pd(a_i, b_i, r);
  r = _mm256_fmadd_pd(a_j, b_j, r);
  r = _mm256_fmadd_pd(a_k, b_k, r);
  _mm256_store_pd(reinterpret_cast<double*>(&A[1][1]), r);

}

/*
* Called by the driver to register your functions
* Use add_function(func, description) to add your own functions
*/
void register_functions() {
  add_function(&slow_performance1, "slow_performance1",1);
  add_function(&vectorized, "vectorized",1);
  add_function(&inlined, "inlined",1);
}
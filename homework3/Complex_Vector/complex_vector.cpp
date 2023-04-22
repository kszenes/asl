#include <immintrin.h>
#include "common.h"

using namespace std;

void print(__m256d v) {
  double a[4];
  _mm256_store_pd(a, v);
  std::cout << a[0] << '\t' << a[1] << '\t' << a[2] << '\t' << a[3] << '\n';

}


// Precondition: 0 <= y[i] < 1.0
void slow_performance1(complex_t *x, double *y, int n) {
    for (int i = 0; i < n; i++) {
        unsigned int k = floor(4.0*y[i]);
        switch (k) {
            case 0:  y[i] += fmin(re(sqr(x[i])), im(sqr(x[i]))); break;
            case 1:  y[i] += fmax(re(sqr(x[i])), im(sqr(x[i]))); break;
            default: y[i] += pow(abs(x[i]), 2.0); break;
        }
    }
}
void single(complex_t *x, double *y, int n) {
    const __m256d m_one = _mm256_set1_pd(0.25);
    const __m256d m_two = _mm256_set1_pd(0.5);
    // std::cout << "  a[0]  " << '\t' << "  a[2]  " << '\t'<< "  a[1]  " << '\t'<< "  a[3]  " << '\n';
    for (int i = 0; i < n-3; i += 4) {

      // get case flags
      __m256d m_y = _mm256_load_pd(&y[i]);
      __m256d m_case_0 = _mm256_cmp_pd(m_y, m_one, _CMP_LT_OS);
      __m256d m_case_1 = _mm256_xor_pd(_mm256_cmp_pd(m_y, m_two, _CMP_LT_OS), m_case_0);
      __m256d m_case_def = _mm256_cmp_pd(m_y, m_two, _CMP_GE_OS);

      // load inputs
      __m256d m_x0 = _mm256_load_pd(reinterpret_cast<double*>(&x[i]));
      __m256d m_x1 = _mm256_load_pd(reinterpret_cast<double*>(&x[i+2]));

      __m256d m_re = _mm256_unpacklo_pd(m_x0, m_x1);
      __m256d m_im = _mm256_unpackhi_pd(m_x0, m_x1);
      m_re = _mm256_permute4x64_pd(m_re, _MM_SHUFFLE(3, 1, 2, 0));
      m_im = _mm256_permute4x64_pd(m_im, _MM_SHUFFLE(3, 1, 2, 0));

      // compute outputs
      __m256d m_re2 = m_re * m_re;
      __m256d m_ore = _mm256_fnmadd_pd(m_im, m_im, m_re2);
      __m256d m_oim = 2.0 * m_re * m_im;


      __m256d m_min_flag = _mm256_cmp_pd(m_ore, m_oim, _CMP_LE_OQ);
      __m256d m_max_flag = _mm256_cmp_pd(m_ore, m_oim, _CMP_GT_OQ);
      __m256d m_pow = _mm256_fmadd_pd(m_im, m_im, m_re2);

      __m256d m_min = _mm256_blendv_pd(m_oim, m_ore, m_min_flag);
      __m256d m_max = _mm256_blendv_pd(m_oim, m_ore, m_max_flag);

      // combine outputs
      m_min = _mm256_and_pd(m_case_0, m_min);
      m_max = _mm256_and_pd(m_case_1, m_max);
      m_pow = _mm256_and_pd(m_case_def, m_pow);
      m_min = _mm256_or_pd(m_min, m_max);
      m_min = _mm256_or_pd(m_min, m_pow);

      _mm256_store_pd(&y[i], m_y + m_min);
    }
}



void maxperformance(complex_t* x, double* y, int n) {
  /* Add your final implementation here */
}


/*
* Called by the driver to register your functions
* Use add_function(func, description) to add your own functions
*/
void register_functions() {
  add_function(&slow_performance1, "slow_performance1",1);
  add_function(&single, "single",1);
  // add_function(&maxperformance, "maxperformance",1);
}
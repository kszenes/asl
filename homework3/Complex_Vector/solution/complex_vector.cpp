#include <immintrin.h>
#include "common.h"

using namespace std;

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

// Replacing switch with if-else and removing ceil operation by changing conditional to <
void slow_performance2(complex_t *x, double *y, int n) {
    for (int i = 0; i < n; i++) {
        if (y[i] < 0.25) {
            y[i] += fmin(re(sqr(x[i])), im(sqr(x[i])));
        } else if (y[i] < 0.5) {
            y[i] += fmax(re(sqr(x[i])), im(sqr(x[i])));
        } else {
            y[i] += pow(abs(x[i]), 2.0);
        }
    }
}

// Replacing calls to sqr with direct member access and replacing fmin/fmax with min/max
// 4.63K cycles
void slow_performance3(complex_t *x, double *y, int n) {
    for (int i = 0; i < n; i++) {
        if (y[i] < 0.25) {
            y[i] += min(x[i].re*x[i].re - x[i].im*x[i].im, 2.0*x[i].re*x[i].im);
        } else if (y[i] < 0.5) {
            y[i] += max(x[i].re*x[i].re - x[i].im*x[i].im, 2.0*x[i].re*x[i].im);
        } else {
            y[i] += x[i].re*x[i].re + x[i].im*x[i].im;
        }
    }
}

// Straight forward vectorization using AVX
// 2.8K cycles
void slow_performance4(complex_t *x, double *y, int n) {
    int i = 0;
    for (i = 0; i < n - 3; i+= 4) {
        /* Load elements */
        __m256d x_0123_re = _mm256_set_pd(x[i+3].re, x[i+2].re, x[i+1].re, x[i].re);
        __m256d x_0123_im = _mm256_set_pd(x[i+3].im, x[i+2].im, x[i+1].im, x[i].im);
        __m256d y_0123    = _mm256_set_pd(y[i+3], y[i+2], y[i+1], y[i]);

        /* Determine mask based on conditional comparison */
        __m256d mask25_0123 = _mm256_cmp_pd(y_0123, _mm256_set1_pd(0.25), _CMP_LT_OQ);
        __m256d mask50_0123 = _mm256_cmp_pd(y_0123, _mm256_set1_pd(0.50), _CMP_LT_OQ);

        /* Compute intermediate results */
        __m256d x_0123_sqr_re = x_0123_re*x_0123_re - x_0123_im*x_0123_im;
        __m256d x_0123_sqr_im = 2.0*x_0123_re*x_0123_im;
        __m256d t_lt25_0123  = _mm256_min_pd(x_0123_sqr_re, x_0123_sqr_im);
        __m256d t_lt50_0123  = _mm256_max_pd(x_0123_sqr_re, x_0123_sqr_im);
        __m256d t_ge50_0123  = x_0123_re*x_0123_re + x_0123_im*x_0123_im;

        /* Compute final result */
        y_0123 += _mm256_blendv_pd(t_ge50_0123, _mm256_blendv_pd(t_lt50_0123, t_lt25_0123, mask25_0123), mask50_0123);
        _mm256_store_pd(&y[i], y_0123);
    }

}

// Better load of data
// 2.3K cycles
void slow_performance5(complex_t *x, double *y, int n) {
    int i = 0;
    for (i = 0; i < n - 3; i+= 4) {
        /* Load elements */
        __m256d x_01      = _mm256_load_pd(&x[i+0].re);
        __m256d x_23      = _mm256_load_pd(&x[i+2].re);
        __m256d x_0213_re = _mm256_unpacklo_pd(x_01, x_23);
        __m256d x_0213_im = _mm256_unpackhi_pd(x_01, x_23);
        __m256d x_0123_re = _mm256_permute4x64_pd(x_0213_re, _MM_SHUFFLE(3, 1, 2, 0));
        __m256d x_0123_im = _mm256_permute4x64_pd(x_0213_im, _MM_SHUFFLE(3, 1, 2, 0));
        __m256d y_0123    = _mm256_load_pd(&y[i]);

        /* Determine mask based on conditional comparison */
        __m256d mask25_0123 = _mm256_cmp_pd(y_0123, _mm256_set1_pd(0.25), _CMP_LT_OQ);
        __m256d mask50_0123 = _mm256_cmp_pd(y_0123, _mm256_set1_pd(0.50), _CMP_LT_OQ);

        /* Compute intermediate results */
        __m256d x_0123_sqr_re = x_0123_re*x_0123_re - x_0123_im*x_0123_im;
        __m256d x_0123_sqr_im = 2.0*x_0123_re*x_0123_im;
        __m256d t_lt25_0123  = _mm256_min_pd(x_0123_sqr_re, x_0123_sqr_im);
        __m256d t_lt50_0123  = _mm256_max_pd(x_0123_sqr_re, x_0123_sqr_im);
        __m256d t_ge50_0123  = x_0123_re*x_0123_re + x_0123_im*x_0123_im;

        /* Compute final result */
        y_0123 += _mm256_blendv_pd(t_ge50_0123, _mm256_blendv_pd(t_lt50_0123, t_lt25_0123, mask25_0123), mask50_0123);
        _mm256_store_pd(&y[i], y_0123);
    }
}

// Optimize for fmas
// 2.16K cycles
void slow_performance6(complex_t *x, double *y, int n) {
    int i = 0;
    for (i = 0; i < n - 3; i+= 4) {
        /* Load elements */
        __m256d x_01      = _mm256_load_pd(&x[i+0].re);
        __m256d x_23      = _mm256_load_pd(&x[i+2].re);
        __m256d x_0213_re = _mm256_unpacklo_pd(x_01, x_23);
        __m256d x_0213_im = _mm256_unpackhi_pd(x_01, x_23);
        __m256d x_0123_re = _mm256_permute4x64_pd(x_0213_re, _MM_SHUFFLE(3, 1, 2, 0));
        __m256d x_0123_im = _mm256_permute4x64_pd(x_0213_im, _MM_SHUFFLE(3, 1, 2, 0));
        __m256d y_0123    = _mm256_load_pd(&y[i]);

        /* Determine mask based on conditional comparison */
        __m256d mask25_0123 = _mm256_cmp_pd(y_0123, _mm256_set1_pd(0.25), _CMP_LT_OQ);
        __m256d mask50_0123 = _mm256_cmp_pd(y_0123, _mm256_set1_pd(0.50), _CMP_LT_OQ);

        /* Compute intermediate results */
        __m256d x_0123_sqr_re = _mm256_fnmadd_pd(x_0123_im, x_0123_im, _mm256_fmadd_pd(x_0123_re, x_0123_re, y_0123));
        __m256d x_0123_sqr_im = _mm256_fmadd_pd (x_0123_re, x_0123_im, _mm256_fmadd_pd(x_0123_re, x_0123_im, y_0123));
        __m256d t_lt25_0123   = _mm256_min_pd(x_0123_sqr_re, x_0123_sqr_im);
        __m256d t_lt50_0123   = _mm256_max_pd(x_0123_sqr_re, x_0123_sqr_im);
        __m256d t_ge50_0123   = _mm256_fmadd_pd(x_0123_im, x_0123_im, _mm256_fmadd_pd(x_0123_re, x_0123_re, y_0123));

        /* Compute final result */
        y_0123 = _mm256_blendv_pd(t_ge50_0123, _mm256_blendv_pd(t_lt50_0123, t_lt25_0123, mask25_0123), mask50_0123);
        _mm256_store_pd(&y[i], y_0123);
    }
}

// Unroll one more time and minimize operations
// 1.99K Cycles
void slow_performance7(complex_t *x, double *y, int n) {
  int i = 0;
  __m256d sign_mask = _mm256_set1_pd(-0.0);
  for (i = 0; i < n - 7; i+= 8) {
    /* Load elements */
    __m256d x_01      = _mm256_load_pd(&x[i+0].re);
    __m256d x_23      = _mm256_load_pd(&x[i+2].re);
    __m256d x_45      = _mm256_load_pd(&x[i+4].re);
    __m256d x_67      = _mm256_load_pd(&x[i+6].re);
    __m256d x_0213_re = _mm256_unpacklo_pd(x_01, x_23);
    __m256d x_0213_im = _mm256_unpackhi_pd(x_01, x_23);
    __m256d x_4657_re = _mm256_unpacklo_pd(x_45, x_67);
    __m256d x_4657_im = _mm256_unpackhi_pd(x_45, x_67);
    __m256d x_0123_re = _mm256_permute4x64_pd(x_0213_re, _MM_SHUFFLE(3, 1, 2, 0));
    __m256d x_0123_im = _mm256_permute4x64_pd(x_0213_im, _MM_SHUFFLE(3, 1, 2, 0));
    __m256d x_4567_re = _mm256_permute4x64_pd(x_4657_re, _MM_SHUFFLE(3, 1, 2, 0));
    __m256d x_4567_im = _mm256_permute4x64_pd(x_4657_im, _MM_SHUFFLE(3, 1, 2, 0));
    __m256d y_0123    = _mm256_load_pd(&y[i]);
    __m256d y_4567    = _mm256_load_pd(&y[i+4]);
    
    /* Determine mask based on conditional comparison */
    __m256d mask25_0123 = _mm256_cmp_pd(y_0123, _mm256_set1_pd(0.25), _CMP_LT_OQ);
    __m256d mask50_0123 = _mm256_cmp_pd(y_0123, _mm256_set1_pd(0.50), _CMP_LT_OQ);
    __m256d mask25_4567 = _mm256_cmp_pd(y_4567, _mm256_set1_pd(0.25), _CMP_LT_OQ);
    __m256d mask50_4567 = _mm256_cmp_pd(y_4567, _mm256_set1_pd(0.50), _CMP_LT_OQ);
    
    /* Compute intermediate results */
    __m256d sign_mask_lt50_0123 = _mm256_and_pd(sign_mask, mask50_0123);
    __m256d sign_mask_lt50_4567 = _mm256_and_pd(sign_mask, mask50_4567);
    __m256d x_0123_im_neg       = _mm256_xor_pd(x_0123_im, sign_mask_lt50_0123);
    __m256d x_4567_im_neg       = _mm256_xor_pd(x_4567_im, sign_mask_lt50_4567);
    
    __m256d t1_0123 = _mm256_fmadd_pd(x_0123_im, x_0123_im_neg, _mm256_fmadd_pd(x_0123_re, x_0123_re, y_0123));
    __m256d t1_4567 = _mm256_fmadd_pd(x_4567_im, x_4567_im_neg, _mm256_fmadd_pd(x_4567_re, x_4567_re, y_4567));
    __m256d t2_0123 = _mm256_and_pd(mask50_0123, _mm256_fmadd_pd(2.0*x_0123_re, x_0123_im, y_0123));
    __m256d t2_4567 = _mm256_and_pd(mask50_4567, _mm256_fmadd_pd(2.0*x_4567_re, x_4567_im, y_4567));
    __m256d t_ge25_0123 = _mm256_max_pd(t1_0123, t2_0123);
    __m256d t_ge25_4567 = _mm256_max_pd(t1_4567, t2_4567);
    __m256d t_lt25_0123 = _mm256_min_pd(t1_0123, t2_0123);
    __m256d t_lt25_4567 = _mm256_min_pd(t1_4567, t2_4567);
    
    /* Extract final result */
    y_0123 = _mm256_blendv_pd(t_ge25_0123, t_lt25_0123, mask25_0123);
    y_4567 = _mm256_blendv_pd(t_ge25_4567, t_lt25_4567, mask25_4567);
    _mm256_store_pd(&y[i], y_0123);
    _mm256_store_pd(&y[i+4], y_4567);

  }
}

void maxperformance(complex_t* x, double* y, int n) {
    slow_performance7(x, y, n);
}


/*
* Called by the driver to register your functions
* Use add_function(func, description) to add your own functions
*/
void register_functions() {
  add_function(&slow_performance1, "slow_performance1",1);
  add_function(&slow_performance2, "slow_performance2",1);
  add_function(&slow_performance3, "slow_performance3",1);
  add_function(&slow_performance4, "slow_performance4",1);
  add_function(&slow_performance5, "slow_performance5",1);
  add_function(&slow_performance6, "slow_performance6",1);
  add_function(&slow_performance7, "slow_performance7",1);
  // add_function(&maxperformance, "maxperformance",1);
}
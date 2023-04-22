#include <immintrin.h>
#include "common.h"
#include "quaternion.h"

void slow_performance1(quaternion_t x[N], quaternion_t y[N],quaternion_t A[N][N]) {
    for (int i = 0; i < N; i++) {
      for (int j = 0; j < N; j++) {
            A[i][j] = mul(x[i], y[j]);
        }
    }
}

void slow_performance2(quaternion_t _x[N], quaternion_t _y[N],quaternion_t _A[N][N]) {
    double* x = (double*) &_x[0];
    double* y = (double*) &_y[0];
    double* A = (double*) &_A[0][0];
    
    A[0]  = x[0]*y[0] - x[1]*y[1] - x[2]*y[2] - x[3]*y[3];
    A[1]  = x[0]*y[1] + x[1]*y[0] + x[2]*y[3] - x[3]*y[2];
    A[2]  = x[0]*y[2] - x[1]*y[3] + x[2]*y[0] + x[3]*y[1];
    A[3]  = x[0]*y[3] + x[1]*y[2] - x[2]*y[1] + x[3]*y[0];
    
    A[4]  = x[0]*y[4] - x[1]*y[5] - x[2]*y[6] - x[3]*y[7];
    A[5]  = x[0]*y[5] + x[1]*y[4] + x[2]*y[7] - x[3]*y[6];
    A[6]  = x[0]*y[6] - x[1]*y[7] + x[2]*y[4] + x[3]*y[5];
    A[7]  = x[0]*y[7] + x[1]*y[6] - x[2]*y[5] + x[3]*y[4];
    
    A[8]  = x[4]*y[0] - x[5]*y[1] - x[6]*y[2] - x[7]*y[3];
    A[9]  = x[4]*y[1] + x[5]*y[0] + x[6]*y[3] - x[7]*y[2];
    A[10] = x[4]*y[2] - x[5]*y[3] + x[6]*y[0] + x[7]*y[1];
    A[11] = x[4]*y[3] + x[5]*y[2] - x[6]*y[1] + x[7]*y[0];
    
    A[12] = x[4]*y[4] - x[5]*y[5] - x[6]*y[6] - x[7]*y[7];
    A[13] = x[4]*y[5] + x[5]*y[4] + x[6]*y[7] - x[7]*y[6];
    A[14] = x[4]*y[6] - x[5]*y[7] + x[6]*y[4] + x[7]*y[5];
    A[15] = x[4]*y[7] + x[5]*y[6] - x[6]*y[5] + x[7]*y[4];
}

// Using set_pd
void slow_performance3(quaternion_t _x[N], quaternion_t _y[N], quaternion_t _A[N][N]) {
    double* x = (double*) &_x[0];
    double* y = (double*) &_y[0];
    double* A = (double*) &_A[0][0];

    __m256d x0000 = _mm256_broadcast_sd(x+0);
    __m256d x1111 = _mm256_broadcast_sd(x+1);
    __m256d x2222 = _mm256_broadcast_sd(x+2);
    __m256d x3333 = _mm256_broadcast_sd(x+3);
    __m256d x4444 = _mm256_broadcast_sd(x+4);
    __m256d x5555 = _mm256_broadcast_sd(x+5);
    __m256d x6666 = _mm256_broadcast_sd(x+6);
    __m256d x7777 = _mm256_broadcast_sd(x+7);

    __m256d y0123 = _mm256_set_pd( y[3],  y[2],  y[1],  y[0]);
    __m256d y4567 = _mm256_set_pd( y[7],  y[6],  y[5],  y[4]);
    __m256d y1032 = _mm256_set_pd( y[2], -y[3],  y[0], -y[1]);
    __m256d y2301 = _mm256_set_pd(-y[1],  y[0],  y[3], -y[2]);
    __m256d y3210 = _mm256_set_pd( y[0],  y[1], -y[2], -y[3]);
    __m256d y5476 = _mm256_set_pd( y[6], -y[7],  y[4], -y[5]);
    __m256d y6745 = _mm256_set_pd(-y[5],  y[4],  y[7], -y[6]);
    __m256d y7654 = _mm256_set_pd( y[4],  y[5], -y[6], -y[7]);

    __m256d A0123 = x0000 * y0123 + x1111 * y1032 + x2222 * y2301 + x3333 * y3210;
    __m256d A4567 = x0000 * y4567 + x1111 * y5476 + x2222 * y6745 + x3333 * y7654;
    __m256d A89AB = x4444 * y0123 + x5555 * y1032 + x6666 * y2301 + x7777 * y3210;
    __m256d ACFED = x4444 * y4567 + x5555 * y5476 + x6666 * y6745 + x7777 * y7654;

    _mm256_store_pd(A+0, A0123);
    _mm256_store_pd(A+4, A4567);
    _mm256_store_pd(A+8, A89AB);
    _mm256_store_pd(A+12, ACFED);
}

// Using permute
void slow_performance4(quaternion_t _x[N], quaternion_t _y[N], quaternion_t _A[N][N]) {
    double* x = (double*) &_x[0];
    double* y = (double*) &_y[0];
    double* A = (double*) &_A[0][0];

    __m256d x0000 = _mm256_broadcast_sd(x+0);
    __m256d x1111 = _mm256_broadcast_sd(x+1);
    __m256d x2222 = _mm256_broadcast_sd(x+2);
    __m256d x3333 = _mm256_broadcast_sd(x+3);
    __m256d x4444 = _mm256_broadcast_sd(x+4);
    __m256d x5555 = _mm256_broadcast_sd(x+5);
    __m256d x6666 = _mm256_broadcast_sd(x+6);
    __m256d x7777 = _mm256_broadcast_sd(x+7);

    __m256d y0123 = _mm256_load_pd(y+0);
    __m256d y4567 = _mm256_load_pd(y+4);
    __m256d y1032 = _mm256_permute_pd(y0123, 0b0101);
    __m256d y5476 = _mm256_permute_pd(y4567, 0b0101);
    __m256d y2301 = _mm256_permute4x64_pd(y0123, 0b01001110);
    __m256d y6745 = _mm256_permute4x64_pd(y4567, 0b01001110);
    __m256d y3210 = _mm256_permute_pd(y2301, 0b0101);
    __m256d y7654 = _mm256_permute_pd(y6745, 0b0101);

    y1032 = _mm256_xor_pd(y1032, _mm256_set_pd( 0.0, -0.0,  0.0, -0.0));
    y2301 = _mm256_xor_pd(y2301, _mm256_set_pd(-0.0,  0.0,  0.0, -0.0));
    y3210 = _mm256_xor_pd(y3210, _mm256_set_pd( 0.0,  0.0, -0.0, -0.0));
    y5476 = _mm256_xor_pd(y5476, _mm256_set_pd( 0.0, -0.0,  0.0, -0.0));
    y6745 = _mm256_xor_pd(y6745, _mm256_set_pd(-0.0,  0.0,  0.0, -0.0));
    y7654 = _mm256_xor_pd(y7654, _mm256_set_pd( 0.0,  0.0, -0.0, -0.0));

    __m256d A0123 = x0000 * y0123 + x1111 * y1032 + x2222 * y2301 + x3333 * y3210;
    __m256d A4567 = x0000 * y4567 + x1111 * y5476 + x2222 * y6745 + x3333 * y7654;
    __m256d A89AB = x4444 * y0123 + x5555 * y1032 + x6666 * y2301 + x7777 * y3210;
    __m256d ACFED = x4444 * y4567 + x5555 * y5476 + x6666 * y6745 + x7777 * y7654;

    _mm256_store_pd(A+0, A0123);
    _mm256_store_pd(A+4, A4567);
    _mm256_store_pd(A+8, A89AB);
    _mm256_store_pd(A+12, ACFED);
}

// Using fma
void slow_performance5(quaternion_t _x[N], quaternion_t _y[N], quaternion_t _A[N][N]) {
    double* x = (double*) &_x[0];
    double* y = (double*) &_y[0];
    double* A = (double*) &_A[0][0];

    __m256d x0000 = _mm256_broadcast_sd(x+0);
    __m256d x1111 = _mm256_broadcast_sd(x+1);
    __m256d x2222 = _mm256_broadcast_sd(x+2);
    __m256d x3333 = _mm256_broadcast_sd(x+3);
    __m256d x4444 = _mm256_broadcast_sd(x+4);
    __m256d x5555 = _mm256_broadcast_sd(x+5);
    __m256d x6666 = _mm256_broadcast_sd(x+6);
    __m256d x7777 = _mm256_broadcast_sd(x+7);

    __m256d y0123 = _mm256_load_pd(y+0);
    __m256d y4567 = _mm256_load_pd(y+4);
    __m256d y1032 = _mm256_permute_pd(y0123, 0b0101);
    __m256d y5476 = _mm256_permute_pd(y4567, 0b0101);
    __m256d y2301 = _mm256_permute4x64_pd(y0123, 0b01001110);
    __m256d y6745 = _mm256_permute4x64_pd(y4567, 0b01001110);
    __m256d y3210 = _mm256_permute_pd(y2301, 0b0101);
    __m256d y7654 = _mm256_permute_pd(y6745, 0b0101);

    y2301 = _mm256_xor_pd(y2301, _mm256_set_pd(-0.0,  0.0,  0.0, -0.0));
    y3210 = _mm256_xor_pd(y3210, _mm256_set_pd( 0.0,  0.0, -0.0, -0.0));
    y6745 = _mm256_xor_pd(y6745, _mm256_set_pd(-0.0,  0.0,  0.0, -0.0));
    y7654 = _mm256_xor_pd(y7654, _mm256_set_pd( 0.0,  0.0, -0.0, -0.0)); 
    
    __m256d A0123 = _mm256_fmadd_pd(x0000, y0123, _mm256_fmadd_pd(x3333, y3210, _mm256_fmaddsub_pd(x2222, y2301, _mm256_mul_pd(x1111, y1032))));
    __m256d A4567 = _mm256_fmadd_pd(x0000, y4567, _mm256_fmadd_pd(x3333, y7654, _mm256_fmaddsub_pd(x2222, y6745, _mm256_mul_pd(x1111, y5476))));
    __m256d A89AB = _mm256_fmadd_pd(x4444, y0123, _mm256_fmadd_pd(x7777, y3210, _mm256_fmaddsub_pd(x6666, y2301, _mm256_mul_pd(x5555, y1032))));
    __m256d ACFED = _mm256_fmadd_pd(x4444, y4567, _mm256_fmadd_pd(x7777, y7654, _mm256_fmaddsub_pd(x6666, y6745, _mm256_mul_pd(x5555, y5476))));

    _mm256_store_pd(A+0, A0123);
    _mm256_store_pd(A+4, A4567);
    _mm256_store_pd(A+8, A89AB);
    _mm256_store_pd(A+12, ACFED);
}

void maxperformance(quaternion_t x[N], quaternion_t y[N],quaternion_t A[N][N]) {
  slow_performance5(x,y,A);
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
  add_function(&maxperformance, "maxperformance",1);
}
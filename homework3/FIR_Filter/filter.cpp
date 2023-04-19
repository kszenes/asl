#include "common.h"
#include <immintrin.h>
#include <math.h>

void slow_performance1(double *x, double* h, double* y, int N, int M) {
  for (int i = 0; i < N - (M - 1); i++) {
    y[i] = 0.0;
    for (int k = 0; k < M; k++) {
      y[i] += (i + k + 1) * h[k] * fabs(x[i + (M - 1) - k]);
    }
  }
}

void maxperformance(double *x, double* h, double* y, int N, int M) {
  /* This is the most optimized version. */
}

/*
* Called by the driver to register your functions
* Use add_function(func, description) to add your own functions
*/
void register_functions() 
{
  add_function(&slow_performance1, "slow_performance1",1);
  // add_function(&maxperformance, "maxperformance",1);
}
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

void maxperformance(quaternion_t x[N], quaternion_t y[N],quaternion_t A[N][N]) {
  /* Add your final implementation here */
}

/*
* Called by the driver to register your functions
* Use add_function(func, description) to add your own functions
*/
void register_functions() {
  add_function(&slow_performance1, "slow_performance1",1);
  add_function(&maxperformance, "maxperformance",1);
}
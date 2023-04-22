#include "include/microbenchmark.h"
#include "include/tsc_x86.h"
#include "include/foo.h"
#include <cmath>
#include <iostream>

/* Global variables so compiler does not optimize the benchmarks */
double inp_xmm[16];
double out_xmm[16];

double x1 , x2 , x3 , x4 ;
double x5 , x6 , x7 , x8 ;
double x9 , x10, x11, x12;
double x13, x14, x15, x16;

typedef double (*funPtr_t)();

const int iter = 5000;
const int warmup_iter = 3000;

static double bench(funPtr_t f) {
    
    /* First warm up the cpu */
    for (int i = 0; i < warmup_iter; ++i) {
      f();
    }
  
    /* Start bench */
    double start = start_tsc();
    f();
    double cycles = stop_tsc(start);
    
    return cycles;
}

void initialize_microbenchmark_data (microbenchmark_mode_t mode) {
    switch (mode) {
        case MAX_LAT:
        case MAX_GAP:
            for (int i = 0; i < 10; ++i) {
                inp_xmm[i]  = 2.1234;
            }
            break;
        case DIV_LAT:
        case DIV_GAP:
            for (int i = 0; i < 10; ++i) {
                inp_xmm[i]  = 2.1234;
            }
            inp_xmm[1] = 3.1;
            break;
        case DIV_LAT_MIN:
        case DIV_GAP_MIN:
            for (int i = 0; i < 10; ++i) {
                inp_xmm[i] = 1.0;
            }
            break;
        case FOO_LAT:
        case FOO_GAP:
            for (int i = 0; i < 8; ++i) {
                inp_xmm[i]    = 2.1234;
                inp_xmm[i+8]  = 2.1234 - foo(2.1234);
            }
            break;
        case FOO_LAT_MIN:
        case FOO_GAP_MIN:
            double cst;
            for (int i = 0; i < 8; ++i) {
                cst = sqrt(17.0);
                inp_xmm[i]    = cst;
                inp_xmm[i+8]  = cst - foo(cst);
            }
            break;
        default: break;
    }
    
    x1  = inp_xmm[0] , x2  = inp_xmm[1] , x3  = inp_xmm[2] , x4  = inp_xmm[3];
    x5  = inp_xmm[4] , x6  = inp_xmm[5] , x7  = inp_xmm[6] , x8  = inp_xmm[7];
    x9  = inp_xmm[8] , x10 = inp_xmm[9] , x11 = inp_xmm[10], x12 = inp_xmm[11];
    x13 = inp_xmm[12], x14 = inp_xmm[13], x15 = inp_xmm[14], x16 = inp_xmm[15];
}

static inline double max(double x1, double x2) {
    return x1 > x2 ? x1 : x2;
}

double max_latency() {
  
  for (int i = 0; i < iter; i++) {
    x1 = max(x1, x2);
    x1 = max(x1, x3);
    x1 = max(x1, x4);
    x1 = max(x1, x5);
    x1 = max(x1, x6);
    x1 = max(x1, x7);
    x1 = max(x1, x8);
    x1 = max(x1, x9);
  }
  
  out_xmm[0] = x1;
  return x1;
}

double max_gap() {
    // Latency is 4 and gap 0.5. Thus, we need at least 8 to fill the pipeline.
    // We do 12 to guarantee that is filled. We unroll once to get more stable measurements.
    for (int i = 0; i < iter-1; i+=2) {
      x2  = max(x1, x2);
      x3  = max(x1, x3);
      x4  = max(x1, x4);
      x5  = max(x1, x5);
      x6  = max(x1, x6);
      x7  = max(x1, x7);
      x8  = max(x1, x8);
      x9  = max(x1, x9);
      x10 = max(x1, x10);
      x11 = max(x1, x11);
      x12 = max(x1, x12);
      x13 = max(x1, x13);
      
      x2  = max(x1, x2);
      x3  = max(x1, x3);
      x4  = max(x1, x4);
      x5  = max(x1, x5);
      x6  = max(x1, x6);
      x7  = max(x1, x7);
      x8  = max(x1, x8);
      x9  = max(x1, x9);
      x10 = max(x1, x10);
      x11 = max(x1, x11);
      x12 = max(x1, x12);
      x13 = max(x1, x13);
    }

    out_xmm[0]  = x1;  out_xmm[1]  = x2;
    out_xmm[2]  = x3;  out_xmm[3]  = x4;
    out_xmm[4]  = x5;  out_xmm[5]  = x6;
    out_xmm[6]  = x7;  out_xmm[7]  = x8;
    out_xmm[8]  = x9;  out_xmm[9]  = x10;
    out_xmm[10] = x11; out_xmm[11] = x12;
    out_xmm[12] = x13; out_xmm[13] = x14;
    out_xmm[14] = x15; out_xmm[15] = x16;

    return x2;
}

double div_latency() {
  for (int i = 0; i < iter; i++) {
    x1 = x2 / x1;
    x1 = x2 / x1;
    x1 = x2 / x1;
    x1 = x2 / x1;
    x1 = x2 / x1;
    x1 = x2 / x1;
  }
  
  out_xmm[0] = x1;
  return x1;
}

double div_gap() {
  for (int i = 0; i < iter; i++) {
    x3 = x2 / x3;
    x4 = x2 / x4;
    x5 = x2 / x5;
    x6 = x2 / x6;
    x7 = x2 / x7;
    x8 = x2 / x8;
  }    
  
  out_xmm[0]  = x1;  out_xmm[1]  = x2;
  out_xmm[2]  = x3;  out_xmm[3]  = x4;
  out_xmm[4]  = x5;  out_xmm[5]  = x6;
  out_xmm[6]  = x7;  out_xmm[7]  = x8;
  out_xmm[8]  = x9;  out_xmm[9]  = x10;
  out_xmm[10] = x11; out_xmm[11] = x12;
  out_xmm[12] = x13; out_xmm[13] = x14;
  out_xmm[14] = x15; out_xmm[15] = x16;

  return x1;
}

double foo_latency() {
  
  for (int i = 0; i < iter; i++) {
    x1 = foo(x1) + x9; // Add x9 to restore to original value
    x1 = foo(x1) + x9;
    x1 = foo(x1) + x9;
    x1 = foo(x1) + x9;
    x1 = foo(x1) + x9;
    x1 = foo(x1) + x9;
  }
  out_xmm[0] = x1;
  return x1;
}

double foo_gap() {
  
  for (int i = 0; i < iter; i++) {
    x1 = foo(x1) + x9; // Add x9 to restore to original value
    x2 = foo(x2) + x9;
    x3 = foo(x3) + x9;
    x4 = foo(x4) + x9;
    x5 = foo(x5) + x9;
    x6 = foo(x6) + x9;
    x7 = foo(x7) + x9;
    x8 = foo(x8) + x9;
  }    
  out_xmm[0]  = x1;  out_xmm[1]  = x2;
  out_xmm[2]  = x3;  out_xmm[3]  = x4;
  out_xmm[4]  = x5;  out_xmm[5]  = x6;
  out_xmm[6]  = x7;  out_xmm[7]  = x8;

    return x1;
}

double microbenchmark_get_max_latency() {
    double cycles = bench( max_latency );
    return cycles / 8.0 / iter;
}

double microbenchmark_get_max_gap() {
    double cycles = bench( max_gap );
    return cycles / 12.0 / iter;
}

double microbenchmark_get_div_latency() {
    double cycles = bench( div_latency );
    return cycles / 6.0 / iter; 
}

double microbenchmark_get_div_gap() {
    double cycles = bench( div_gap );
    return cycles / 6.0 / iter;
}

double microbenchmark_get_foo_latency() {
    double cycles = bench( foo_latency );
    return cycles / 6.0 / iter - 4.0; // Subtract overhead of addition
}

double microbenchmark_get_foo_gap() {
    double cycles = bench( foo_gap );
    return cycles / 8.0 / iter;
}
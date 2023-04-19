#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "tsc_x86.h"

#define NUM_RUNS 30
#define CALIBRATE
#define CYCLES_REQUIRED 1e8
#define FREQUENCY 2.6e9

void comp(double *x, double *y, int n) {
    double s = 0.0;
    for (int i = 0; i < n; i++) {
        s = (s + x[i]*x[i]) + y[i]*y[i]*y[i];
    }
    x[0] = s;
}

void comp2(double *x, double *y, int n) {
    double s = 0.0, s2 = 0.0, s3 = 0.0, s4 = 0.0, s5 = 0.0, s6 = 0.0, s7 = 0.0, s8 = 0.0 , s9 = 0.0;
    int i = 0;
    for (i = 0; i < n - 8; i += 8) {
        s = (s + x[i]*x[i]) + y[i]*y[i]*y[i];
        s2 = (s2 + x[i+1]*x[i+1]) + y[i+1]*y[i+1]*y[i+1];
        s3 = (s3 + x[i+2]*x[i+2]) + y[i+2]*y[i+2]*y[i+2];
        s4 = (s4 + x[i+3]*x[i+3]) + y[i+3]*y[i+3]*y[i+3];
        s5 = (s5 + x[i+4]*x[i+4]) + y[i+4]*y[i+4]*y[i+4];
        s6 = (s6 + x[i+5]*x[i+5]) + y[i+5]*y[i+5]*y[i+5];
        s7 = (s7 + x[i+6]*x[i+6]) + y[i+6]*y[i+6]*y[i+6];
        s8 = (s8 + x[i+7]*x[i+7]) + y[i+7]*y[i+7]*y[i+7];
        s9 = (s9 + x[i+8]*x[i+8]) + y[i+8]*y[i+8]*y[i+8];
    }
    for (; i < n; ++i) {
        s = (s + x[i]*x[i]) + y[i]*y[i]*y[i];
    }
    x[0] = s + s2 + s3 + s4 + s5 + s6 + s7 + s8 + s9;
}

/*
 * Timing function based on the TimeStep Counter of the CPU.
 */
#ifdef __x86_64__
double rdtsc(double A[], double B[], int n) {
    int i, num_runs;
    myInt64 cycles;
    myInt64 start;
    num_runs = NUM_RUNS;

    /*
     * The CPUID instruction serializes the pipeline.
     * Using it, we can create execution barriers around the code we want to time.
     * The calibrate section is used to make the computation large enough so as to
     * avoid measurements bias due to the timing overhead.
     */
#ifdef CALIBRATE
    while(num_runs < (1 << 14)) {
        start = start_tsc();
        for (i = 0; i < num_runs; ++i) {
            comp2(A, B, n);
        }
        cycles = stop_tsc(start);

        if(cycles >= CYCLES_REQUIRED) break;

        num_runs *= 2;
    }
#endif

    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
        comp2(A, B, n);
    }

    cycles = stop_tsc(start)/num_runs;
    return (double) cycles;
}
#endif

double gen_rand() {
    return (((double) rand()) / RAND_MAX) - 0.5;
}


int main() {
    for (int i = 4; i <= 24; ++i) {
        double n = pow(2.0, (double) i);
        double flops = 5 * n; 
        double* x = (double*) malloc(n * sizeof(double));
        double* y = (double*) malloc(n * sizeof(double));

        for (int j = 0; j < n; ++j) {
            x[j] = gen_rand();
            y[j] = gen_rand();
        }

        double r = rdtsc(x, y, n);
        printf("%d: %f [flops/cycle]\n", i, flops / r);

        free(y);
        free(x);
        
    }

}

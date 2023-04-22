#include "common.h"
#include "mat.h"

#define C1 0.1
#define C2 (2.0/3.0)

using namespace std;

// 1241K cycles. 1x (0%)
void slow_performance1(mat* x, mat* y, mat*z) {
    double t1;
    for (int i = 0; i < z->n1; i++) {
        for (int j = 1; j < z->n2 - 1; j++) {
            if (i % 2) {
                t1 = mat_get(z,i,j)/sqrt(mat_get(y,0,i%2)) + (mat_get(x,i,j) + C1)*(mat_get(x,i,j) - C1);
            } else {
                t1 = mat_get(z,i,j)/sqrt(mat_get(y,0,i%2)) + C1*mat_get(x,i,j);
            }
            mat_set(z,i,j-1, mat_get(z,i,j-1)*cos(C2*M_PI*j));
            mat_set(z,i,j,t1);
            mat_set(z,i,j+1,fmax(mat_get(z,i,j+1), mat_get(x,i,j+1)));
        }
    }
}

// Without mat data structure
// 490K cycles. 2.57x (0%)
void slow_performance2(mat* _x, mat* _y, mat* _z) {
    double* x = _x->data;
    double* y = _y->data;
    double* z = _z->data;
    int n = _z->n2;

    double t1;
    for ( int i = 0; i < _z->n1; i++) {
        for (int j = 1; j < _z->n2 - 1; j++) {
            if (i % 2) {
                t1 = z[i*n + j]/sqrt(y[i%2]) + (x[i*n + j] + C1)*(x[i*n + j] - C1);
            } else {
                t1 = z[i*n + j]/sqrt(y[i%2]) + C1*x[i*n + j];
            }
            z[i*n + j-1] = z[i*n + j-1]*cos(C2*M_PI*j);
            z[i*n + j] = t1;
            z[i*n + j+1] = fmax(z[i*n + j + 1], x[i*n + j + 1]);
        }
    }
}

// Remove the if condition by unrolling outer loop
// 289K cycles. 4.37x (19%)
void slow_performance3(mat* _x, mat* _y, mat* _z) {
    double* x = _x->data;
    double* y = _y->data;
    double* z = _z->data;
    int n = _z->n2;

    for (int i = 0; i < _z->n1 - 1; i += 2) {
        for (int j = 1; j < _z->n2 - 1; j++) {
            z[(i+0)*n + j-1] = z[(i+0)*n + j-1]*cos(C2*M_PI*(j+0));
            z[(i+0)*n + j+0] = z[(i+0)*n + j+0]/sqrt(y[0]) + C1*x[(i+0)*n + j+0];
            z[(i+0)*n + j+1] = fmax(z[(i+0)*n + j+1], x[(i+0)*n + j+1]);

            z[(i+1)*n + j-1] = z[(i+1)*n + j-1]*cos(C2*M_PI*(j+0));
            z[(i+1)*n + j+0] = z[(i+1)*n + j+0]/sqrt(y[1]) + (x[(i+1)*n + j+0] + C1)*(x[(i+1)*n + j+0] - C1);
            z[(i+1)*n + j+1] = fmax(z[(i+1)*n + j+1], x[(i+1)*n + j+1]);
        }
    }
}


// Unrolling inner loop three times and remove cos and sqrt from loop
// 78.7K cycles. 16x (50%)
void slow_performance4(mat* _x, mat* _y, mat* _z) {
    double* x = _x->data;
    double* y = _y->data;
    double* z = _z->data;
    int n = _z->n2;

    double cos_1 = cos(C2*M_PI*(1.0));
    double cos_2 = cos(C2*M_PI*(2.0));
    double cos_3 = cos(C2*M_PI*(3.0));
    double inv_sqtr_y0 = 1.0/sqrt(y[0]);
    double inv_sqtr_y1 = 1.0/sqrt(y[1]);

    for (int i = 0; i < _z->n1 - 1; i += 2) {
        for (int j = 1; j < _z->n2 - 3; j+=3) {
            z[(i+0)*n + j-1] = z[(i+0)*n + j-1]*cos_1;
            z[(i+0)*n + j+0] = z[(i+0)*n + j+0]*inv_sqtr_y0 + C1*x[(i+0)*n + j+0];
            z[(i+0)*n + j+1] = fmax(z[(i+0)*n + j+1], x[(i+0)*n + j+1]);

            z[(i+1)*n + j-1] = z[(i+1)*n + j-1]*cos_1;
            z[(i+1)*n + j+0] = z[(i+1)*n + j+0]*inv_sqtr_y1 + (x[(i+1)*n + j+0] + C1)*(x[(i+1)*n + j+0] - C1);
            z[(i+1)*n + j+1] = fmax(z[(i+1)*n + j+1], x[(i+1)*n + j+1]);

            ///
            z[(i+0)*n + j+0] = z[(i+0)*n + j+0]*cos_2;
            z[(i+0)*n + j+1] = z[(i+0)*n + j+1]*inv_sqtr_y0 + C1*x[(i+0)*n + j+1];
            z[(i+0)*n + j+2] = fmax(z[(i+0)*n + j+2], x[(i+0)*n + j+2]);

            z[(i+1)*n + j+0] = z[(i+1)*n + j+0]*cos_2;
            z[(i+1)*n + j+1] = z[(i+1)*n + j+1]*inv_sqtr_y1 + (x[(i+1)*n + j+1] + C1)*(x[(i+1)*n + j+1] - C1);
            z[(i+1)*n + j+2] = fmax(z[(i+1)*n + j+2], x[(i+1)*n + j+2]);

            ///
            z[(i+0)*n + j+1] = z[(i+0)*n + j+1]*cos_3;
            z[(i+0)*n + j+2] = z[(i+0)*n + j+2]*inv_sqtr_y0 + C1*x[(i+0)*n + j+2];
            z[(i+0)*n + j+3] = fmax(z[(i+0)*n + j+3], x[(i+0)*n + j+3]);

            z[(i+1)*n + j+1] = z[(i+1)*n + j+1]*cos_3;
            z[(i+1)*n + j+2] = z[(i+1)*n + j+2]*inv_sqtr_y1 + (x[(i+1)*n + j+2] + C1)*(x[(i+1)*n + j+2] - C1);
            z[(i+1)*n + j+3] = fmax(z[(i+1)*n + j+3], x[(i+1)*n + j+3]);
        }
    }
}

// Replacing fmax function to avoid function call to math library
// 40.9K cycles. 31x (80%)
void slow_performance5(mat* _x, mat* _y, mat* _z) {
    double* x = _x->data;
    double* y = _y->data;
    double* z = _z->data;
    int n = _z->n2;

    double cos_1 = cos(C2*M_PI*(1.0));
    double cos_2 = cos(C2*M_PI*(2.0));
    double cos_3 = cos(C2*M_PI*(3.0));
    double inv_sqtr_y0 = 1.0/sqrt(y[0]);
    double inv_sqtr_y1 = 1.0/sqrt(y[1]);

    for (int i = 0; i < _z->n1 - 1; i += 2) {
        for (int j = 1; j < _z->n2 - 3; j+=3) {
            z[(i+0)*n + j-1] = z[(i+0)*n + j-1]*cos_1;
            z[(i+0)*n + j+0] = z[(i+0)*n + j+0]*inv_sqtr_y0 + C1*x[(i+0)*n + j+0];
            z[(i+0)*n + j+1] = max(z[(i+0)*n + j+1], x[(i+0)*n + j+1]);

            z[(i+1)*n + j-1] = z[(i+1)*n + j-1]*cos_1;
            z[(i+1)*n + j+0] = z[(i+1)*n + j+0]*inv_sqtr_y1 + (x[(i+1)*n + j+0] + C1)*(x[(i+1)*n + j+0] - C1);
            z[(i+1)*n + j+1] = max(z[(i+1)*n + j+1], x[(i+1)*n + j+1]);

            ///
            z[(i+0)*n + j+0] = z[(i+0)*n + j+0]*cos_2;
            z[(i+0)*n + j+1] = z[(i+0)*n + j+1]*inv_sqtr_y0 + C1*x[(i+0)*n + j+1];
            z[(i+0)*n + j+2] = max(z[(i+0)*n + j+2], x[(i+0)*n + j+2]);

            z[(i+1)*n + j+0] = z[(i+1)*n + j+0]*cos_2;
            z[(i+1)*n + j+1] = z[(i+1)*n + j+1]*inv_sqtr_y1 + (x[(i+1)*n + j+1] + C1)*(x[(i+1)*n + j+1] - C1);
            z[(i+1)*n + j+2] = max(z[(i+1)*n + j+2], x[(i+1)*n + j+2]);

            ///
            z[(i+0)*n + j+1] = z[(i+0)*n + j+1]*cos_3;
            z[(i+0)*n + j+2] = z[(i+0)*n + j+2]*inv_sqtr_y0 + C1*x[(i+0)*n + j+2];
            z[(i+0)*n + j+3] = max(z[(i+0)*n + j+3], x[(i+0)*n + j+3]);

            z[(i+1)*n + j+1] = z[(i+1)*n + j+1]*cos_3;
            z[(i+1)*n + j+2] = z[(i+1)*n + j+2]*inv_sqtr_y1 + (x[(i+1)*n + j+2] + C1)*(x[(i+1)*n + j+2] - C1);
            z[(i+1)*n + j+3] = max(z[(i+1)*n + j+3], x[(i+1)*n + j+3]);
        }
    }
}

// Simplifications and remove extra memory accesses
// 24.5K cycles. 51x (100%) ~1.95 flops/cycle
void slow_performance6(mat* _x, mat* _y, mat* _z) {
    double* x = _x->data;
    double* y = _y->data;
    double* z = _z->data;
    int n = _z->n2;

    double cos_1 = -0.5;
    double cos_2 = -0.5;
    double cos_3 =  1.0;
    double inv_sqtr_y0 = 1.0/sqrt(y[0]);
    double inv_sqtr_y1 = 1.0/sqrt(y[1]);
    double c1_sqr = C1*C1;
    double a1 = inv_sqtr_y0*cos_2;
    double a2 = C1*cos_2;
    double a3 = inv_sqtr_y0*cos_3;
    double a4 = C1*cos_3;

    double b1 = inv_sqtr_y1*cos_2;
    double b2 = c1_sqr*cos_2;
    double b3 = inv_sqtr_y1*cos_3;
    double b4 = c1_sqr*cos_3;
    
    double d1 = inv_sqtr_y0*cos_1;
    double d2 = C1*cos_1;
    double d3 = inv_sqtr_y1*cos_1;
    double d4 = c1_sqr*cos_1;

    for (int i = 0; i < _z->n1 - 1; i += 2) { // 50 iterations
        // Prelude: 10 flops
        z[(i+0)*n + 1-1] = z[(i+0)*n + 1-1]*cos_1;
        z[(i+1)*n + 1-1] = z[(i+1)*n + 1-1]*cos_1;
        z[(i+0)*n + 1+0] = z[(i+0)*n + 1+0]*a1 + x[(i+0)*n + 1+0]*a2;
        z[(i+1)*n + 1+0] = z[(i+1)*n + 1+0]*b1 + x[(i+1)*n + 1+0]*x[(i+1)*n + 1+0]*cos_2 - b2;
        
        int j = 1;
        for (; j < _z->n2 - 6; j+=3) { // 32 iterations
            // Body: 29 flops
            z[(i+0)*n + j+1] = max(z[(i+0)*n + j+1], x[(i+0)*n + j+1])*a3 + x[(i+0)*n + j+1]*a4;
            z[(i+0)*n + j+2] = max(z[(i+0)*n + j+2], x[(i+0)*n + j+2])*d1 + x[(i+0)*n + j+2]*d2;
            z[(i+0)*n + j+3] = max(z[(i+0)*n + j+3], x[(i+0)*n + j+3])*a1 + x[(i+0)*n + j+3]*a2;
            z[(i+1)*n + j+1] = max(z[(i+1)*n + j+1], x[(i+1)*n + j+1])*b3 + x[(i+1)*n + j+1]*x[(i+1)*n + j+1]       - b4;
            z[(i+1)*n + j+2] = max(z[(i+1)*n + j+2], x[(i+1)*n + j+2])*d3 + x[(i+1)*n + j+2]*x[(i+1)*n + j+2]*cos_1 - d4;
            z[(i+1)*n + j+3] = max(z[(i+1)*n + j+3], x[(i+1)*n + j+3])*b1 + x[(i+1)*n + j+3]*x[(i+1)*n + j+3]*cos_2 - b2;
        }
        
        // Postamble: 20 flops
        z[(i+0)*n + j+1] = max(z[(i+0)*n + j+1], x[(i+0)*n + j+1])*a3          + x[(i+0)*n + j+1]*a4;
        z[(i+0)*n + j+2] = max(z[(i+0)*n + j+2], x[(i+0)*n + j+2])*inv_sqtr_y0 + x[(i+0)*n + j+2]*C1;
        z[(i+0)*n + j+3] = max(z[(i+0)*n + j+3], x[(i+0)*n + j+3]);
        z[(i+1)*n + j+1] = max(z[(i+1)*n + j+1], x[(i+1)*n + j+1])*b3          + x[(i+1)*n + j+1]*x[(i+1)*n + j+1] - b4;
        z[(i+1)*n + j+2] = max(z[(i+1)*n + j+2], x[(i+1)*n + j+2])*inv_sqtr_y1 + x[(i+1)*n + j+2]*x[(i+1)*n + j+2] - c1_sqr;
        z[(i+1)*n + j+3] = max(z[(i+1)*n + j+3], x[(i+1)*n + j+3]);
    }
}

void maxperformance(mat* x, mat* y, mat* z) {
  slow_performance6(x,y,z);
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
  add_function(&maxperformance, "maxperformance",1);
}
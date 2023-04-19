#include "common.h"
#include "mat.h"

#define C1 0.1
#define C2 (2.0/3.0)

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

void scalar_replacement(mat* x, mat* y, mat* z) {
    double t1 = 0.0;
    for (int i = 0; i < z->n1; i++) {
        double mat_y = mat_get(y, 0, i%2);
        double invsqrt_y = 1.0 / sqrt(mat_y);
        for (int j = 1; j < z->n2 - 1; j++) {
            double mat_x = mat_get(x, i, j);
            double mat_z = mat_get(z, i, j);
            if (i % 2) {
                t1 = mat_z * invsqrt_y + (mat_x + C1)*(mat_x - C1);
            } else {
                t1 = mat_z * invsqrt_y + C1*mat_x;
            }
            mat_set(z,i,j-1, mat_get(z,i,j-1)*cos(C2*M_PI*j));
            mat_set(z,i,j,t1);
            mat_set(z,i,j+1,fmax(mat_get(z,i,j+1), mat_get(x,i,j+1)));
        }
    }
}
void inlining(mat* x, mat* y, mat* z) {
    double t1 = 0.0;
    for (int i = 0; i < z->n1; i++) {
        double mat_y = y->data[i%2];
        double invsqrt_y = 1.0 / sqrt(mat_y);
        for (int j = 1; j < z->n2 - 1; j++) {
            double mat_x = x->data[i*x->n2 + j];
            double mat_z = z->data[i*z->n2 + j];
            if (i % 2) {
                t1 = mat_z * invsqrt_y + (mat_x + C1)*(mat_x - C1);
            } else {
                t1 = mat_z * invsqrt_y + C1*mat_x;
            }
            z->data[i*z->n2 + j-1] = z->data[i*z->n2 + j-1] * cos(C2*M_PI*j);
            z->data[i*z->n2 + j] = t1;
            z->data[i*z->n2 + j+1] = fmax(z->data[i*z->n2 + j+1],
                                          x->data[i*x->n2 + j+1]);
        }
    }
}

void branchless(mat* x, mat* y, mat* z) {
    double t1 = 0.0;
    for (int i = 0; i < z->n1; i++) {
        int odd = i % 2;
        int even = !odd;
        double mat_y = y->data[odd];
        double invsqrt_y = 1.0 / sqrt(mat_y);
        for (int j = 1; j < z->n2 - 1; j++) {
            double mat_x = x->data[i*x->n2 + j];
            double mat_z = z->data[i*z->n2 + j];
            t1 = mat_z * invsqrt_y + odd*(mat_x + C1)*(mat_x - C1) + even*(C1 * mat_x);
            z->data[i*z->n2 + j-1] = z->data[i*z->n2 + j-1] * cos(C2*M_PI*j);
            z->data[i*z->n2 + j] = t1;
            z->data[i*z->n2 + j+1] = fmax(z->data[i*z->n2 + j+1],
                                          x->data[i*x->n2 + j+1]);
        }
    }
}


void maxperformance(mat* x, mat* y, mat* z) {
  /* Add your final implementation here */
}

/*
* Called by the driver to register your functions
* Use add_function(func, description) to add your own functions
*/
void register_functions() {
  add_function(&slow_performance1, "slow_performance1",1);
  add_function(&scalar_replacement, "scalar_replacement",1);
  add_function(&inlining, "inlining",1);
  add_function(&branchless, "branchless",1);
  /* add_function(&maxperformance, "maxperformance",1); */
}

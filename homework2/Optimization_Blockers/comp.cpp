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
    double t0;
    double t1;
    int i;
    double invsqrt_y0 = 1.0 / sqrt(y->data[0]);
    double invsqrt_y1 = 1.0 / sqrt(y->data[1]);
    for (i = 0; i < z->n1-1; i += 2) {
        for (int j = 1; j < z->n2 - 1; j++) {
            double mat_x0 = x->data[i*x->n2 + j];
            double mat_z0 = z->data[i*z->n2 + j];
            double mat_x1 = x->data[(i+1)*x->n2 + j];
            double mat_z1 = z->data[(i+1)*z->n2 + j];

            t0 = mat_z0 * invsqrt_y0 + C1*mat_x0;
            t1 = mat_z1 * invsqrt_y1 + (mat_x1 + C1)*(mat_x1 - C1);

            z->data[i*z->n2 + j-1] = z->data[i*z->n2 + j-1] * cos(C2*M_PI*j);
            z->data[i*z->n2 + j] = t0;
            z->data[i*z->n2 + j+1] = fmax(z->data[i*z->n2 + j+1], x->data[i*x->n2 + j+1]);

            z->data[(i+1)*z->n2 + j-1] = z->data[(i+1)*z->n2 + j-1] * cos(C2*M_PI*j);
            z->data[(i+1)*z->n2 + j] = t1;
            z->data[(i+1)*z->n2 + j+1] = fmax(z->data[(i+1)*z->n2 + j+1], x->data[(i+1)*x->n2 + j+1]);
        }
    }
}

static inline double max(double a, double b) {
    return a > b ? a : b;
}

void max_custom(mat* x, mat* y, mat* z) {
    double t0;
    double t1;
    int i;
    double invsqrt_y0 = 1.0 / sqrt(y->data[0]);
    double invsqrt_y1 = 1.0 / sqrt(y->data[1]);
    for (i = 0; i < z->n1-1; i += 2) {
        for (int j = 1; j < z->n2 - 1; j++) {
            double mat_x0 = x->data[i*x->n2 + j];
            double mat_z0 = z->data[i*z->n2 + j];
            double mat_x1 = x->data[(i+1)*x->n2 + j];
            double mat_z1 = z->data[(i+1)*z->n2 + j];

            t0 = mat_z0 * invsqrt_y0 + C1*mat_x0;
            t1 = mat_z1 * invsqrt_y1 + (mat_x1 + C1)*(mat_x1 - C1);

            z->data[i*z->n2 + j-1] = z->data[i*z->n2 + j-1] * cos(C2*M_PI*j);
            z->data[i*z->n2 + j] = t0;
            z->data[i*z->n2 + j+1] = max(z->data[i*z->n2 + j+1], x->data[i*x->n2 + j+1]);

            z->data[(i+1)*z->n2 + j-1] = z->data[(i+1)*z->n2 + j-1] * cos(C2*M_PI*j);
            z->data[(i+1)*z->n2 + j] = t1;
            z->data[(i+1)*z->n2 + j+1] = max(z->data[(i+1)*z->n2 + j+1], x->data[(i+1)*x->n2 + j+1]);
        }
    }
}

void cosless(mat* x, mat* y, mat* z) {
    int i;
    double invsqrt_y0 = 1.0 / sqrt(y->data[0]);
    double invsqrt_y1 = 1.0 / sqrt(y->data[1]);
    double cos1 = cos(C2*M_PI*1.0);
    double cos2 = cos(C2*M_PI*2.0);
    double cos3 = cos(C2*M_PI*3.0);
    for (i = 0; i < z->n1-1; i += 2) {
        for (int j = 1; j < z->n2 - 3; j += 3) {
            double mat_x0_0 = x->data[i*x->n2 + j];
            double mat_z0_0 = z->data[i*z->n2 + j];
            double mat_x1_0 = x->data[(i+1)*x->n2 + j];
            double mat_z1_0 = z->data[(i+1)*z->n2 + j];
            double mat_x0_1 = x->data[i*x->n2 + j+1];
            double mat_z0_1 = z->data[i*z->n2 + j+1];
            double mat_x1_1 = x->data[(i+1)*x->n2 + j+1];
            double mat_z1_1 = z->data[(i+1)*z->n2 + j+1];
            double mat_x0_2 = x->data[i*x->n2 + j+2];
            double mat_z0_2 = z->data[i*z->n2 + j+2];
            double mat_x1_2 = x->data[(i+1)*x->n2 + j+2];
            double mat_z1_2 = z->data[(i+1)*z->n2 + j+2];

            z->data[i*z->n2 + j-1] = z->data[i*z->n2 + j-1] * cos1;
            z->data[i*z->n2 + j] = mat_z0_0 * invsqrt_y0 + C1*mat_x0_0;
            z->data[i*z->n2 + j+1] = fmax(z->data[i*z->n2 + j+1], x->data[i*x->n2 + j+1]);

            z->data[(i+1)*z->n2 + j-1] = z->data[(i+1)*z->n2 + j-1] * cos1;
            z->data[(i+1)*z->n2 + j] = mat_z1_0 * invsqrt_y1 + (mat_x1_0 + C1)*(mat_x1_0 - C1);
            z->data[(i+1)*z->n2 + j+1] = fmax(z->data[(i+1)*z->n2 + j+1], x->data[(i+1)*x->n2 + j+1]);


            z->data[i*z->n2 + j] = z->data[i*z->n2 + j] * cos2;
            z->data[i*z->n2 + j+1] = mat_z0_1 * invsqrt_y0 + C1*mat_x0_1;
            z->data[i*z->n2 + j+2] = fmax(z->data[i*z->n2 + j+2], x->data[i*x->n2 + j+2]);

            z->data[(i+1)*z->n2 + j] = z->data[(i+1)*z->n2 + j] * cos2;
            z->data[(i+1)*z->n2 + j+1] = mat_z1_1 * invsqrt_y1 + (mat_x1_1 + C1)*(mat_x1_1 - C1);
            z->data[(i+1)*z->n2 + j+2] = fmax(z->data[(i+1)*z->n2 + j+2], x->data[(i+1)*x->n2 + j+2]);


            z->data[i*z->n2 + j+1] = z->data[i*z->n2 + j+1] * cos3;
            z->data[i*z->n2 + j+2] = mat_z0_2 * invsqrt_y0 + C1*mat_x0_2;
            z->data[i*z->n2 + j+3] = fmax(z->data[i*z->n2 + j+3], x->data[i*x->n2 + j+3]);

            z->data[(i+1)*z->n2 + j+1] = z->data[(i+1)*z->n2 + j+1] * cos3;
            z->data[(i+1)*z->n2 + j+2] = mat_z1_2 * invsqrt_y1 + (mat_x1_2 + C1)*(mat_x1_2 - C1);
            z->data[(i+1)*z->n2 + j+3] = fmax(z->data[(i+1)*z->n2 + j+3], x->data[(i+1)*x->n2 + j+3]);
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
  add_function(&max_custom, "max_custom",1);
//  add_function(&cosless, "cosless",1);
  /* add_function(&maxperformance, "maxperformance",1); */
}

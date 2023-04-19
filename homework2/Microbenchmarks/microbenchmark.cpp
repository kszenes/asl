#include "include/microbenchmark.h"
#include "include/tsc_x86.h"
#include "include/foo.h"


void initialize_microbenchmark_data (microbenchmark_mode_t mode) {
  /* You can use to initialize some data if needed */
    switch (mode) {
        case MAX_LAT:
        case MAX_GAP:
        case DIV_LAT:
        case DIV_GAP:
        case DIV_LAT_MIN:
        case DIV_GAP_MIN:
        case FOO_LAT:
        case FOO_GAP:
        case FOO_LAT_MIN:
        case FOO_GAP_MIN:
        default: break;
    }
}


double microbenchmark_get_max_latency() {
    /* Implement your microbenchmark benchmark here */
    return 0;
}

double microbenchmark_get_max_gap() {
    /* Implement your microbenchmark benchmark here */
    return 0;
}

double microbenchmark_get_div_latency() {
    /* Implement your microbenchmark benchmark here */
    return 0;
}

double microbenchmark_get_div_gap() {
    /* Implement your microbenchmark benchmark here */
    return 0;
}

double microbenchmark_get_foo_latency() {
    /* Implement your microbenchmark benchmark here */
    return 0;
}

double microbenchmark_get_foo_gap() {
    /* Implement your microbenchmark benchmark here */
    return 0;
}
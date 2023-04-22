#ifndef MICROBENCHMARK_H
#define MICROBENCHMARK_H

typedef enum {
    MAX_LAT,
    MAX_GAP,
    DIV_LAT,
    DIV_GAP,
    DIV_LAT_MIN,
    DIV_GAP_MIN,
    FOO_LAT,
    FOO_GAP,
    FOO_LAT_MIN,
    FOO_GAP_MIN,
    
    /* Limits */
    START_TEST = MAX_LAT,
    END_TEST = FOO_GAP_MIN,
} microbenchmark_mode_t;


void    initialize_microbenchmark_data(microbenchmark_mode_t mode);
double  microbenchmark_get_max_latency();
double  microbenchmark_get_max_gap    ();
double  microbenchmark_get_div_latency();
double  microbenchmark_get_div_gap    ();
double  microbenchmark_get_div_latency();
double  microbenchmark_get_div_gap    ();
double  microbenchmark_get_foo_latency();
double  microbenchmark_get_foo_gap    ();


#endif /* MICROBENCHMARK_H */

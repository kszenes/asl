#include "include/microbenchmark.h"
#include "include/tsc_x86.h"
#include "include/foo.h"

#define CYCLES_REQUIRED 1e8
#define REP 50

double a, b;

double perf_test(funPtr_t f, const bool latency) {
    long start, end;
    if (latency) {
        start = start_tsc();
        f();
        end = stop_tsc(start);
        return (double) end;
    } else {

        double cycles = 0.;
        long num_runs = 100;
        double multiplier = 1;

        int n = 100;

        // Warm-up phase: we determine a number of executions that allows
        // the code to be executed for at least CYCLES_REQUIRED cycles.
        // This helps excluding timing overhead when measuring small runtimes.
        do {
            num_runs = num_runs * multiplier;
            start = start_tsc();
            for (size_t i = 0; i < num_runs; i++) {
                f();
            }
            end = stop_tsc(start);

            cycles = (double) end;
            multiplier = (CYCLES_REQUIRED) / (cycles);

        } while (multiplier > 2);

        //    list<double> cyclesList;

        // Actual performance measurements repeated REP times.
        // We simply store all results and compute medians during post-processing.
        double total_cycles = 0;
        for (size_t j = 0; j < REP; j++) {

            start = start_tsc();
            for (size_t i = 0; i < num_runs; ++i) {
                f();
            }
            end = stop_tsc(start);

            cycles = ((double) end) / num_runs;
            total_cycles += cycles;

            //        cyclesList.push_back(cycles);
        }
        total_cycles /= REP;

        cycles = total_cycles;
        return 1.0 / cycles; //round((100.0 * flops) / cycles) / 100.0;
    }
}

void initialize_microbenchmark_data (microbenchmark_mode_t mode) {
  /* You can use to initialize some data if needed */
    switch (mode) {
        case MAX_LAT: a = ((double) rand()) / RAND_MAX; b = ((double) rand()) / RAND_MAX; break;
        case MAX_GAP:
        case DIV_LAT: a = ((double) rand()) / RAND_MAX; b = ((double) rand()) / RAND_MAX; break;
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
    return std::max(a, b);
}

double microbenchmark_get_max_gap() {
    /* Implement your microbenchmark benchmark here */
    return std::max(a, b);
}

double microbenchmark_get_div_latency() {
    /* Implement your microbenchmark benchmark here */
    return a / b;
}

double microbenchmark_get_div_gap() {
    /* Implement your microbenchmark benchmark here */
    return a / b;
}

double microbenchmark_get_foo_latency() {
    /* Implement your microbenchmark benchmark here */
    return foo(a);
}

double microbenchmark_get_foo_gap() {
    /* Implement your microbenchmark benchmark here */
    return foo(a);
}
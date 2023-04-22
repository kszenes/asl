#include <iostream>
#include <iomanip>
#include <cmath>
#include "include/microbenchmark.h"
#include <fenv.h>

using namespace std;



void run_microbenchmark(microbenchmark_mode_t test) {
    funPtr_t f;
    bool latency;
    
    string test_name;
    switch (test) {
      case MAX_LAT        : test_name = "max latency      "; f = microbenchmark_get_max_latency; latency =  true; break;
      case MAX_GAP        : test_name = "max gap          "; f = microbenchmark_get_max_gap    ; latency = false; break;
      case DIV_LAT        : test_name = "div latency      "; f = microbenchmark_get_div_latency; latency =  true; break;
      case DIV_GAP        : test_name = "div gap          "; f = microbenchmark_get_div_gap    ; latency = false; break;
      case DIV_LAT_MIN    : test_name = "div latency (min)"; f = microbenchmark_get_div_latency; latency =  true; break;
      case DIV_GAP_MIN    : test_name = "div gap     (min)"; f = microbenchmark_get_div_gap    ; latency = false; break;
      case FOO_LAT        : test_name = "foo latency      "; f = microbenchmark_get_foo_latency; latency =  true; break;
      case FOO_GAP        : test_name = "foo gap          "; f = microbenchmark_get_foo_gap    ; latency = false; break;
      case FOO_LAT_MIN    : test_name = "foo latency (min)"; f = microbenchmark_get_foo_latency; latency =  true; break;
      case FOO_GAP_MIN    : test_name = "foo gap     (min)"; f = microbenchmark_get_foo_gap    ; latency = false; break;
      default: cout << "Out of range" << endl;  return;
    }
    
    /* Initialize and run microbenchmark */
    initialize_microbenchmark_data(test);
    double cycles = perf_test(f, latency);
    
    /* Print results */
    cout << "Measured "  << test_name << " : ";
    cout << "\033[1;35m" << setw(8)  << cycles << "\033[0m" << " cyc";
    cout << endl;
}

int main () {
    // feenableexcept(FE_INVALID | FE_OVERFLOW | FE_DIVBYZERO); // Exception when nan is detected
    for (int i = START_TEST; i <= END_TEST; ++i) {
      auto test = (microbenchmark_mode_t) i;
      run_microbenchmark(test);
    }    
    cout << "End of benchmark" << endl;
    return 0;
}
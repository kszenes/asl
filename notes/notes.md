# Formulas to rederive and understand
- Number of accumulators for reduction based on latencies and throughput
- MMM block size based on cache size
- compiler optimizations
    - scalar replacement

# Instruction Level Parallelism: Number of accumulators for reduction

Reduction code:
```C
double reduce(const double* v, const size_t n) {
    double sum = 0.0;
    for (int i = 0; i < n; ++i) {
        sum = sum + v[i];
    }
    return sum;
}
```
Skylake microarchitecture floating point add
- Throughput: 2 flops/cycle (given by the number of ports supporting instruction)
- Latency = 4 cycles

@import "img/ilp-reduction.png"

**Number of accumulators = ceil(latency * throughput) = ceil(4 * 2) = 8**
Explanation: The processor can issue 2 flops / cycle and each flop requires 4 cycles to complete. In order to achieve best performance, we need to issue flop instructions each cycle. Since the first instruction will take 4 cycles to complete, we need to create separate accumulators for each issued flop instruction during this period in order to use ILP. Thus for 4 cycles, we issue 2 flop instruction and require 8 accumulators in total. After the 4 cycles the same accumulators can be reused since the previous operation has terminated.

# Compiler Optimizations
- Code Motion (e.g., Loop-invariant code motion): Remove redundant computations
  - Pull computation out of loop
- Strength Reduction: Replace costly operations with simpler ones
    - Power by bit shift
- Scalar Replacement: Copy memory variables that are reused into local variables
  - Prevents memory aliasing issues

  # Tile Size for GEMM

  @import "img/mmm-tiling.png"
  
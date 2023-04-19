CC=gcc
CFLAGS=-std=c99

.PHONY: all clean
all: unoptimized unvectorized optimized

unoptimized: mmm.c tsc_x86.h
	$(CC) $(CFLAGS) $< -O0 -o $@

unvectorized: mmm.c tsc_x86.h
	$(CC) $(CFLAGS) $< -O3 -fno-tree-vectorize -o $@

optimized: mmm.c tsc_x86.h
	$(CC) $(CFLAGS) $< -O3 -ffast-math -march=native -o $@

clean:
	rm -rf unoptimized unvectorized optimized

.PHONY: all
a.out: main.cpp comp.cpp mat.cpp common.h mat.cpp tsc_x86.h utils.h
	g++ -std=c++20 -O3 -march=native -mno-fma -fno-tree-vectorize main.cpp mat.cpp comp.cpp -o $@

clean:
	rm -rf a.out

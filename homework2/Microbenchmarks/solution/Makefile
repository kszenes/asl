.PHNOY: all
all: a.out

a.out: main.cpp microbenchmark.cpp include/*
	g++-12 -O3 -fno-tree-vectorize -march=native -mno-fma -fno-math-errno main.cpp microbenchmark.cpp

.PHONY: run
run: a.out
	./a.out

.PHONY: clean
clean:
	rm -rf a.out
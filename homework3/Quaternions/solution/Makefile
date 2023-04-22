a.out: *.cpp *.h
	g++ -O3 -std=c++20 -march=native -fno-tree-vectorize -ffp-contract=off comp.cpp main.cpp

.PHONY: clean
clean:
	rm -rf a.out
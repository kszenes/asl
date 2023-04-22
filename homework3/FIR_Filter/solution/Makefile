.PHONY: all
all: a.out debug

a.out: *.cpp *.h
	g++ -O3 -std=c++20 -march=native -fno-tree-vectorize -ffp-contract=off filter.cpp main.cpp

debug: *.cpp *.h
	g++ -O3 -g -std=c++20 -march=native -fno-tree-vectorize -ffp-contract=off filter.cpp main.cpp -o debug

.PHONY: clean
clean:
	rm -rf a.out debug.out
.PHONY: all
a.out: *.cpp *.h
	g++ -O3 -march=native -mno-fma -fno-tree-vectorize main.cpp mat.cpp comp.cpp -o $@

clean:
	rm -rf a.out

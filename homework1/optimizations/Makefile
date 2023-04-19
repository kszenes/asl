all: a.out

a.out: main.c
	gcc -O3 -mfma -fno-tree-vectorize $< -o $@


clean:
	-rm main test

main: main.c quad_tree.c rand_gen.c
	gcc main.c quad_tree.c rand_gen.c -o main

test: test.c quad_tree.c rand_gen.c
	gcc -ggdb test.c quad_tree.c rand_gen.c -o test

all: main test
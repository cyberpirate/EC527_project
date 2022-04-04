
clean:
	-rm main test

main: main.c oct_tree.c rand_gen.c linked_stack.c
	gcc main.c oct_tree.c rand_gen.c linked_stack.c -o main

main_debug: main.c oct_tree.c rand_gen.c linked_stack.c
	gcc -D DEBUG -ggdb main.c oct_tree.c rand_gen.c linked_stack.c -o main_debug

test: test.c oct_tree.c rand_gen.c linked_stack.c
	gcc -D DEBUG -ggdb test.c oct_tree.c rand_gen.c linked_stack.c -o test

all: main test
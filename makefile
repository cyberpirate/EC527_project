
clean:
	-rm main test main_debug

h_files = linked_stack.h oct_tree.h rand_gen.h utils.h params.h
c_files = linked_stack.c oct_tree.c rand_gen.c utils.c

main: $(h_files) $(c_files) main.c
	gcc -lm $(c_files) main.c -o main

main_debug: $(h_files) $(c_files) main.c
	gcc -D DEBUG -ggdb -lm $(c_files) main.c -o main_debug

test: $(h_files) $(c_files) test.c greatest.h
	gcc -D DEBUG -ggdb -lm $(c_files) test.c greatest.h -o test

all: main test
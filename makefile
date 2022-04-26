
clean:
	-rm main test main_debug

h_files = oct_tree.h rand_gen.h utils.h params.h
c_files = oct_tree.c rand_gen.c utils.c

main: $(h_files) $(c_files) main.c
	gcc -lpthread -lm $(c_files) -std=gnu99 main.c -o main

main_debug: $(h_files) $(c_files) main.c
	gcc -lpthread -D DEBUG -ggdb -lm $(c_files) -std=gnu99 main.c -o main_debug

test: $(h_files) $(c_files) test.c greatest.h
	gcc -lpthread -D DEBUG -ggdb -lm $(c_files) -std=gnu99 test.c greatest.h -o test

all: main test main_debug

clean:
	-rm main test main_debug

h_files = oct_tree.cuh rand_gen.cuh utils.cuh params.cuh profiler.cuh
# c_files = oct_tree.cu rand_gen.cu utils.cu
c_files = 
gpu_options = -code sm_86 -arch compute_86

main: $(h_files) $(c_files) main.cu
	nvcc -lpthread -lm $(c_files) main.cu -o main

main_debug: $(h_files) $(c_files) main.cu
	nvcc -lpthread -D DEBUG -lm $(c_files) main.cu -o main_debug

test: $(h_files) $(c_files) test.cu greatest.h
	nvcc -lpthread -D DEBUG -lm $(gpu_options) $(c_files) test.cu -o test

all: main test main_debug
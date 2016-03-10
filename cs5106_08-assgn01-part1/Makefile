all:
	gcc -c squeue_lib.c
	gcc -o main_1_1 main_1_1.c squeue_lib.o -pthread -lm

clean:
	rm -f squeue_lib.o main_1_1
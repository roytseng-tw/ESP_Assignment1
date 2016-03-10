all:
	gcc -c squeue_lib.c message.c #-Wall
	gcc -o main_1_1 main_1_1.c squeue_lib.o -pthread #-Wall

clean:
	rm -f squeue_lib.o main_1_1
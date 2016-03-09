all:
	gcc -c squeue_lib.c message.c
	gcc -o main_1_1 main_1_1.c squeue_lib.o message.o -pthread

clean:
	rm -f squeue_lib.o message.o
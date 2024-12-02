CFLAGS = -g -Wall -Wextra -O3 -pthread

all: main

main: main.o allocatore.o
	gcc main.o allocatore.o -o main

main.o: main.c
	gcc $(CFLAGS) -c main.c -o main.o

allocatore.o: allocatore.c allocatore.h
	gcc $(CFLAGS) -c allocatore.c -o allocatore.o

clean: clean_obj
	rm -f ./main

clean_obj:
	rm -f *.o
CFLAGS = -g -Wall -Wextra -O3

all: main

main: main.o buffer.o semaphore.o
	gcc main.o buffer.o semaphore.o -o main

main.o: main.c
	gcc $(CFLAGS) -c main.c -o main.o

buffer.o: buffer.c buffer.h
	gcc $(CFLAGS) -c buffer.c -o buffer.o

semaphore.o: semaphore.c semaphore.h
	gcc $(CFLAGS) -c semaphore.c -o semaphore.o

clean: clean_obj
	rm -f ./main

clean_obj:
	rm -f *.o
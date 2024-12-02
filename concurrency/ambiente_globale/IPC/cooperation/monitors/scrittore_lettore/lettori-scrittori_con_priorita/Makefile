CFLAGS = -g -Wall -Wextra -O3

all: main

main: main.o monitorLS.o monitor_hoare.o semaphore.o
	gcc main.o monitorLS.o monitor_hoare.o semaphore.o -o main

main.o: main.c
	gcc $(CFLAGS) -c main.c -o main.o

monitorLS.o: monitorLS.c monitorLS.h monitor_hoare.c monitor_hoare.h semaphore.c semaphore.h
	gcc $(CFLAGS) -c monitorLS.c -o monitorLS.o

monitor_hoare.o: monitor_hoare.c monitor_hoare.h semaphore.c semaphore.o
	gcc $(CFLAGS) -c monitor_hoare.c -o monitor_hoare.o

semaphore.o: semaphore.c semaphore.h
	gcc $(CFLAGS) -c semaphore.c -o semaphore.o

clean: clean_obj
	rm -f ./main

clean_obj:
	rm -f *.o
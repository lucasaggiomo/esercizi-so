CFLAGS = -g -Wall -Wextra -O3

all: main analizzatore elaboratore generatore

main: main.o matrice.o semaphore.o
	gcc main.o matrice.o semaphore.o -o main

analizzatore: analizzatore.o matrice.o semaphore.o
	gcc analizzatore.o matrice.o semaphore.o -o analizzatore

elaboratore: elaboratore.o matrice.o semaphore.o
	gcc elaboratore.o matrice.o semaphore.o -o elaboratore
	
generatore: generatore.o matrice.o semaphore.o
	gcc generatore.o matrice.o semaphore.o -o generatore

main.o: main.c
	gcc $(CFLAGS) -c main.c -o main.o

analizzatore.o: analizzatore.c
	gcc $(CFLAGS) -c analizzatore.c -o analizzatore.o

elaboratore.o: elaboratore.c
	gcc $(CFLAGS) -c elaboratore.c -o elaboratore.o

generatore.o: generatore.c
	gcc $(CFLAGS) -c generatore.c -o generatore.o

matrice.o: matrice.c matrice.h semaphore.c semaphore.h
	gcc $(CFLAGS) -c matrice.c -o matrice.o

semaphore.o: semaphore.c semaphore.h
	gcc $(CFLAGS) -c semaphore.c -o semaphore.o

clean: clean_obj
	rm -f ./main ./elaboratore ./generatore ./analizzatore

clean_obj:
	rm -f *.o
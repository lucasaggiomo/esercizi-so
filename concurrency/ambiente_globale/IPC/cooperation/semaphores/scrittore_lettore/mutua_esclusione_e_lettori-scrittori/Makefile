CFLAGS = -g -Wall -Wextra -O3

all: main docente studente

main: main.o procedure.o semaphore.o
	gcc main.o procedure.o semaphore.o -o main

docente: docente.o procedure.o semaphore.o
	gcc docente.o procedure.o semaphore.o -o docente

studente: studente.o procedure.o semaphore.o
	gcc studente.o procedure.o semaphore.o -o studente
	
main.o: main.c
	gcc $(CFLAGS) -c main.c -o main.o

docente.o: docente.c
	gcc $(CFLAGS) -c docente.c -o docente.o
	
studente.o: studente.c
	gcc $(CFLAGS) -c studente.c -o studente.o

procedure.o: procedure.c header.h semaphore.h
	gcc $(CFLAGS) -c procedure.c -o procedure.o

semaphore.o: semaphore.c semaphore.h
	gcc $(CFLAGS) -c semaphore.c -o semaphore.o

clean: clean_obj
	rm -f ./main ./docente ./studente

clean_obj:
	rm -f *.o
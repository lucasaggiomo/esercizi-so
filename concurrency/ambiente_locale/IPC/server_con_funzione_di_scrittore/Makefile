CFLAGS = -g -Wall -Wextra -O3

all: main

main: main.o aeroporto.o procedure.o semaphore.o
	gcc main.o aeroporto.o procedure.o semaphore.o -o main

main.o: main.c aeroporto.h
	gcc $(CFLAGS) -c main.c -o main.o

procedure.o: procedure.c procedure.h messaggi.h
	gcc $(CFLAGS) -c procedure.c -o procedure.o

aeroporto.o: aeroporto.c aeroporto.h semaphore.h
	gcc $(CFLAGS) -c aeroporto.c -o aeroporto.o

semaphore.o: semaphore.c semaphore.h
	gcc $(CFLAGS) -c semaphore.c -o semaphore.o

clean: clean_obj ipcrm
	rm -f ./main ./client ./server

clean_obj:
	rm -f ./*.o

ipcrm:
	ipcs -q | cut -d" " -f2 | tail -n +4 | xargs -I {} ipcrm -q {}
	ipcs -m | cut -d" " -f2 | tail -n +4 | xargs -I {} ipcrm -m {}
	ipcs -s | cut -d" " -f2 | tail -n +4 | xargs -I {} ipcrm -s {}

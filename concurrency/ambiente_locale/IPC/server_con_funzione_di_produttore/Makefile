CFLAGS = -g -Wall -Wextra -O3

all: main client server

main: main.o
	gcc main.o -o main

client: client.o semaphore.o buffer.o procedure.o
	gcc client.o semaphore.o buffer.o procedure.o -o client

server: server.o semaphore.o buffer.o procedure.o
	gcc server.o semaphore.o buffer.o procedure.o -o server

main.o: main.c
	gcc $(CFLAGS) -c main.c -o main.o

client.o: client.c procedure.h messaggi.h buffer.h
	gcc $(CFLAGS) -c client.c -o client.o

server.o: server.c procedure.h messaggi.h buffer.h
	gcc $(CFLAGS) -c server.c -o server.o

procedure.o: procedure.c procedure.h messaggi.h
	gcc $(CFLAGS) -c procedure.c -o procedure.o

buffer.o: buffer.c buffer.h semaphore.h
	gcc $(CFLAGS) -c buffer.c -o buffer.o

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

CFLAGS = -g -Wall -Wextra -O3

all: client server

client: client.o
	gcc client.o -o client

server: server.o semaphore.o buffer.o
	gcc server.o semaphore.o buffer.o -o server

client.o: client.c messaggi.h
	gcc $(CFLAGS) -c client.c -o client.o

server.o: server.c messaggi.h buffer.h
	gcc $(CFLAGS) -c server.c -o server.o

buffer.o: buffer.c buffer.h semaphore.h
	gcc $(CFLAGS) -c buffer.c -o buffer.o

semaphore.o: semaphore.c semaphore.h
	gcc $(CFLAGS) -c semaphore.c -o semaphore.o

clean: clean_obj ipcrm
	rm -f ./client ./server

clean_obj:
	rm -f ./*.o

ipcrm:
	ipcs -q | cut -d" " -f2 | tail -n +4 | xargs -I {} ipcrm -q {}
	ipcs -m | cut -d" " -f2 | tail -n +4 | xargs -I {} ipcrm -m {}
	ipcs -s | cut -d" " -f2 | tail -n +4 | xargs -I {} ipcrm -s {}

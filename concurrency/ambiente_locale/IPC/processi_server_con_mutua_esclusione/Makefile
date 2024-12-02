CFLAGS = -g -Wall -Wextra -O3

all: main server client

# LINK
main: main.o shared.o semaphore.o
	gcc main.o shared.o semaphore.o -o main

server: server.o shared.o semaphore.o
	gcc server.o shared.o semaphore.o -o server

client: client.o
	gcc client.o -o client

# COMPILE
main.o: main.c shared.h
	gcc $(CFLAGS) -c main.c -o main.o

server.o: server.c messaggi.h shared.h
	gcc $(CFLAGS) -c server.c -o server.o

client.o: client.c messaggi.h
	gcc $(CFLAGS) -c client.c -o client.o

shared.o: shared.c shared.h data.h semaphore.h
	gcc $(CFLAGS) -c shared.c -o shared.o

semaphore.o: semaphore.c semaphore.h
	gcc $(CFLAGS) -c semaphore.c -o semaphore.o

clean: clean_obj
	rm -f *.o
	rm -f main client server
	rm -f *~

clean_obj:
	rm -f *.o
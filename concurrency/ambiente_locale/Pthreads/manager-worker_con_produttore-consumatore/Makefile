CFLAGS = -g -Wall -Wextra -O3

all: main client server

main: main.o wrapper.o
	gcc -g main.o wrapper.o -o main

server: server.o wrapper.o shared.o
	gcc -g server.o wrapper.o shared.o -o server -pthread

client: client.o wrapper.o
	gcc -g client.o wrapper.o -o client

main.o: main.c
	gcc $(CFLAGS) -c main.c -o main.o

server.o: server.c
	gcc $(CFLAGS) -c server.c -o server.o

client.o: client.c
	gcc $(CFLAGS) -c client.c -o client.o

%.o: %.c %.h wrapper.c wrapper.h
	gcc $(CFLAGS) -c $< -o $@

clean_obj:
	rm -f *.o

clean: clean_obj
	rm -f ./main ./server ./client
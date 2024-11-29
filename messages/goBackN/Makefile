CFLAGS = -g -Wall -Wextra -O3

CLIENT = client.c app_client.c app_client.h transport_client.c transport_client.h transport.c transport.h window.c window.h
CLIENT_OBJ = client.o app_client.o transport_client.o window.o transport.o

SERVER = server.c app_server.c app_server.h transport_server.c transport_server.h transport.c transport.h
SERVER_OBJ = server.o app_server.o transport_server.o transport.o

all: master client server

master: master.o
	gcc -g master.o -o master

client: $(CLIENT_OBJ)
	gcc -g $(CLIENT_OBJ) -o client -pthread

server: $(SERVER_OBJ)
	gcc -g $(SERVER_OBJ) -o server

master.o: master.c
	gcc $(CFLAGS) -c master.c -o master.o

client.o: $(CLIENT)
	gcc $(CFLAGS) -c client.c -o client.o

server.o: $(SERVER)
	gcc $(CFLAGS) -c server.c -o server.o

%.o: %.c %.h messages.h
	gcc $(CFLAGS) -c $< -o $@

clean: clean_obj
	rm -f ./master ./client ./server

clean_obj:
	rm -f *.o

CFLAGS = -g -Wall -Wextra -O3

all: master app_client app_server

master: master.o
	gcc -g master.o -o master

app_server: app_server.o
	gcc -g app_server.o -o app_server -pthread

app_client: app_client.o
	gcc -g app_client.o -o app_client -pthread

transport_server: transport_server.o
	gcc -g transport_server.o -o app_server -pthread

transport_client: transport_client.o
	gcc -g transport_client.o -o app_client -pthread

main.o: master.c
	gcc $(CFLAGS) -c master.c

app_server.o: app_server.c app_msg.h
	gcc $(CFLAGS) -c app_server.c

app_client.o: app_client.c app_msg.h
	gcc $(CFLAGS) -c app_client.c

transport_server.o: transport_server.c app_msg.h msg_dati.h msg_ack.h
	gcc $(CFLAGS) -c transport_server.c

transport_client.o: transport_client.c app_msg.h msg_dati.h msg_ack.h
	gcc $(CFLAGS) -c transport_client.c

clean: clean_obj
	rm -f ./master ./app_client ./app_server

clean_obj:
	rm -f *.o

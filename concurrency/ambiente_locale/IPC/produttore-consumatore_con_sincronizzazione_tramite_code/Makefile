CFLAGS = -g -Wall -Wextra -O3

all: main produttore consumatore

main: main.o
	gcc main.o -o main

produttore: produttore.o
	gcc produttore.o -o produttore

consumatore: consumatore.o
	gcc consumatore.o -o consumatore

main.o: main.c common.h
	gcc $(CFLAGS) -c main.c -o main.o

produttore.o: produttore.c common.h
	gcc $(CFLAGS) -c produttore.c -o produttore.o

consumatore.o: consumatore.c common.h
	gcc $(CFLAGS) -c consumatore.c -o consumatore.o

clean: clean_obj
	rm -f ./main ./produttore ./consumatore

clean_obj:
	rm -f ./*.o
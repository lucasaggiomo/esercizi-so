CFLAGS = -g -Wall -Wextra -O3

LIBRARIES = -pthread

all: main

main: main.o negozio.o
	gcc $(CFLAGS) $(LIBRARIES) main.o negozio.o -o main

main.o: main.c negozio.h articolo.h
	gcc -c main.c -o main.o

negozio.o: negozio.c negozio.h articolo.h
	gcc -c negozio.c -o negozio.o

clean_obj:
	rm -f *.o

clean: clean_obj
	rm -f ./main
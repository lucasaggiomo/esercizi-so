CFLAGS = -g -Wall -Wextra -O3

LIBRARIES = -pthread

all: main

main: main.o dvd.o
	gcc $(CFLAGS) $(LIBRARIES) main.o dvd.o -o main

main.o: main.c dvd.h
	gcc -c main.c -o main.o

dvd.o: dvd.c dvd.h
	gcc -c dvd.c -o dvd.o

clean_obj:
	rm -f *.o

clean: clean_obj
	rm -f ./main
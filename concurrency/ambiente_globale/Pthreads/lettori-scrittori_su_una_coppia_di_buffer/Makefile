CFLAGS = -g -Wall -Wextra -O3

LIBRARIES = -pthread

all: main

main: main.o buffer.o
	gcc $(CFLAGS) $(LIBRARIES) main.o buffer.o -o main

main.o: main.c buffer.h
	gcc -c main.c -o main.o

buffer.o: buffer.c buffer.h
	gcc -c buffer.c -o buffer.o

clean_obj:
	rm -f *.o

clean: clean_obj
	rm -f ./main
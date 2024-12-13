CFLAGS = -g -Wall -Wextra -O3

LIBRARIES = -pthread

all: main

main: main.o buffer_stato.o
	gcc -g main.o buffer_stato.o -o main $(LIBRARIES)

main.o: main.c
	gcc $(CFLAGS) -c main.c -o main.o

%.o: %.c %.h
	gcc $(CFLAGS) -c $< -o $@

clean_obj:
	rm -f *.o

clean: clean_obj
	rm -f ./main
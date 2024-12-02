CFLAGS = -g -Wall -Wextra -O3

LIBRARIES = -pthread

all: main

main: main.o procedure.o wrapper.o shared.o
	gcc -g main.o procedure.o wrapper.o shared.o -o main $(LIBRARIES)

main.o: main.c
	gcc $(CFLAGS) -c main.c -o main.o

%.o: %.c %.h wrapper.c wrapper.h messaggi.c messaggi.h
	gcc $(CFLAGS) -c $< -o $@

clean_obj:
	rm -f *.o

clean: clean_obj
	rm -f ./main
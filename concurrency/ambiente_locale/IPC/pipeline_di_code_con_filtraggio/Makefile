CFLAGS = -g -Wall -Wextra -O3

all: main

main: main.o wrapper.o procedure.o
	gcc -g main.o wrapper.o procedure.o -o main

main.o: main.c
	gcc $(CFLAGS) -c main.c -o main.o

%.o: %.c %.h wrapper.h
	gcc $(CFLAGS) -c $< -o $@

clean_obj:
	rm -f *.o

clean: clean_obj
	rm -f ./main
CFLAGS = -g -Wall -Wextra -O3

all: main

main: main.o procedure.o
	gcc -g main.o procedure.o -o main

main.o: main.c procedure.h
	gcc $(CFLAGS) -c main.c -o main.o

procedure.o: procedure.c procedure.h messaggi.h
	gcc $(CFLAGS) -c procedure.c -o procedure.o

clean_obj:
	rm -f *.o

clean: clean_obj
	rm -f ./main
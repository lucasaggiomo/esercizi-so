CFLAGS = -g -Wall -Wextra -O3

all: main

main: main.o procedure.o messaggi.o
	gcc -g main.o procedure.o messaggi.o -o main

main.o: main.c procedure.h messaggi.h
	gcc $(CFLAGS) -c main.c -o main.o

procedure.o: procedure.c procedure.h messaggi.h
	gcc $(CFLAGS) -c procedure.c -o procedure.o

messaggi.o: messaggi.c messaggi.h
	gcc $(CFLAGS) -c messaggi.c -o messaggi.o

clean_obj:
	rm -f *.o

clean: clean_obj
	rm -f ./main
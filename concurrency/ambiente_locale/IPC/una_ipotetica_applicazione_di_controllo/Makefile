CFLAGS = -g -Wall -Wextra -O3

all: main interfaccia macchina

main: main.o
	gcc -g main.o -o main

interfaccia: interfaccia.o messaggi.o
	gcc -g interfaccia.o messaggi.o -o interfaccia

macchina: macchina.o messaggi.o procedure.o
	gcc -g macchina.o messaggi.o procedure.o -o macchina

main.o: main.c
	gcc $(CFLAGS) -c main.c -o main.o

interfaccia.o: interfaccia.c messaggi.h
	gcc $(CFLAGS) -c interfaccia.c -o interfaccia.o

macchina.o: macchina.c procedure.h
	gcc $(CFLAGS) -c macchina.c -o macchina.o

procedure.o: procedure.c procedure.h messaggi.h
	gcc $(CFLAGS) -c procedure.c -o procedure.o

messaggi.o: messaggi.c messaggi.h
	gcc $(CFLAGS) -c messaggi.c -o messaggi.o

clean_obj:
	rm -f *.o

clean: clean_obj
	rm -f ./main ./interfaccia ./macchina
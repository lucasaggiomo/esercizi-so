CFLAGS = -g -Wall -Wextra -O3 -lm

all: master consumatore produttore

master: master.o buffer_circolare.o semaphore.o
	gcc master.o buffer_circolare.o semaphore.o -o master -lm

produttore: produttore.o buffer_circolare.o semaphore.o
	gcc produttore.o buffer_circolare.o semaphore.o -o produttore -lm

consumatore: consumatore.o buffer_circolare.o semaphore.o
	gcc consumatore.o buffer_circolare.o semaphore.o -o consumatore -lm

master.o: master.c
	gcc $(CFLAGS) -c master.c -o master.o

produttore.o: produttore.c
	gcc $(CFLAGS) -c produttore.c -o produttore.o

consumatore.o: consumatore.c
	gcc $(CFLAGS) -c consumatore.c -o consumatore.o

buffer_circolare.o: buffer_circolare.c buffer_circolare.h semaphore.o
	gcc $(CFLAGS) -c buffer_circolare.c -o buffer_circolare.o

semaphore.o: semaphore.c semaphore.h
	gcc $(CFLAGS) -c semaphore.c -o semaphore.o

clean: clean_obj
	rm -f ./master ./consumatore ./produttore

clean_obj:
	rm -f *.o
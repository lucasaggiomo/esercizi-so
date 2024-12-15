CFLAGS = -g -O3 -Wall -Wextra
LIBRARIES = -pthread

all: main

main: main.o bufchar.o
	gcc main.o bufchar.o -o main $(LIBRARIES)

main.o: main.c bufchar.h
	gcc $(CFLAGS) -c main.c -o main.o

bufchar.o: bufchar.c bufchar.h
	gcc $(CFLAGS) -c bufchar.c -o bufchar.o

clean: clean_obj
	rm main

clean_obj:
	rm ./*.o

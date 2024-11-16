CFLAGS = -Wall -Wextra -pthread -O3
BIN = semProgram
OBJ = $(BIN).o queue.o semaphore.o

all: $(BIN)

$(BIN): $(OBJ)
	gcc $(OBJ) -o $(BIN)

$(BIN).o: $(BIN).c
	gcc $(CFLAGS) -c $< -o $@

queue.o: queue.c queue.h
	gcc $(CFLAGS) -c $< -o $@

semaphore.o: semaphore.c semaphore.h
	gcc $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o
	rm -f $(BIN)
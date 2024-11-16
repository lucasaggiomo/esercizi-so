CFLAGS = -Wall -Wextra -pthread -O3
BIN = main
OBJ = $(BIN).o semaphore.o

all: $(BIN)

$(BIN): $(OBJ)
	gcc $(OBJ) -o $(BIN)

$(BIN).o: $(BIN).c
	gcc $(CFLAGS) -c $< -o $@

%.o: %.c %.h
	gcc $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o
	rm -f $(BIN)
CFLAGS = -Wall -Wextra -pthread -O3
BIN = lock
OBJ = $(BIN).o

all: $(BIN)

$(BIN): $(BIN).o
	gcc $(BIN).o -o $(BIN)

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o
	rm -f lock
	rm -f peterson
	rm -f semaphore
	rm -f $(BIN)
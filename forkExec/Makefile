CFLAGS = -Wall -Wextra
BIN = main
OBJ = $(BIN).o

all: $(BIN)

$(BIN): $(BIN).o
	gcc $(CFLAGS) $(BIN).o -o $(BIN)

%.o: %.c
	gcc -c $< -o $@

clean:
	rm -f *.o
	rm -f $(BIN)
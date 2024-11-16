CFLAGS = -g -Wall -Wextra -O3
LFLAGS = -pthread
BIN = main
OBJ = $(BIN).o vettore.o

all: $(BIN) clean_obj

$(BIN): $(OBJ)
	gcc $(LFLAGS) $(OBJ) -o $@

$(BIN).o: $(BIN).c
	gcc $(CFLAGS) -c $< -o $@

%.o: %.c %.h
	gcc $(CFLAGS) -c $< -o $@

clean: clean_obj
	rm -f $(BIN)

clean_obj:
	rm -f *.o
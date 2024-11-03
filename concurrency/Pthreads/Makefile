CFLAGS = -g -Wall -Wextra -O3
LFLAGS = -pthread
BIN = prodotto_scalare
OBJ = $(BIN).o

all: $(BIN) clean_obj

%(BIN): %(BIN).o
	gcc $(LFLAGS) -c $< -o $@

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

clean: clean_obj
	rm -f $(BIN)

clean_obj:
	rm -f *.o
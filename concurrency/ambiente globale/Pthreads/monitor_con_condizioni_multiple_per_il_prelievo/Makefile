CFLAGS = -g -Wall -Wextra -O3 -pthread
BIN = main
OBJ = $(BIN).o monitor_spedizioni.o

all: $(BIN) clean_obj

$(BIN): $(OBJ)
	gcc $(OBJ) -o $@

$(BIN).o: $(BIN).c
	gcc $(CFLAGS) -c $< -o $@

%.o: %.c %.h
	gcc $(CFLAGS) -c $< -o $@

clean: clean_obj
	rm -f $(BIN)

clean_obj:
	rm -f *.o
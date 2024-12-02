CFLAGS = -g -Wall -Wextra -O3
LFLAGS = -pthread
BIN = counter
OBJ = $(BIN).o

all: $(BIN) clean_obj

$(BIN): $(OBJ)
	gcc $(LFLAGS) $(OBJ) -o $@

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

clean: clean_obj
	rm -f $(BIN)

clean_obj:
	rm -f *.o
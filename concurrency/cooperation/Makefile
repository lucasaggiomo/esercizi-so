CFLAGS = -Wall -Wextra -pthread -O3
BIN = circularbuffer
OBJ = $(BIN).o $(UTILS_OBJ)
UTILS_DIR = ../utils
UTILS_SRC = $(UTILS_DIR)/semaphore.c
UTILS_OBJ = $(UTILS_DIR)/semaphore.o

all: $(BIN)

$(BIN): $(OBJ)
	gcc $(OBJ) -o $(BIN)

$(BIN).o: $(BIN).c
	gcc $(CFLAGS) -c $< -o $@

$(UTILS_OBJ): $(UTILS_SRC) $(UTILS_DIR)/semaphore.h
	gcc $(CFLAGS) -c $< -o $@

%.o: %.c %.h
	gcc $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o
	rm -f $(BIN)
	rm -f $(UTILS_DIR)/*.o
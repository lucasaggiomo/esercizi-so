CFLAGS = -Wall -Wextra -pthread -O3

SOURCE_DIR = ./scrittore_lettore

BIN = monobuffer_ls_starvation

BIN_FULL = $(SOURCE_DIR)/$(BIN)

OBJ = $(BIN_FULL).o $(UTILS_OBJ)

UTILS_DIR = ../utils
UTILS_SRC = $(UTILS_DIR)/semaphore.c
UTILS_OBJ = $(UTILS_DIR)/semaphore.o

all: $(BIN_FULL)

$(BIN_FULL): $(OBJ)
	gcc $(OBJ) -o $(BIN_FULL)

$(BIN_FULL).o: $(BIN_FULL).c
	gcc $(CFLAGS) -c $< -o $@

$(UTILS_OBJ): $(UTILS_SRC) $(UTILS_DIR)/semaphore.h
	gcc $(CFLAGS) -c $< -o $@

$(SOURCE_DIR)/%.o: $(SOURCE_DIR)/%.c $(SOURCE_DIR)/%.h
	gcc $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SOURCE_DIR)/*.o
	rm -f $(BIN_FULL)
	rm -f $(UTILS_DIR)/*.o
CFLAGS = -g -Wall -Wextra -O3

SOURCE_DIR = ./produttore_consumatore

BIN = monobuffer_hoare

BIN_FULL = $(SOURCE_DIR)/$(BIN)

OBJ = $(BIN_FULL).o semaphore.o monitor_hoare.o

all: $(BIN_FULL) clean_obj

$(BIN_FULL): $(OBJ)
	gcc $(OBJ) -o $(BIN_FULL)

$(BIN_FULL).o: $(BIN_FULL).c
	gcc $(CFLAGS) -c $< -o $@

semaphore.o: semaphore.c semaphore.h
	gcc $(CFLAGS) -c $< -o $@

monitor_hoare.o: monitor_hoare.c monitor_hoare.h
	gcc $(CFLAGS) -c $< -o $@

$(SOURCE_DIR)/%.o: $(SOURCE_DIR)/%.c $(SOURCE_DIR)/%.h
	gcc $(CFLAGS) -c $< -o $@

clean: clean_obj
	rm -f $(BIN_FULL)

clean_obj:
	rm -f $(SOURCE_DIR)/*.o
	rm -f *.o
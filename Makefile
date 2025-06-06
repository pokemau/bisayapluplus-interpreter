CC = gcc

CFLAGS  = -std=c11 -O2 -g -Wall -Wextra -Wpedantic

SRC = $(wildcard src/**/*.c) $(wildcard src/*.c) $(wildcard src/**/**/*.c) $(wildcard src/**/**/**/*.c)
OBJ = $(SRC:.c=.o)
BIN = bin

.PHONY: all clean dirs run bisaya++

all: dirs bisaya++

dirs:
	mkdir -p ./$(BIN)

run: all
	$(BIN)/bisaya++

bisaya++: $(OBJ)
	$(CC) -o $(BIN)/bisaya++ $^ $(LDFLAGS)
	./$(BIN)/bisaya++ test.b++

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -rf compile_commands.json $(BIN) $(OBJ)

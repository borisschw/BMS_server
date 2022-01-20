# CC		:= /opt/gcc-linaro-7.3.1-2018.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-gcc
CC := g++
C_FLAGS := -pthread -Wall -Wextra -Wno-unused-parameter -lpthread

BIN		:= bin
SRC		:= src
INCLUDE	:= include

EXECUTABLE	:= bmsServer.out


all: $(BIN)/$(EXECUTABLE)

clean:
	$(RM) $(BIN)/$(EXECUTABLE)

run: all
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*
	$(CC) $(C_FLAGS) -I$(INCLUDE) $^ -o $@

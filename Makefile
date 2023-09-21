CC = gcc
CFLAGS = -std=c99 -pedantic -O2 -Wall -Werror -I./raylib/src
LIBS = -L./raylib/src -lraylib -lm
SRC = src/main.c
TARGET = M7Demo

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)

.PHONY: all clean

setup:
	@cd ./raylib/src && make

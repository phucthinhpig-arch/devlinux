# Compiler definitions
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -Werror -std=c99

# Targets
.PHONY: all clean

all: main

main: main.c
	$(CC) $(CFLAGS) -o main main.c

clean:
	rm -f main
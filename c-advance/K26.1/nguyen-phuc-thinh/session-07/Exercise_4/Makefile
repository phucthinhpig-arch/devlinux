CC = gcc
CFLAGS = -Wall -Wextra -pedantic -Werror -std=c99

# Default configuration flag. 
# Overridable via command line using: make FLAGS="-DCONFIG_..."
FLAGS ?= -DCONFIG_WIFI_ENABLED

.PHONY: all clean

all: main

main: main.c
	$(CC) $(CFLAGS) $(FLAGS) -o main main.c

clean:
	rm -f main
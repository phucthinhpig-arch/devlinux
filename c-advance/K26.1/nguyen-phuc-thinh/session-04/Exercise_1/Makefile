CC = gcc
CFLAGS = -Wall -Wextra -pedantic -Werror -std=c99

all: main

main: main.c i_display.h console_display.c console_display.h dummy_display.c dummy_display.h
    $(CC) $(CFLAGS) -o main main.c console_display.c dummy_display.c

clean:
    rm -f main

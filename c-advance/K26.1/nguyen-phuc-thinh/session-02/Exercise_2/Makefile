CC ?= gcc
TARGET := stack_monitor
CFLAGS := -Wall -Wextra -pedantic -Werror -std=c99

.PHONY: all clean

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) -o $(TARGET) main.c

clean:
	rm -f $(TARGET) *.o

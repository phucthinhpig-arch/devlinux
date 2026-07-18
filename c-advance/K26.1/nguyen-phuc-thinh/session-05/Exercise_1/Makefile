CC = gcc
CFLAGS = -Wall -Wextra -pedantic -Werror -std=c99
SRCS = hw_array_of_pointers.c
OBJS = $(SRCS:.c=.o)
TARGET = hw_array_of_pointers

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
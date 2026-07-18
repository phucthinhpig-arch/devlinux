CC = gcc
CFLAGS = -Wall -Wextra -pedantic -Werror -std=c99
SRCS = hw_jump_table.c
OBJS = $(SRCS:.c=.o)
TARGET = hw_jump_table

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
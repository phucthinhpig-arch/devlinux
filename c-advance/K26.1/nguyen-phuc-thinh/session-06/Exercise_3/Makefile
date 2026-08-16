CC = gcc
CFLAGS = -Wall -Wextra -pedantic -Werror -std=c99
SRCS = main.c
OBJS = $(SRCS:.c=.o)
TARGET = Exercise_3_App

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
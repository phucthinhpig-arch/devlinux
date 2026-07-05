CC := gcc
CFLAGS := -Wall -Wextra -pedantic -Werror -std=c99
TARGET := memory_map

.PHONY: all clean analyze

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) main.c -o $(TARGET)

analyze: $(TARGET)
	cppcheck --enable=all --inconclusive --std=c99 --suppress=missingIncludeSystem main.c
	clang-tidy main.c -- $(CFLAGS)

clean:
	rm -f $(TARGET)

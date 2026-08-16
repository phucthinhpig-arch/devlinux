CC = gcc
# Note: Using -Wno-variadic-macros to strictly pass -pedantic in C99 with GCC extensions
CFLAGS = -Wall -Wextra -pedantic -Werror -std=c99 -Wno-variadic-macros

# Define the max log level here. Try changing to LOG_LEVEL_WARNING to test!
LOG_FLAG = -DLOG_LEVEL_MAX=LOG_LEVEL_DEBUG

.PHONY: all clean

all: main

main: main.c
	$(CC) $(CFLAGS) $(LOG_FLAG) -o main main.c

clean:
	rm -f mainCC = gcc
# Note: Using -Wno-variadic-macros to strictly pass -pedantic in C99 with GCC extensions
CFLAGS = -Wall -Wextra -pedantic -Werror -std=c99 -Wno-variadic-macros

# Define the max log level here. Try changing to LOG_LEVEL_WARNING to test!
LOG_FLAG = -DLOG_LEVEL_MAX=LOG_LEVEL_DEBUG

.PHONY: all clean

all: main

main: main.c
	$(CC) $(CFLAGS) $(LOG_FLAG) -o main main.c

clean:
	rm -f main
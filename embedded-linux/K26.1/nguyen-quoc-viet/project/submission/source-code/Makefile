CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinc
LDFLAGS = -lcrypto -lpthread

INC_DIR = inc
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

SERVER_SRC_DIR = src/server
CLIENT_SRC_DIR = src/client

SERVER_SRCS = $(SERVER_SRC_DIR)/main.c $(SERVER_SRC_DIR)/auth.c $(SERVER_SRC_DIR)/broadcast.c $(SERVER_SRC_DIR)/history.c $(SERVER_SRC_DIR)/users.c $(SERVER_SRC_DIR)/status.c
CLIENT_SRCS = $(CLIENT_SRC_DIR)/main.c

SERVER_OBJS = $(patsubst $(SERVER_SRC_DIR)/%.c,$(OBJ_DIR)/server_%.o,$(SERVER_SRCS))
CLIENT_OBJS = $(patsubst $(CLIENT_SRC_DIR)/%.c,$(OBJ_DIR)/client_%.o,$(CLIENT_SRCS))

TARGET_SERVER = $(BUILD_DIR)/chat-server
TARGET_CLIENT = $(BUILD_DIR)/chat-client

HEADERS = $(INC_DIR)/chat.h

.PHONY: all clean rebuild help

all: $(TARGET_SERVER) $(TARGET_CLIENT)

$(TARGET_SERVER): directories $(SERVER_OBJS)
	$(CC) $(CFLAGS) -o $@ $(SERVER_OBJS) $(LDFLAGS)
	@echo "✅ Build successful: $(TARGET_SERVER)"

$(TARGET_CLIENT): directories $(CLIENT_OBJS)
	$(CC) $(CFLAGS) -o $@ $(CLIENT_OBJS)
	@echo "✅ Build successful: $(TARGET_CLIENT)"

directories:
	@mkdir -p $(BUILD_DIR) $(OBJ_DIR)

$(OBJ_DIR)/server_%.o: $(SERVER_SRC_DIR)/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/client_%.o: $(CLIENT_SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)
	@echo "🧹 Cleaned"

rebuild: clean all

help:
	@echo "Chat Server & Client — Makefile"
	@echo "make              - Build both server and client"
	@echo "make clean        - Remove build artifacts"
	@echo "make rebuild      - Clean and build"
	@echo "make help         - Show this help"

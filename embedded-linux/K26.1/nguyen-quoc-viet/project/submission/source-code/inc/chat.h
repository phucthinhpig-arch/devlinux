#ifndef CHAT_H
#define CHAT_H

#include <netinet/in.h>

#define PORT 5000
#define MAX_CLIENTS 100
#define BUFFER_SIZE 4096
#define MAX_USERNAME 64
#define MAX_PASSWORD 256

typedef struct {
	int fd;
	char username[MAX_USERNAME];
	int authenticated;
	struct sockaddr_in addr;  /* Client address (IP:port) */
} client_t;

/* auth.c */
int authenticate_user(const char *username, const char *password);
int register_user(const char *username, const char *password);

/* broadcast.c */
void broadcast_message(const char *username, const char *text);
void save_message_log(const char *username, const char *text);

/* history.c */
void send_message_history(int client_fd, const char *username);

/* users.c */
void send_all_users_list(int client_fd);

#endif

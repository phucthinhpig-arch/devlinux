#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

#define PORT 5000
#define MAX_CLIENTS 100
#define BUFFER_SIZE 4096

typedef struct {
	int fd;
	char username[64];
	int authenticated;
	struct sockaddr_in addr;
} client_t;

client_t clients[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
volatile int running = 1;

extern int authenticate_user(const char *username, const char *password);
extern int register_user(const char *username, const char *password);
extern void broadcast_message(const char *username, const char *text);
extern void send_message_history(int client_fd, const char *username);
extern void send_all_users_list(int client_fd);
extern void print_user_status(void);

void signal_handler(int sig)
{
	(void)sig;
	running = 0;
}

void send_message(int client_fd, const char *text)
{
	send(client_fd, text, strlen(text), 0);
}

void send_error(int client_fd, const char *error)
{
	char buf[512];
	snprintf(buf, sizeof(buf), "ERR:%s\n", error);
	send_message(client_fd, buf);
}

void close_client(client_t *client)
{
	if (client->fd < 0)
		return;

	int was_authenticated = client->authenticated;
	char username[64];
	strcpy(username, client->username);

	close(client->fd);

	pthread_mutex_lock(&clients_mutex);
	if (client->authenticated)
		client_count--;
	client->fd = -1;
	client->authenticated = 0;
	pthread_mutex_unlock(&clients_mutex);

	if (was_authenticated) {
		printf("[-] %s logged out\n", username);
		print_user_status();
	}
}

void route_message(client_t *client, const char *message);

void *client_handler(void *arg)
{
	client_t *client = (client_t *)arg;
	char buf[BUFFER_SIZE];
	char line[BUFFER_SIZE];
	int pos = 0;
	int n;

	while (running && client->fd >= 0) {
		n = recv(client->fd, buf, sizeof(buf), 0);

		if (n <= 0) {
			close_client(client);
			break;
		}

		for (int i = 0; i < n; i++) {
			line[pos++] = buf[i];

			if (buf[i] == '\n' || pos >= BUFFER_SIZE - 1) {
				if (pos > 0) {
					line[pos - 1] = '\0';
					route_message(client, line);
				}
				pos = 0;
			}
		}
	}

	return NULL;
}

void handle_login(client_t *client, const char *credentials)
{
	char username[64], password[256];
	const char *colon = strchr(credentials, ':');

	if (!colon) {
		send_error(client->fd, "INVALID_FORMAT");
		return;
	}

	int user_len = colon - credentials;
	if (user_len >= 64)
		user_len = 63;

	strncpy(username, credentials, user_len);
	username[user_len] = '\0';
	strcpy(password, colon + 1);

	if (authenticate_user(username, password)) {
		strcpy(client->username, username);
		client->authenticated = 1;
		pthread_mutex_lock(&clients_mutex);
		client_count++;
		pthread_mutex_unlock(&clients_mutex);
		printf("[+] %s logged in\n", username);
		print_user_status();
		send_message(client->fd, "OK:LOGIN\n");
		send_message_history(client->fd, username);
	} else {
		send_error(client->fd, "INVALID_CREDENTIALS");
	}
}

void handle_register(client_t *client, const char *credentials)
{
	char username[64], password[256];
	const char *colon = strchr(credentials, ':');

	if (!colon) {
		send_error(client->fd, "INVALID_FORMAT");
		return;
	}

	int user_len = colon - credentials;
	if (user_len >= 64)
		user_len = 63;

	strncpy(username, credentials, user_len);
	username[user_len] = '\0';
	strcpy(password, colon + 1);

	if (register_user(username, password)) {
		strcpy(client->username, username);
		client->authenticated = 1;
		pthread_mutex_lock(&clients_mutex);
		client_count++;
		pthread_mutex_unlock(&clients_mutex);
		printf("[+] %s registered and logged in\n", username);
		print_user_status();
		send_message(client->fd, "OK:REGISTER\n");
		send_message_history(client->fd, username);
	} else {
		send_error(client->fd, "REGISTER_FAILED");
	}
}

void handle_message(client_t *client, const char *text)
{
	if (!client->authenticated) {
		send_error(client->fd, "NOT_AUTHENTICATED");
		return;
	}
	broadcast_message(client->username, text);
}

void handle_logout(client_t *client)
{
	close_client(client);
}

void handle_userlist(client_t *client)
{
	char buf[BUFFER_SIZE] = "USERS:";

	pthread_mutex_lock(&clients_mutex);
	for (int i = 0; i < MAX_CLIENTS; i++) {
		if (clients[i].fd >= 0 && clients[i].authenticated) {
			if (strlen(buf) > 6)
				strcat(buf, ",");
			strcat(buf, clients[i].username);
		}
	}
	pthread_mutex_unlock(&clients_mutex);

	strcat(buf, "\n");
	send_message(client->fd, buf);
}

void route_message(client_t *client, const char *message)
{
	if (strncmp(message, "LOGIN:", 6) == 0) {
		handle_login(client, message + 6);
	} else if (strncmp(message, "REGISTER:", 9) == 0) {
		handle_register(client, message + 9);
	} else if (strncmp(message, "MSG:", 4) == 0) {
		handle_message(client, message + 4);
	} else if (strcmp(message, "LOGOUT") == 0) {
		handle_logout(client);
	} else if (strcmp(message, "USERLIST") == 0) {
		handle_userlist(client);
	} else if (strcmp(message, "ALLUSERS") == 0) {
		send_all_users_list(client->fd);
	} else if (strcmp(message, "HISTORY") == 0) {
		if (client->authenticated)
			send_message_history(client->fd, client->username);
	} else {
		send_error(client->fd, "INVALID_FORMAT");
	}
}

int main(int argc, char *argv[])
{
	int listen_fd, client_fd;
	struct sockaddr_in server_addr, client_addr;
	socklen_t addr_len;
	pthread_t thread;
	int i;

	(void)argc;
	(void)argv;

	for (i = 0; i < MAX_CLIENTS; i++)
		clients[i].fd = -1;

	printf("Starting Chat Server on port %d...\n", PORT);

	signal(SIGTERM, signal_handler);
	signal(SIGINT, signal_handler);

	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_fd < 0) {
		perror("socket");
		return 1;
	}

	int reuse = 1;
	setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		perror("bind");
		return 1;
	}

	if (listen(listen_fd, 5) < 0) {
		perror("listen");
		return 1;
	}

	printf("✅ Server listening on port %d\n", PORT);

	while (running) {
		addr_len = sizeof(client_addr);
		client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &addr_len);

		if (client_fd < 0) {
			if (running)
				perror("accept");
			continue;
		}

		pthread_mutex_lock(&clients_mutex);
		int found = 0;
		for (i = 0; i < MAX_CLIENTS; i++) {
			if (clients[i].fd < 0) {
				clients[i].fd = client_fd;
				clients[i].authenticated = 0;
				clients[i].addr = client_addr;
				found = 1;
				break;
			}
		}
		pthread_mutex_unlock(&clients_mutex);

		if (found) {
			pthread_create(&thread, NULL, client_handler, &clients[i]);
			pthread_detach(thread);
		} else {
			close(client_fd);
		}
	}

	printf("🛑 Shutting down...\n");
	close(listen_fd);
	return 0;
}

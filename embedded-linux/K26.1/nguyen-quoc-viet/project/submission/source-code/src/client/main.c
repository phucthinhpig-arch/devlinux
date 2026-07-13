#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/types.h>
#include "colors.h"

void display_message(const char *line);

#define SERVER_HOST "127.0.0.1"
#define SERVER_PORT 5000
#define BUFFER_SIZE 4096
#define MAX_USERNAME 64
#define MAX_PASSWORD 256

volatile int running = 1;

void signal_handler(int sig)
{
	(void)sig;
	running = 0;
}

void show_banner(void)
{
	printf("\n");
	printf(COLOR_BRIGHT_CYAN);
	printf("╔═══════════════════════════════════════╗\n");
	printf("║" COLOR_BOLD "   💬 DevLinux Chat Client v1.0   " COLOR_RESET COLOR_BRIGHT_CYAN "║\n");
	printf("╚═══════════════════════════════════════╝\n");
	printf(COLOR_RESET);
	printf(COLOR_CYAN "🌐 Server: %s:%d" COLOR_RESET "\n\n", SERVER_HOST, SERVER_PORT);
}

void show_help(void)
{
	printf("\n" COLOR_BRIGHT_BLUE "╭─ Available Commands " COLOR_RESET "\n");
	printf(COLOR_BRIGHT_BLUE "│\n");
	printf(COLOR_BRIGHT_BLUE "│ " COLOR_GREEN "/who" COLOR_RESET "              → " "List online users\n");
	printf(COLOR_BRIGHT_BLUE "│ " COLOR_GREEN "/all-users" COLOR_RESET "        → " "List all registered users\n");
	printf(COLOR_BRIGHT_BLUE "│ " COLOR_GREEN "/help" COLOR_RESET "             → " "Show this help\n");
	printf(COLOR_BRIGHT_BLUE "│ " COLOR_GREEN "/quit" COLOR_RESET "             → " "Exit chat\n");
	printf(COLOR_BRIGHT_BLUE "│ " COLOR_YELLOW "message text" COLOR_RESET "     → " "Send message to all users\n");
	printf(COLOR_BRIGHT_BLUE "╰" COLOR_RESET "\n\n");
}

int connect_to_server(void)
{
	int sock;
	struct sockaddr_in server_addr;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("socket");
		return -1;
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);

	if (inet_pton(AF_INET, SERVER_HOST, &server_addr.sin_addr) <= 0) {
		perror("inet_pton");
		close(sock);
		return -1;
	}

	if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		perror("connect");
		close(sock);
		return -1;
	}

	return sock;
}

int handle_login(int sock, char *username, char *password)
{
	char buf[BUFFER_SIZE];
	int len;
	fd_set readfds;
	struct timeval tv;

	snprintf(buf, sizeof(buf), "LOGIN:%s:%s\n", username, password);
	send(sock, buf, strlen(buf), 0);

	FD_ZERO(&readfds);
	FD_SET(sock, &readfds);
	tv.tv_sec = 3;
	tv.tv_usec = 0;

	if (select(sock + 1, &readfds, NULL, NULL, &tv) > 0) {
		memset(buf, 0, sizeof(buf));
		len = recv(sock, buf, sizeof(buf) - 1, 0);
		if (len > 0) {
			buf[len] = '\0';

			char *pos = buf;
			char *newline;
			int success = 0;

			while ((newline = strchr(pos, '\n')) != NULL) {
				*newline = '\0';
				if (strncmp(pos, "OK:LOGIN", 8) == 0) {
					success = 1;
					pos = newline + 1;
					continue;
				}
				display_message(pos);
				pos = newline + 1;
			}

			if (success) {
				printf(COLOR_GREEN "✅ Login successful!" COLOR_RESET "\n\n");
				return 1;
			}
		}
	}

	printf(COLOR_RED "❌ Login failed: Invalid username or password." COLOR_RESET "\n");
	return 0;
}

int handle_register(int sock, char *username, char *password)
{
	char buf[BUFFER_SIZE];
	int len;
	fd_set readfds;
	struct timeval tv;

	snprintf(buf, sizeof(buf), "REGISTER:%s:%s\n", username, password);
	send(sock, buf, strlen(buf), 0);

	FD_ZERO(&readfds);
	FD_SET(sock, &readfds);
	tv.tv_sec = 3;
	tv.tv_usec = 0;

	if (select(sock + 1, &readfds, NULL, NULL, &tv) > 0) {
		memset(buf, 0, sizeof(buf));
		len = recv(sock, buf, sizeof(buf) - 1, 0);
		if (len > 0) {
			buf[len] = '\0';

			char *pos = buf;
			char *newline;
			int success = 0;

			while ((newline = strchr(pos, '\n')) != NULL) {
				*newline = '\0';
				if (strncmp(pos, "OK:REGISTER", 11) == 0) {
					success = 1;
					pos = newline + 1;
					continue;
				}
				display_message(pos);
				pos = newline + 1;
			}

			if (success) {
				printf(COLOR_GREEN "✅ Registration successful!" COLOR_RESET "\n\n");
				return 1;
			}
		}
	}

	printf(COLOR_RED "❌ Registration failed: Username may already exist or server error." COLOR_RESET "\n");
	return 0;
}

void display_message(const char *line)
{
	if (strncmp(line, "HISTORY:", 8) == 0) {
		char *colon = strchr(line + 8, ':');
		if (colon) {
			char user[64];
			sscanf(line, "HISTORY:%63[^:]", user);
			printf(COLOR_BRIGHT_BLACK "📜 [%s]: " COLOR_RESET "%s\n", user, colon + 1);
		}
	} else if (strncmp(line, "FROM:", 5) == 0) {
		char *colon = strchr(line + 5, ':');
		if (colon) {
			char user[64];
			sscanf(line, "FROM:%63[^:]", user);
			printf(COLOR_BRIGHT_GREEN "💬 %s:" COLOR_RESET " %s\n", user, colon + 1);
		}
	} else if (strncmp(line, "USERS:", 6) == 0) {
		printf(COLOR_BRIGHT_YELLOW "👥 Online Users:" COLOR_RESET " %s\n", line + 6);
	} else if (strncmp(line, "ALLUSERS:", 9) == 0) {
		printf("\n" COLOR_BRIGHT_CYAN "📋 Registered Users" COLOR_RESET "\n");
		printf(COLOR_BRIGHT_CYAN "─────────────────────────────────────────" COLOR_RESET "\n");
	} else if (strchr(line, '|') && strchr(line, ':')) {
		printf(COLOR_DIM "  %s" COLOR_RESET "\n", line);
	} else if (strncmp(line, "ERR:", 4) == 0) {
		printf(COLOR_RED "❌ Error: %s" COLOR_RESET "\n", line + 4);
	} else if (strncmp(line, "OK:", 3) == 0) {
		printf(COLOR_GREEN "✅ %s" COLOR_RESET "\n", line + 3);
	} else {
		printf(COLOR_BRIGHT_BLACK "🔔 %s" COLOR_RESET "\n", line);
	}
}

void chat_loop(int sock, const char *username)
{
	char input_line[BUFFER_SIZE];
	char recv_buf[BUFFER_SIZE];
	int recv_len;
	fd_set readfds;

	printf(COLOR_BRIGHT_GREEN "✨ Connected as " COLOR_BOLD "%s" COLOR_RESET COLOR_BRIGHT_GREEN " ✨" COLOR_RESET "\n", username);
	show_help();
	printf("\n" COLOR_BRIGHT_BLUE "❯" COLOR_RESET " ");
	fflush(stdout);

	while (running) {
		FD_ZERO(&readfds);
		FD_SET(sock, &readfds);
		FD_SET(STDIN_FILENO, &readfds);

		struct timeval tv;
		tv.tv_sec = 1;
		tv.tv_usec = 0;

		int ret = select(sock + 1, &readfds, NULL, NULL, &tv);

		if (ret < 0) {
			if (errno != EINTR)
				perror("select");
			break;
		}

		if (FD_ISSET(sock, &readfds)) {
			memset(recv_buf, 0, sizeof(recv_buf));
			recv_len = recv(sock, recv_buf, sizeof(recv_buf) - 1, 0);

			if (recv_len <= 0) {
				printf("\n[!] Connection lost. Exiting.\n");
				break;
			}

			recv_buf[recv_len] = '\0';

			printf("\n");

			char *pos = recv_buf;
			char *newline;
			while ((newline = strchr(pos, '\n')) != NULL) {
				*newline = '\0';
				display_message(pos);
				pos = newline + 1;
			}

			printf(COLOR_BRIGHT_BLUE "❯" COLOR_RESET " ");
			fflush(stdout);
		}

		if (FD_ISSET(STDIN_FILENO, &readfds)) {
			if (fgets(input_line, sizeof(input_line), stdin) == NULL) {
				break;
			}

			input_line[strcspn(input_line, "\n")] = '\0';

			if (strlen(input_line) == 0) {
				printf(COLOR_BRIGHT_BLUE "❯" COLOR_RESET " ");
				fflush(stdout);
				continue;
			}

			if (strncmp(input_line, "/quit", 5) == 0) {
				char buf[BUFFER_SIZE];
				snprintf(buf, sizeof(buf), "LOGOUT\n");
				send(sock, buf, strlen(buf), 0);
				printf("[!] Logged out.\n");
				break;
			} else if (strncmp(input_line, "/who", 4) == 0) {
				char buf[BUFFER_SIZE];
				snprintf(buf, sizeof(buf), "USERLIST\n");
				send(sock, buf, strlen(buf), 0);
			} else if (strncmp(input_line, "/all-users", 10) == 0) {
				char buf[BUFFER_SIZE];
				snprintf(buf, sizeof(buf), "ALLUSERS\n");
				send(sock, buf, strlen(buf), 0);
			} else if (strncmp(input_line, "/help", 5) == 0) {
				show_help();
				printf(COLOR_BRIGHT_BLUE "❯" COLOR_RESET " ");
				fflush(stdout);
				continue;
			} else {
				char buf[BUFFER_SIZE];
				int maxlen = (int)sizeof(buf) - 6;
				snprintf(buf, sizeof(buf), "MSG:%.*s\n",
					maxlen > 0 ? maxlen : 0, input_line);
				send(sock, buf, strlen(buf), 0);
			}

			printf(COLOR_BRIGHT_BLUE "❯" COLOR_RESET " ");
			fflush(stdout);
		}
	}
}

int main(int argc, char *argv[])
{
	int sock;
	char username[MAX_USERNAME];
	char password[MAX_PASSWORD];
	char choice[16];

	(void)argc;
	(void)argv;

	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

	show_banner();

	sock = connect_to_server();
	if (sock < 0) {
		fprintf(stderr, "[!] Failed to connect to %s:%d\n", SERVER_HOST, SERVER_PORT);
		return EXIT_FAILURE;
	}

	while (1) {
		printf("Login or Register? [login/register]: ");
		fgets(choice, sizeof(choice), stdin);
		choice[strcspn(choice, "\n")] = '\0';

		printf("Username: ");
		fgets(username, sizeof(username), stdin);
		username[strcspn(username, "\n")] = '\0';

		printf("Password: ");
		fgets(password, sizeof(password), stdin);
		password[strcspn(password, "\n")] = '\0';

		int success = 0;
		if (strcmp(choice, "register") == 0) {
			success = handle_register(sock, username, password);
		} else {
			success = handle_login(sock, username, password);
		}

		if (success) {
			break;
		}

		printf("\nTry again? [yes/no]: ");
		char retry[16];
		fgets(retry, sizeof(retry), stdin);
		retry[strcspn(retry, "\n")] = '\0';

		if (strcmp(retry, "no") == 0 || strcmp(retry, "n") == 0) {
			close(sock);
			printf("Goodbye!\n");
			return EXIT_SUCCESS;
		}
		printf("\n");
	}

	fcntl(sock, F_SETFL, O_NONBLOCK);
	chat_loop(sock, username);

	close(sock);
	printf("\nGoodbye!\n");
	return EXIT_SUCCESS;
}

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/file.h>
#include "chat.h"

#define ACCOUNTS_FILE "accounts.db"

extern client_t clients[MAX_CLIENTS];
extern pthread_mutex_t clients_mutex;

static int is_user_online(const char *username)
{
	for (int i = 0; i < MAX_CLIENTS; i++) {
		if (clients[i].fd >= 0 && clients[i].authenticated &&
		    strcmp(clients[i].username, username) == 0) {
			return i;
		}
	}
	return -1;
}

void send_all_users_list(int client_fd)
{
	char buf[8192];
	int offset = 0;
	FILE *fp;
	char line[512];
	char username[64], hash[65];

	offset += snprintf(buf + offset, sizeof(buf) - offset, "ALLUSERS:\n");

	fp = fopen(ACCOUNTS_FILE, "r");
	if (!fp) {
		snprintf(buf + offset, sizeof(buf) - offset, "[no registered users]\n");
		send(client_fd, buf, strlen(buf), 0);
		return;
	}

	flock(fileno(fp), LOCK_SH);

	while (fgets(line, sizeof(line), fp)) {
		line[strcspn(line, "\n")] = 0;

		if (sscanf(line, "%63[^:]:%64[^:]", username, hash) == 2) {
			pthread_mutex_lock(&clients_mutex);
			int idx = is_user_online(username);
			pthread_mutex_unlock(&clients_mutex);

			char status[32] = "offline";
			char ip_info[128] = "";

			if (idx >= 0) {
				snprintf(status, sizeof(status), "online");
				char ip_str[INET_ADDRSTRLEN];
				inet_ntop(AF_INET, &clients[idx].addr.sin_addr, ip_str, INET_ADDRSTRLEN);
				snprintf(ip_info, sizeof(ip_info), " | %s:%d", ip_str,
					ntohs(clients[idx].addr.sin_port));
			}

			int len = snprintf(buf + offset, sizeof(buf) - offset,
				"%s | %s%s\n",
				username, status, ip_info);

			if (len < 0 || offset + (size_t)len >= sizeof(buf)) {
				break;
			}
			offset += len;
		}
	}

	flock(fileno(fp), LOCK_UN);
	fclose(fp);

	send(client_fd, buf, strlen(buf), 0);
}

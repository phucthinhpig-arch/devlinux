#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/file.h>
#include "chat.h"

#define ACCOUNTS_FILE "accounts.db"

extern client_t clients[MAX_CLIENTS];
extern pthread_mutex_t clients_mutex;

void print_user_status(void)
{
	FILE *fp;
	char line[512];
	char username[64], hash[65];

	printf("\n");
	printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ Current Users ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");

	fp = fopen(ACCOUNTS_FILE, "r");
	if (!fp) {
		printf("[no registered users]\n");
		printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n");
		return;
	}

	flock(fileno(fp), LOCK_SH);

	while (fgets(line, sizeof(line), fp)) {
		line[strcspn(line, "\n")] = 0;

		if (sscanf(line, "%63[^:]:%64[^:]", username, hash) == 2) {
			pthread_mutex_lock(&clients_mutex);

			char status_color[32] = "\x1b[31m";
			char status_text[12] = "✗ offline";
			char ip_info[128] = "";

			for (int i = 0; i < MAX_CLIENTS; i++) {
				if (clients[i].fd >= 0 && clients[i].authenticated &&
				    strcmp(clients[i].username, username) == 0) {
					snprintf(status_color, sizeof(status_color), "\x1b[32m");
					snprintf(status_text, sizeof(status_text), "✓ online");
					char ip_str[INET_ADDRSTRLEN];
					inet_ntop(AF_INET, &clients[i].addr.sin_addr, ip_str, INET_ADDRSTRLEN);
					snprintf(ip_info, sizeof(ip_info), "| %s:%d", ip_str,
						ntohs(clients[i].addr.sin_port));
					break;
				}
			}

			pthread_mutex_unlock(&clients_mutex);

			printf("%-12s| %s%-13s\x1b[0m%s\n", username, status_color, status_text, ip_info);
		}
	}

	flock(fileno(fp), LOCK_UN);
	fclose(fp);

	printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n");
}

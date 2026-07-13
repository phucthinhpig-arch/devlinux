#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <pthread.h>
#include "chat.h"

extern client_t clients[MAX_CLIENTS];
extern pthread_mutex_t clients_mutex;
extern void save_message_log(const char *username, const char *text);

void broadcast_message(const char *username, const char *text)
{
	int i;
	char buf[512];

	snprintf(buf, sizeof(buf), "FROM:%s:%s\n", username, text);

	pthread_mutex_lock(&clients_mutex);
	for (i = 0; i < MAX_CLIENTS; i++) {
		if (clients[i].fd >= 0 && clients[i].authenticated) {
			send(clients[i].fd, buf, strlen(buf), 0);
		}
	}
	pthread_mutex_unlock(&clients_mutex);

	save_message_log(username, text);
}

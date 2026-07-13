#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define MESSAGES_LOG "messages.log"
#define MAX_MESSAGE 256

void save_message_log(const char *username, const char *text)
{
	FILE *fp;
	time_t now = time(NULL);

	fp = fopen(MESSAGES_LOG, "a");
	if (!fp)
		return;

	flock(fileno(fp), LOCK_EX);

	fprintf(fp, "%s:%ld:%s\n", username, now, text);
	fflush(fp);

	flock(fileno(fp), LOCK_UN);
	fclose(fp);
}

void send_message_history(int client_fd, const char *username)
{
	FILE *fp;
	char line[512];
	char user[64];
	long timestamp;
	char message[MAX_MESSAGE];

	(void)username;

	fp = fopen(MESSAGES_LOG, "r");
	if (!fp)
		return;

	flock(fileno(fp), LOCK_SH);

	while (fgets(line, sizeof(line), fp)) {
		line[strcspn(line, "\n")] = 0;

		if (sscanf(line, "%63[^:]:%ld:%255[^\n]", user, &timestamp, message) == 3) {
			char buf[512];
			snprintf(buf, sizeof(buf), "HISTORY:%s:%s\n", user, message);
			send(client_fd, buf, strlen(buf), 0);
		}
	}

	flock(fileno(fp), LOCK_UN);
	fclose(fp);
}

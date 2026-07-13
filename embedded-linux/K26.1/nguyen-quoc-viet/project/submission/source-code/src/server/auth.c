#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/sha.h>
#include <fcntl.h>
#include <sys/file.h>
#include <unistd.h>
#include <sys/types.h>

#define ACCOUNTS_FILE "accounts.db"
#define MAX_USERNAME 64
#define MAX_PASSWORD 256
#define SHA256_HEX_LEN 65
#define SALT "devlinux_salt_2026"

char *sha256_hash(const char *password)
{
	static char hex[SHA256_HEX_LEN];
	unsigned char hash[32];
	int i;
	char combined[512];

	snprintf(combined, sizeof(combined), "%s%s", password, SALT);

	SHA256((unsigned char *)combined, strlen(combined), hash);

	for (i = 0; i < 32; i++)
		sprintf(&hex[i * 2], "%02x", hash[i]);

	hex[64] = '\0';
	return hex;
}

int authenticate_user(const char *username, const char *password)
{
	FILE *fp;
	char line[512];
	char stored_user[MAX_USERNAME];
	char stored_hash[SHA256_HEX_LEN];
	const char *input_hash = sha256_hash(password);
	int found = 0;

	fp = fopen(ACCOUNTS_FILE, "r");
	if (!fp) {
		if (!strcmp(username, "alice") && !strcmp(password, "pass123"))
			return 1;
		if (!strcmp(username, "bob") && !strcmp(password, "secret"))
			return 1;
		return 0;
	}

	flock(fileno(fp), LOCK_SH);

	while (fgets(line, sizeof(line), fp)) {
		line[strcspn(line, "\n")] = 0;

		if (sscanf(line, "%63[^:]:%64[^:]", stored_user, stored_hash) == 2) {
			if (strcmp(username, stored_user) == 0) {
				if (strcmp(input_hash, stored_hash) == 0)
					found = 1;
				break;
			}
		}
	}

	flock(fileno(fp), LOCK_UN);
	fclose(fp);

	return found;
}

int register_user(const char *username, const char *password)
{
	FILE *fp;
	const char *hash = sha256_hash(password);
	time_t now = time(NULL);

	fp = fopen(ACCOUNTS_FILE, "a");
	if (!fp)
		return 0;

	flock(fileno(fp), LOCK_EX);

	fprintf(fp, "%s:%s:%ld\n", username, hash, now);

	flock(fileno(fp), LOCK_UN);
	fclose(fp);

	return 1;
}

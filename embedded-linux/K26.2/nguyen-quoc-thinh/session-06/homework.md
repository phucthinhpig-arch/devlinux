# Assignment — session-06
**Deadline: 2026-06-21 23:59:00**

---

## Exercise_1 [review-only]

### Problem Statement

Write a C program that runs as a background service and create a systemd unit file to manage it.

**C program (`monitor.c`):**
- Runs in an infinite loop
- Calls `setbuf(stdout, NULL)` at startup so log lines appear in the journal immediately
- Prints one log line to stdout every 1 second
- Handles the `SIGTERM` signal: when systemd stops the service, the program must print `"Service shutting down..."` and exit cleanly

**Unit file (`monitor.service`):**
- The service starts automatically on boot
- If the service crashes, systemd restarts it automatically after 5 seconds
- Restart attempts are limited to a maximum of 3 times within 30 seconds

After installing the service, carry out the following steps and record the output of each:
1. `systemctl status monitor` — confirm the service is running
2. `kill <PID>` — simulate a crash and observe systemd restarting the service automatically
3. `systemctl stop monitor` — stop the service properly and confirm the `"shutting down"` log appears in the journal

### Submission

```
Exercise_1/
├── monitor.c           (required)
└── monitor.service     (required)
```

---

## Exercise_2 [review-only]

### Problem Statement

Write a C program that generates logs at different severity levels, run it as a systemd service, let it crash automatically after 30 seconds, then use `journalctl` to analyse the logs and save the results.

**C program (`logger.c`):**
- Calls `setbuf(stdout, NULL)` and `setbuf(stderr, NULL)` at startup
- Every 2 seconds writes **3 log lines** to stderr using the sd-daemon log level prefix format: `<3>` for error, `<4>` for warning, `<6>` for info
- After 30 seconds, calls `abort()` to simulate a crash

```c
#define LOG_ERR     "<3>"
#define LOG_WARNING "<4>"
#define LOG_INFO    "<6>"

// Example usage:
fprintf(stderr, LOG_INFO    "Service running normally, cycle %d\n", cycle);
fprintf(stderr, LOG_WARNING "Memory usage high: %d%%\n", 80 + rand() % 15);
fprintf(stderr, LOG_ERR     "Failed to connect to database, retry %d\n", cycle);
```

**Unit file (`logger.service`):**
- Redirects both stdout and stderr to the journal
- Does **not** restart after a crash (so the crash is preserved for analysis)

**Log collection (`log.txt`):** After the service has run and crashed, run the following commands in order. Include a section header before each command so the output is easy to read:

```bash
echo "=== 1. Full service log ===" >> log.txt
journalctl -u logger.service >> log.txt

echo "=== 2. Warning level and above ===" >> log.txt
journalctl -u logger.service -p warning >> log.txt

echo "=== 3. Error level and above ===" >> log.txt
journalctl -u logger.service -p err >> log.txt

echo "=== 4. Log from the last 1 minute ===" >> log.txt
journalctl -u logger.service --since "1 minute ago" >> log.txt

echo "=== 5. Crash point ===" >> log.txt
journalctl -u logger.service -p err >> log.txt
```

### Submission

```
Exercise_2/
├── logger.c        (required)
├── logger.service  (required)
└── log.txt         (required — output from all 5 journalctl commands with section headers)
```

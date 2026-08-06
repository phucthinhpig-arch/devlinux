# Assignment — Session: 08
**Deadline: 2026-07-05 23:59:00**

---

## Exercise_1 — System Monitor Daemon + CLI Tool via Unix Domain Socket (Stream) [build]

### Problem Statement

In embedded Linux systems, daemons typically run in the background and expose their internal state through a **Unix Domain Socket**, allowing CLI tools to query them without requiring root privileges or network access. This exercise simulates exactly that pattern.

Write **two C programs** that communicate over a **Unix Domain Stream Socket**:

**monitor-daemon** (`server.c`) — runs in the background, acting as a system resource monitoring daemon:
- Create an `AF_UNIX`, `SOCK_STREAM` socket and bind it to `/tmp/monitor.sock`.
- Accept connections from the CLI tool (`accept()`), serving one client at a time.
- For each connected client, read commands and respond accordingly. Support two commands:
  - `"cpu"` → read `/proc/loadavg` and reply with `"load_avg=<load1>"`.
  - `"mem"` → read `/proc/meminfo` and reply with `"mem_total=<kB> mem_free=<kB>"`.
- Reply `"ERROR: unknown command"` for any unrecognized input.
- When the client closes the connection (`recv()` returns 0), go back to `accept()` and wait for the next client.
- Remove the socket file (`unlink`) on startup and when receiving `SIGINT`.

**monitor-cli** (`client.c`) — CLI tool to query the daemon:
- Create an `AF_UNIX`, `SOCK_STREAM` socket and connect to `/tmp/monitor.sock`.
- Read commands from stdin (`fgets`), send them to the daemon with `send()`, receive the response, and print it to stdout.
- Type `"quit"` to close the connection and exit.

Requirements:
- Check return values for all syscalls: `socket()`, `bind()`, `listen()`, `accept()`, `connect()`, `send()`, `recv()`.
- Use `struct sockaddr_un` to configure the socket address.
- Do not use `fork()` or threads.

### Design Hints

```c
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/monitor.sock"
#define BUFFER_SIZE 256

/* Configure Unix Domain Socket address */
struct sockaddr_un addr;
memset(&addr, 0, sizeof(addr));
addr.sun_family = AF_UNIX;
strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

/* Read mem_total and mem_free from /proc/meminfo */
char line[128];
FILE *f = fopen("/proc/meminfo", "r");
while (fgets(line, sizeof(line), f)) {
    /* look for "MemTotal:" and "MemFree:" lines */
}
fclose(f);
```

### Suggested Approach

```
[monitor-daemon]
1. Register SIGINT handler to unlink the socket file on exit
2. unlink(SOCKET_PATH)  ← remove stale socket if it exists
3. socket(AF_UNIX, SOCK_STREAM, 0)
4. bind() → listen()
5. Client loop:
   a. accept() → get client fd
   b. Command loop:
      - recv() command from client
      - If "cpu" : read /proc/loadavg  → send() result
      - If "mem" : read /proc/meminfo  → send() result
      - If recv() == 0: client disconnected → break inner loop
   c. close(client_fd) → back to accept()

[monitor-cli]
1. socket(AF_UNIX, SOCK_STREAM, 0)
2. connect()
3. Loop:
   a. fgets(stdin) → if "quit": break
   b. send() command to daemon
   c. recv() result → print to stdout
4. close()
```

### Expected Output

```
# Terminal 1 – daemon:
[Daemon] Listening on /tmp/monitor.sock...
[Daemon] Client connected.
[Daemon] CMD: cpu
[Daemon] CMD: mem
[Daemon] Client disconnected. Waiting for next client...

# Terminal 2 – CLI tool:
[monitor-cli] Connected to /tmp/monitor.sock
> cpu
load_avg=0.42
> mem
mem_total=8051320 kB mem_free=3124560 kB
> quit
```

### Submission

```
Exercise_1/
├── server.c
├── client.c
└── Makefile      (targets: all, clean — builds both binaries)
```

---

## Exercise_2 — Simulated Sensor Telemetry via UDP Internet Socket [build]

### Problem Statement

In IoT and embedded systems, sensors typically send data periodically to a log collector server over **UDP** — a lightweight, connectionless protocol well-suited for resource-constrained devices. This exercise simulates that pattern locally on a single machine.

Write **two C programs** that communicate over an **Internet Domain Datagram Socket (UDP)**:

**sensor** (`sender.c`) — simulates an embedded sensor device:
- Create an `AF_INET`, `SOCK_DGRAM` socket.
- Every 2 seconds, read system information and send a **datagram** to the collector containing:
  - Simulated CPU temperature: read load average from `/proc/loadavg` and derive a temperature value (e.g. `temp = 40.0 + load1 * 10`).
  - RAM usage: read `MemTotal` and `MemFree` from `/proc/meminfo` and compute `mem_used_percent`.
- Datagram format: `"id=sensor-01 temp=<C> mem_used=<percent>%"` (plain text string).
- Send exactly **5 datagrams** then exit.

**collector** (`receiver.c`) — simulates a server collecting device logs:
- Create an `AF_INET`, `SOCK_DGRAM` socket and bind to `0.0.0.0:9000`.
- Receive datagrams from the sensor and print each one to stdout with a timestamp and sender address:
  `[<HH:MM:SS>] <IP>:<port> → <message>`
- Run continuously until Ctrl+C is pressed.

Requirements:
- Use `struct sockaddr_in` to configure socket addresses.
- Use `sendto()` and `recvfrom()` — do **not** use `connect()`/`send()`/`recv()`.
- Enable `SO_REUSEADDR` on the collector to avoid "Address already in use" errors on restart.
- Generate timestamps using `time()` + `strftime()`, format `HH:MM:SS`.
- Check return values for all syscalls.

### Design Hints

```c
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define COLLECTOR_IP   "127.0.0.1"
#define COLLECTOR_PORT 9000
#define BUFFER_SIZE    128

/* Compute mem_used_percent from /proc/meminfo */
long mem_total = 0, mem_free = 0;
/* read MemTotal and MemFree, then: */
double mem_used_pct = (double)(mem_total - mem_free) / mem_total * 100.0;

/* Generate timestamp */
time_t now = time(NULL);
struct tm *t = localtime(&now);
char ts[16];
strftime(ts, sizeof(ts), "%H:%M:%S", t);
```

### Suggested Approach

```
[collector]
1. socket(AF_INET, SOCK_DGRAM, 0)
2. setsockopt(SO_REUSEADDR)
3. bind() to 0.0.0.0:9000
4. Loop: recvfrom() → receive data + sender address
         Get current timestamp
         Print "[HH:MM:SS] <IP>:<port> → <message>"

[sensor]
1. socket(AF_INET, SOCK_DGRAM, 0)
2. Configure collector address (127.0.0.1:9000)
3. Loop 5 times:
   a. Read /proc/loadavg → compute temp
   b. Read /proc/meminfo → compute mem_used_pct
   c. Build string "id=sensor-01 temp=X.X mem_used=Y.Y%"
   d. sendto() to collector
   e. Print "[Sent N/5] <message>"
   f. sleep(2)
4. close()
```

### Expected Output

```
# Terminal 1 – collector:
[Collector] Listening on 0.0.0.0:9000...
[10:22:01] 127.0.0.1:54321 → id=sensor-01 temp=44.2 mem_used=61.2%
[10:22:03] 127.0.0.1:54321 → id=sensor-01 temp=43.8 mem_used=61.5%
[10:22:05] 127.0.0.1:54321 → id=sensor-01 temp=44.5 mem_used=61.3%
[10:22:07] 127.0.0.1:54321 → id=sensor-01 temp=43.1 mem_used=61.1%
[10:22:09] 127.0.0.1:54321 → id=sensor-01 temp=44.0 mem_used=61.4%

# Terminal 2 – sensor:
[Sensor] Target collector: 127.0.0.1:9000
[Sent 1/5] id=sensor-01 temp=44.2 mem_used=61.2%
[Sent 2/5] id=sensor-01 temp=43.8 mem_used=61.5%
...
[Sent 5/5] id=sensor-01 temp=44.0 mem_used=61.4%
[Sensor] Done.
```

### Submission

```
Exercise_2/
├── sender.c
├── receiver.c
└── Makefile      (targets: all, clean — builds both binaries)
```

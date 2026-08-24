# Assignment — Session: 10
**Deadline: 2026-07-12 23:59:00**

---

## Exercise_1 — Multi-Client IoT Server using select() [build]

### Problem Statement

In embedded IoT systems, a single server often must handle **multiple concurrent clients** (e.g., temperature sensors, humidity sensors, relay controllers) without threads or async libraries. The `select()` system call provides an efficient, blocking multiplexing mechanism.

Write a **TCP server** (`iot_server.c`) that:

1. **Listens** on `localhost:9999` for incoming TCP connections.
2. **Uses `select()`** to monitor:
   - The listening socket (for new connections)
   - All connected client sockets (for incoming data)
3. **Handles client commands**:
   - `GET_TEMP`: Return simulated temperature: `25.0 + (rand() % 10)`
   - `GET_HUMIDITY`: Return simulated humidity: `30.0 + (rand() % 30)`
   - `SET_MODE <mode>`: Store mode (0=idle, 1=active, 2=alert); respond with `OK`
   - `QUIT`: Close the client connection
   - Unknown commands: Respond with `ERROR: Unknown command`
4. **Broadcasts** every 5 seconds:
   - A status line: `[SERVER_STATUS] Temp=25.5 Humidity=45.2 Mode=1 Clients=3`
   - To all connected clients via their sockets
5. **Exits cleanly** on `SIGINT`:
   - Close all client sockets
   - Close the listening socket
   - Print `[Server] Shutdown complete.`

Write a **simple client** (`iot_client.c`) for testing:
- Connect to `localhost:9999`
- Read commands from stdin and send to server
- Print server responses
- On EOF or `quit`, close and exit

Requirements:
- Use `select()` with `FD_SET`, `FD_CLR`, `FD_ISSET`, `FD_ZERO`.
- Handle `SIGINT` gracefully (set a flag, break loop, cleanup).
- Proper error checking for `select()`, `socket()`, `bind()`, `listen()`, `accept()`, `send()`, `recv()`.
- Buffer management: handle partial reads/writes.

### Design Hints

```c
/* iot_server.h */
#define SERVER_PORT    9999
#define MAX_CLIENTS    10
#define BROADCAST_INTERVAL 5  /* seconds */

typedef struct {
    int     fd;           /* socket file descriptor */
    int     mode;         /* 0=idle, 1=active, 2=alert */
    time_t  last_activity;
} client_t;
```

### Suggested Approach

```
[IoT Server]
1. Create a listening socket, bind to localhost:9999, listen()
2. Register SIGINT handler (set volatile sig_atomic_t flag)
3. Main loop:
   a. Initialize fd_set with listening socket
   b. For each connected client: add client->fd to fd_set
   c. Set timeout for select() = 1 second (to check broadcast timer)
   d. select(max_fd+1, &readfds, NULL, NULL, &timeout)
   e. If listening socket is ready: accept() new client, add to array
   f. For each client socket in readfds:
      - recv() command
      - Parse and execute (GET_TEMP, GET_HUMIDITY, SET_MODE, QUIT)
      - send() response
   g. Check if 5 seconds elapsed since last broadcast:
      - Format status: "Temp=X Humidity=Y Mode=Z Clients=N"
      - send() to all connected clients
   h. If SIGINT flag set: break loop and cleanup

[IoT Client]
1. socket() → connect() to localhost:9999
2. Loop:
   a. fgets() from stdin
   b. send() to server
   c. recv() response, print it
   d. If "quit" or EOF: break
3. close() socket
```

### Expected Output

```
# Terminal 1 – server:
[Server] Listening on localhost:9999
[Server] Client 1 connected from 127.0.0.1:54321
[Server] Broadcasting to 1 clients: Temp=27.3 Humidity=52.1 Mode=1 Clients=1
[Server] Client 1: GET_TEMP → 27.3
[Server] Broadcasting to 1 clients: Temp=28.1 Humidity=48.5 Mode=1 Clients=1
[Server] Client 2 connected from 127.0.0.1:54322
[Server] Client 1: SET_MODE 2 → OK
[Server] Client 2: GET_HUMIDITY → 48.5
[Server] Broadcasting to 2 clients: Temp=26.9 Humidity=50.3 Mode=2 Clients=2
...
^C
[Server] Shutdown complete.

# Terminal 2 – client 1:
$ ./iot_client
GET_TEMP
27.3
SET_MODE 2
OK
[BROADCAST] Temp=28.1 Humidity=48.5 Mode=1 Clients=1
GET_HUMIDITY
48.5
quit

# Terminal 3 – client 2:
$ ./iot_client
GET_HUMIDITY
48.5
[BROADCAST] Temp=26.9 Humidity=50.3 Mode=2 Clients=2
quit
```

### Submission

```
Exercise_1/
├── iot_server.c
├── iot_client.c
├── iot_server.h
└── Makefile      (targets: all, clean)
```

---

## Exercise_2 — System Event Monitor using poll() [build]

### Problem Statement

In embedded Linux systems, daemons must **monitor multiple event sources** simultaneously: pipes (inter-process communication), sockets, files, timers. The `poll()` syscall is a flexible, portable alternative to `select()`, especially when managing **many file descriptors**.

Write a **multi-source event monitor** (`event_monitor.c`) that:

1. **Opens and monitors** three event sources:
   - **Named pipe (FIFO)** at `/tmp/event_log`: Read log messages from external processes
   - **Unix Domain Socket** at `/tmp/event_control.sock`: Accept control commands (START, STOP, STATUS, EXIT)
   - **Regular file** at `/tmp/system_status`: Detect when the file changes size (simulates log appends)
2. **Uses `poll()`** to multiplex all three with a 2-second timeout.
3. **Processes events**:
   - **FIFO**: Read lines, print: `[FIFO_EVENT] <message>`, count total events
   - **Socket**: Accept connection, recv() command:
     - `STATUS`: Send back `Total events: N, Uptime: N seconds`
     - `START`: Resume monitoring (toggle active flag)
     - `STOP`: Pause monitoring (toggle active flag)
     - `EXIT`: Close connection
   - **File**: If size changed since last check, print: `[FILE_EVENT] /tmp/system_status size changed to N bytes`
4. **Timeout handler**: Every 2 seconds (on poll timeout), print: `[HEARTBEAT] Monitor alive, events_seen=N`
5. **Graceful shutdown** on `SIGTERM`:
   - Close FIFO, socket, file
   - Print `[Monitor] Shutdown complete.`

Write a **test client** (`event_client.c`) to send commands to the monitor's Unix socket.

Requirements:
- Use `poll()` with `pollfd` array.
- Create FIFO with `mkfifo()` if it doesn't exist.
- Create Unix socket listener with `AF_UNIX` domain.
- Track file size with `stat()`.
- Handle `SIGTERM` gracefully.
- Proper error checking for all syscalls.

### Design Hints

```c
/* event_monitor.h */
#define FIFO_PATH       "/tmp/event_log"
#define SOCKET_PATH     "/tmp/event_control.sock"
#define FILE_PATH       "/tmp/system_status"
#define POLL_TIMEOUT_MS 2000  /* 2 seconds */

typedef enum {
    FD_FIFO,
    FD_SOCKET_LISTENER,
    FD_FILE,
    NUM_POLL_FDS
} pollfd_index_t;
```

### Suggested Approach

```
[Event Monitor]
1. Setup signal handler for SIGTERM (set flag)
2. Create FIFO at /tmp/event_log (mkfifo, O_NONBLOCK)
3. Create Unix domain listening socket at /tmp/event_control.sock
4. Open /tmp/system_status for monitoring (stat to get initial size)
5. Initialize pollfd array:
   - pollfd[FD_FIFO].fd = fifo_fd, .events = POLLIN
   - pollfd[FD_SOCKET_LISTENER].fd = socket_fd, .events = POLLIN
   - pollfd[FD_FILE].fd = file_fd, .events = POLLERR (just track, no read)
6. Main loop:
   a. poll(pollfd, NUM_POLL_FDS, POLL_TIMEOUT_MS)
   b. On timeout: print heartbeat, increment timeout_count
   c. Check pollfd[FD_FIFO].revents & POLLIN:
      - read() line from FIFO
      - print "[FIFO_EVENT] <message>"
      - increment event counter
   d. Check pollfd[FD_SOCKET_LISTENER].revents & POLLIN:
      - accept() new connection
      - recv() command (STATUS, START, STOP, EXIT)
      - send() appropriate response
      - close() connection
   e. Check pollfd[FD_FILE].revents:
      - stat() file, compare size
      - If size changed: print "[FILE_EVENT] size changed to N"
      - increment event counter
   f. If SIGTERM flag set: break and cleanup

[Event Client]
1. socket(AF_UNIX) → connect() to /tmp/event_control.sock
2. Loop:
   a. fgets() command from stdin
   b. send() to socket
   c. recv() response, print it
   d. If "exit" or EOF: break
3. close() socket
```

### Expected Output

```
# Terminal 1 – monitor:
[Monitor] Listening on /tmp/event_control.sock
[Monitor] Monitoring /tmp/event_log (FIFO) and /tmp/system_status
[HEARTBEAT] Monitor alive, events_seen=0
[FIFO_EVENT] Sensor1: temperature=25.3C
[HEARTBEAT] Monitor alive, events_seen=1
[FIFO_EVENT] Sensor2: humidity=65.2%
[FILE_EVENT] /tmp/system_status size changed to 1024 bytes
[HEARTBEAT] Monitor alive, events_seen=3
...
^C
[Monitor] Shutdown complete.

# Terminal 2 – client:
$ ./event_client
STATUS
Total events: 3, Uptime: 15 seconds
START
OK
STOP
OK
EXIT

# Terminal 3 – external process (sends to FIFO):
$ echo "Sensor1: temperature=25.3C" >> /tmp/event_log
$ echo "Sensor2: humidity=65.2%" >> /tmp/event_log
```

### Submission

```
Exercise_2/
├── event_monitor.c
├── event_client.c
├── event_monitor.h
└── Makefile      (targets: all, clean)
```

---

## Summary

**Session 10** covers two multiplexing mechanisms fundamental to embedded systems:

- **select()** — Classic, portable approach; good for small fd counts (<=1024)
- **poll()** — Modern, flexible; better for many fds, no FD_SETSIZE limit

Both exercises are **real-world scenarios**:
1. **IoT Server** = typical multi-client server (sensors, controllers)
2. **Event Monitor** = typical daemon (log aggregation, event processing)

Master these patterns and you can build robust, efficient embedded daemons.

# Assignment — Session: 09
**Deadline: 2026-07-05 23:59:00**

---

## Exercise_1 — Sensor Monitor with SystemV Shared Memory [build]

### Problem Statement

In embedded systems, a daemon is typically responsible for collecting sensor data and writing it to shared memory so that multiple processes can read it without communicating directly with each other. This exercise simulates that model using **SystemV Shared Memory**.

Write **two** C programs:

**sensor-daemon** (`daemon.c`) — collects and writes sensor data to shared memory:
- Create a shared memory region using `shmget()` + `shmat()`, with a fixed key `0x1234`.
- Every 2 seconds, read system information and write it to shared memory:
  - Simulated CPU temperature: `temp = 40.0 + load1 * 10` (load1 read from `/proc/loadavg`).
  - RAM usage percentage: calculated from `MemTotal` and `MemFree` in `/proc/meminfo`.
  - Timestamp: Unix time in seconds (`time()`).
- On `SIGINT`: detach (`shmdt`), remove shared memory (`shmctl IPC_RMID`), then exit.

**sensor-cli** (`cli.c`) — reads and displays data from shared memory:
- Attach to shared memory with the same key `0x1234` using `shmget()` + `shmat()`.
- Read and print data to stdout once, then exit:
  ```
  [Sensor Report]
  Timestamp : 1717123456
  CPU Temp  : 44.20 C
  RAM Used  : 61.30 %
  ```
- If shared memory does not exist (daemon is not running), print an error and exit.

Requirements:
- Define the shared struct in a header file `sensor_shm.h`.
- Check errors for all syscalls: `shmget()`, `shmat()`, `shmdt()`, `shmctl()`.

### Design Hints

```c
/* sensor_shm.h */
#define SHM_KEY  0x1234

typedef struct {
    time_t  timestamp;
    double  cpu_temp;
    double  ram_used_pct;
} sensor_data_t;
```

### Suggested Approach

```
[sensor-daemon]
1. Register a SIGINT handler to clean up shared memory on exit
2. shmget(SHM_KEY, sizeof(sensor_data_t), IPC_CREAT | 0666)
3. shmat() → get a pointer to the shared memory region
4. Loop every 2 seconds:
   a. Read /proc/loadavg → compute cpu_temp
   b. Read /proc/meminfo → compute ram_used_pct
   c. Write struct to shared memory
   d. Print "[Daemon] Written: temp=X ram=Y%"
5. SIGINT handler: shmdt() → shmctl(IPC_RMID) → exit

[sensor-cli]
1. shmget(SHM_KEY, sizeof(sensor_data_t), 0666)  ← no IPC_CREAT
   If it fails: print "Daemon is not running." → exit(1)
2. shmat() → read struct
3. Print report to stdout
4. shmdt()
```

### Expected Output

```
# Terminal 1 – daemon:
[Daemon] Shared memory created. Key=0x1234
[Daemon] Written: temp=44.20 ram=61.30%
[Daemon] Written: temp=43.80 ram=61.50%
...
^C
[Daemon] Cleaning up shared memory. Goodbye.

# Terminal 2 – CLI (run at any time while the daemon is running):
[Sensor Report]
Timestamp : 1717123456
CPU Temp  : 44.20 C
RAM Used  : 61.30 %
```

### Submission

```
Exercise_1/
├── daemon.c
├── cli.c
├── sensor_shm.h
└── Makefile      (targets: all, clean — builds both binaries)
```

---

## Exercise_2 — Device Config Sharing with mmap [build]

### Problem Statement

In embedded Linux, device configuration (baud rate, sampling rate, log level, etc.) is often stored in a file and read by multiple processes. Instead of each process calling `fread()` on the file separately, using `mmap()` allows all processes to map the same physical memory region — when one process updates the config, others see the change immediately without re-reading the file. This exercise simulates that model.

Write **two** C programs:

**config-writer** (`writer.c`) — updates device configuration:
- Create or open the file `/tmp/device.cfg`, ensuring it is large enough to hold the config struct (`ftruncate()`).
- Map the file into memory using `mmap()` with `PROT_READ | PROT_WRITE`, `MAP_SHARED`.
- Display a menu for the user to change individual config fields:
  - `baud_rate`: choose from `{9600, 115200, 460800}`.
  - `sampling_rate_hz`: enter an integer (1–1000).
  - `log_level`: choose from `{0=OFF, 1=ERROR, 2=INFO, 3=DEBUG}`.
- After each change, write immediately through the mmap pointer (changes are reflected to the file right away).
- Enter `"quit"` to exit.

**config-reader** (`reader.c`) — reads and displays the current configuration:
- Open the file `/tmp/device.cfg`, map it into memory with `PROT_READ`, `MAP_SHARED`.
- Every 2 seconds, read config from the mmap region and print to stdout.
- Run continuously until Ctrl+C.

Requirements:
- Define the shared struct in `device_cfg.h`.
- Call `msync()` after writing to ensure data is flushed to the file.
- Check errors for `open()`, `ftruncate()`, `mmap()`, `munmap()`, `msync()`.
- Do not use `read()`/`write()` to access file content — use only the mmap pointer.

### Design Hints

```c
/* device_cfg.h */
typedef struct {
    int    baud_rate;
    int    sampling_rate_hz;
    int    log_level;        /* 0=OFF 1=ERROR 2=INFO 3=DEBUG */
} device_cfg_t;

/* mmap the file */
int fd = open("/tmp/device.cfg", O_RDWR | O_CREAT, 0666);
ftruncate(fd, sizeof(device_cfg_t));
device_cfg_t *cfg = mmap(NULL, sizeof(device_cfg_t),
                         PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
close(fd);  /* fd can be closed after a successful mmap */
```

### Suggested Approach

```
[config-writer]
1. open() + ftruncate() + mmap(PROT_READ|PROT_WRITE, MAP_SHARED)
2. close(fd)
3. Loop displaying menu:
   a. Print current config
   b. User selects a field and enters a new value
   c. Update directly via pointer: cfg->baud_rate = ...
   d. msync() to flush to file
   e. Print "[Updated] <field> = <value>"
   f. If "quit": break
4. munmap()

[config-reader]
1. open() + mmap(PROT_READ, MAP_SHARED)
2. close(fd)
3. Loop every 2 seconds:
   a. Read cfg->baud_rate, cfg->sampling_rate_hz, cfg->log_level
   b. Print to stdout
4. munmap()
```

### Expected Output

```
# Terminal 1 – reader (run first):
[Config Reader] Polling /tmp/device.cfg every 2s...
baud_rate=9600  sampling_rate=100 Hz  log_level=INFO
baud_rate=9600  sampling_rate=100 Hz  log_level=INFO
baud_rate=115200  sampling_rate=100 Hz  log_level=DEBUG   ← writer just changed it
baud_rate=115200  sampling_rate=500 Hz  log_level=DEBUG   ← writer just changed it
...

# Terminal 2 – writer:
[Config Writer] Loaded /tmp/device.cfg
Current: baud_rate=9600 sampling_rate=100 log_level=2

Select field to update [baud/rate/log/quit]: baud
Select baud rate [9600/115200/460800]: 115200
[Updated] baud_rate = 115200
```

### Submission

```
Exercise_2/
├── writer.c
├── reader.c
├── device_cfg.h
└── Makefile      (targets: all, clean — builds both binaries)
```

---

## Exercise_3 — Device On/Off Controller with Shared Memory + pthread_mutex [build]

### Problem Statement

In embedded systems, a controller process (HMI, remote control) typically writes commands to shared memory, while a device process continuously reads and executes those commands. Without synchronization, the device process may read data mid-write while the controller is still updating it — causing a race condition. This exercise uses a **pthread_mutex** placed directly inside shared memory to solve that problem.

Write **two** C programs:

**controller** (`controller.c`) — sends on/off commands to the device:
- Create shared memory using `shm_open()` + `mmap()`, named `/device_shm`.
- Initialize a `pthread_mutex` residing inside the shared memory struct with the `PTHREAD_PROCESS_SHARED` attribute.
- Display a menu for the user to enter commands:
  - `"on"` → write state `1` to shared memory.
  - `"off"` → write state `0` to shared memory.
  - `"quit"` → clean up and exit.
- For each write: `pthread_mutex_lock()` → update state → `pthread_mutex_unlock()`.
- On exit: `pthread_mutex_destroy()`, `munmap()`, `shm_unlink()`.

**device** (`device.c`) — reads commands and acts on them:
- Attach to shared memory `/device_shm` using `shm_open()` + `mmap()`.
- Every 1 second: `pthread_mutex_lock()` → read state → `pthread_mutex_unlock()` → print to stdout:
  - If state is `1`: `"[Device] Status: ON  — Running..."`
  - If state is `0`: `"[Device] Status: OFF — Idle."`
- Run continuously until Ctrl+C.
- On exit: `munmap()`.

Requirements:
- Use `shm_open()` + `mmap()` (do not use SystemV `shmget`).
- The mutex must reside **inside the struct** in shared memory — do not declare the mutex as a local variable in each process.
- Set the `PTHREAD_PROCESS_SHARED` attribute on the mutex before calling `pthread_mutex_init()`.
- Check errors for all syscalls.
- Link with `-lrt -lpthread` when compiling.

### Design Hints

```c
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>

#define SHM_NAME "/device_shm"

typedef struct {
    pthread_mutex_t mutex;   /* mutex lives inside shared memory */
    int             status;  /* 0 = OFF, 1 = ON */
} device_state_t;

/* controller: initialize mutex with PTHREAD_PROCESS_SHARED */
pthread_mutexattr_t attr;
pthread_mutexattr_init(&attr);
pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
pthread_mutex_init(&state->mutex, &attr);
pthread_mutexattr_destroy(&attr);

/* Both processes share the mutex through the mmap pointer */
pthread_mutex_lock(&state->mutex);
state->status = 1;
pthread_mutex_unlock(&state->mutex);
```

### Suggested Approach

```
[controller]  ← run first to create shared memory and initialize the mutex
1. shm_open(O_CREAT|O_RDWR) + ftruncate(sizeof(device_state_t)) + mmap
2. Initialize mutex with PTHREAD_PROCESS_SHARED
3. state->status = 0
4. Loop displaying menu:
   a. User enters "on" / "off" / "quit"
   b. pthread_mutex_lock() → update state->status → pthread_mutex_unlock()
   c. Print "[Controller] Command sent: ON/OFF"
5. pthread_mutex_destroy() → munmap() → shm_unlink()

[device]  ← run after controller
1. shm_open(O_RDWR) + mmap   ← no O_CREAT, mutex already exists in shm
2. Loop every 1 second:
   a. pthread_mutex_lock() → read state->status → pthread_mutex_unlock()
   b. Print corresponding status
3. SIGINT: munmap() → exit
```

### Expected Output

```
# Terminal 1 – device:
[Device] Attached to /device_shm
[Device] Status: OFF — Idle.
[Device] Status: OFF — Idle.
[Device] Status: ON  — Running...   ← controller just sent "on"
[Device] Status: ON  — Running...
[Device] Status: OFF — Idle.        ← controller just sent "off"
...

# Terminal 2 – controller:
[Controller] Shared memory ready. Commands: on / off / quit
> on
[Controller] Command sent: ON
> off
[Controller] Command sent: OFF
> quit
[Controller] Cleaning up. Goodbye.
```

### Submission

```
Exercise_3/
├── controller.c
├── device.c
└── Makefile      (targets: all, clean — builds both binaries, link -lrt -lpthread)
```

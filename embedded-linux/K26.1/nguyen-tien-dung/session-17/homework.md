# Assignment — session-XX
**Deadline: 2026-08-08 23:59:00**

---

## Exercise_1 [review-only]

### Problem Statement

Write a **kernel platform driver** that controls a single LED (GPIO output), using the **GPIO descriptor-based API** (`gpiod_*` / `devm_gpiod_get`) and reading its GPIO configuration from **Device Tree** (same approach as `gpio_test_driver.c` covered in class).

The driver must:

1. Register as a **platform driver**, matched against Device Tree via `of_match_table` (choose your own compatible string, e.g. `"<your-name>,led-blink"`).
2. In `probe()`:
   - Acquire the LED's GPIO descriptor with `devm_gpiod_get()`, configured as output, initial level LOW (LED off).
   - Allocate a device number, initialize the `cdev`, create the class, and create the device node `/dev/led_blink`.
3. Implement the following file operations:
   - `write()`: on receiving `'1'` → turn the LED on (set GPIO HIGH); on receiving `'0'` → turn the LED off (set GPIO LOW). Any other value → return `-EINVAL`.
   - `read()`: **required** — return the LED's current state (`'1'` = on, `'0'` = off) by reading the GPIO value directly via `gpiod_get_value()` (do NOT cache the last value written in a global variable), the same way `gpio_read()` was implemented in the sample driver. This lets user space (Exercise_2) query the LED's actual hardware state at any time, including right after opening the device — before any `write()` has ever happened.
4. In `remove()`: clean up in the reverse order of `probe()` (destroy device → destroy class → delete cdev → unregister device number). The GPIO descriptor was acquired with `devm_gpiod_get()`, so it does not need to be freed manually.
5. Log all important steps with `dev_info()` / `dev_err()` (acquiring the GPIO, creating the device, removing it, etc.).

> Do NOT use the legacy GPIO API (`gpio_request`, `gpio_direction_output`, ...) — the descriptor-based API, as used in the sample driver, is mandatory.

Along with the driver, submit a **Device Tree overlay** (`led-blink-overlay.dts`) declaring the matching node, using the same `compatible` string as in your driver's `of_match_table`. The node must include:

- `compatible`: matching the `of_device_id` in your driver (e.g. `"<your-name>,led-blink"`).
- `gpios`: the GPIO pin connected to the LED, using `GPIO_ACTIVE_HIGH` or `GPIO_ACTIVE_LOW` depending on your wiring (add a comment explaining why you chose that flag).
- `label`: a descriptive name for the device (e.g. `"led-blink"`).

The overlay only needs to compile correctly with `dtc` and match the driver — it does not need to be tested on real hardware.

### Design Hints (optional)

You can reuse almost the entire data structure and control flow of `gpio_test_driver.c`, just renaming the device/class for the LED, for example:

```c
#define DEVICE_NAME  "led_blink"
#define CLASS_NAME   "led_blink_class"

static struct gpio_desc *led_gpio;
static dev_t dev_num;
static struct cdev led_cdev;
static struct class *led_class;
```

### Suggested Approach (optional)

```
1. probe(): devm_gpiod_get() to get the GPIO from DT -> GPIOD_OUT_LOW
2. Allocate device number, cdev_init + cdev_add
3. class_create() + device_create() -> /dev/led_blink
4. write(): parse '0'/'1' -> gpiod_set_value()
5. read(): gpiod_get_value() -> return '0'/'1' to user space
6. remove(): clean up in reverse order of probe()
```

### Expected Output (optional)

```
$ echo 1 > /dev/led_blink     # LED turns on
$ cat /dev/led_blink
1
$ echo 0 > /dev/led_blink     # LED turns off
$ cat /dev/led_blink
0
```

Exit code: `0` on a valid operation; a negative value (errno) when writing an invalid character to the device.

### Submission

```
Exercise_1/
├── main.c                (required) - kernel driver
├── Makefile              (required — targets: all, clean)
├── led-blink.dts (required) - device tree file
└── *.h                   (if any)
```

---

## Exercise_2 [build]

### Problem Statement

Write a **user-space application** (`main.c`) that **blinks the LED with a 5-second period**, through the `/dev/led_blink` device created by the driver in Exercise_1.

The program must:

1. Open `/dev/led_blink` with `open()` (mode `O_RDWR` — read access is needed too, to query the LED's state, not just write access).
2. Right after opening the device, `read()` the current LED state and print it (so you know the LED's state before blinking starts, instead of assuming it's off by default).
3. Loop forever (or until a stop signal such as Ctrl+C is received):
   - `write()` `'1'` to turn the LED on, then `read()` it back to **confirm the actual state** (don't just assume the LED is on because the write succeeded), then `sleep(5)`.
   - `write()` `'0'` to turn the LED off, `read()` to confirm, then `sleep(5)`.
4. Print a log line on every state change, including the value read back from the driver (e.g. `[APP] LED ON (status=1)`, `[APP] LED OFF (status=0)`) along with a timestamp. If the value from `read()` doesn't match the value just written, print a warning (`[APP] WARNING: status mismatch`).
5. Check and handle errors for every system call (`open()`, `write()`, `read()`) — on error, print a clear message and exit with a non-zero status.
6. Close the file descriptor before the program exits (including when exiting due to a signal).

### Design Hints (optional)

```c
#define DEVICE_PATH "/dev/led_blink"
#define BLINK_PERIOD_SEC 5

int fd = open(DEVICE_PATH, O_RDWR);

char read_led_status(int fd) {
    char status;
    read(fd, &status, 1);   /* read back the actual state from the driver */
    return status;
}
```

Instead of using `sleep(5)` in the loop (which only measures relative time and can drift if other steps take extra time), you can use **`timerfd_create()`** to get a more accurate 5-second period, and one that's easier to extend later (e.g. combined with `select()`/`poll()` to wait on the timer while also listening for other input if needed):

```c
#include <sys/timerfd.h>

int tfd = timerfd_create(CLOCK_MONOTONIC, 0);

struct itimerspec its = {
    .it_value    = { .tv_sec = BLINK_PERIOD_SEC, .tv_nsec = 0 }, /* first expiration */
    .it_interval = { .tv_sec = BLINK_PERIOD_SEC, .tv_nsec = 0 }, /* repeat every 5s */
};
timerfd_settime(tfd, 0, &its, NULL);

uint64_t expirations;
while (1) {
    read(tfd, &expirations, sizeof(expirations)); /* blocks until the timer fires (every 5s) */
    /* toggle the LED here */
}
```

Using `alarm()` with a `SIGALRM` signal handler is another, simpler option, but `timerfd` is recommended since it's safer to combine with other `read()`/`write()` calls in the same loop.

### Suggested Approach (optional)

```
1. open("/dev/led_blink", O_RDWR) -> check fd >= 0
2. read(fd, &status, 1) -> log the initial LED state
3. timerfd_create(CLOCK_MONOTONIC, 0) -> timerfd_settime(interval = 5s)
4. (Optional) install a SIGINT handler for a clean exit
5. while (1):
     read(tfd, &expirations, sizeof(expirations))   // wait a full 5 seconds
     write(fd, "1"/"0", 1) -> read(fd, &status, 1) -> log "LED ON/OFF (status=...)"
     (flip the value to write '1'/'0' on the next iteration)
6. close(tfd), close(fd) before the program exits
```

### Expected Output (optional)

```
$ ./led_app
[APP] Initial LED status: 0
[APP] LED ON (status=1)
[APP] LED OFF (status=0)
[APP] LED ON (status=1)
[APP] LED OFF (status=0)
...
```

Exit code: `0` on normal exit; non-zero if `open()`/`write()` fails.

### Submission

```
Exercise_2/
├── main.c        (required) - user-space app
├── Makefile      (required — targets: all, clean)
└── *.h           (if any)
```

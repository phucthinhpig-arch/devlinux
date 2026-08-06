# Assignment — session-15
**Deadline: 2026-08-01 23:59:00**

---

## Exercise_1 [review-only]

### Problem Statement

Write a character device driver called **`counter_chardev`** that creates the device node `/dev/counter`, simulating a counter maintained in kernel space.

The driver must support:

- **read()**: return the current counter value as a string, e.g. `"5\n"`.
- **write()**: accept one of three text commands (case-insensitive):
  - `"inc"` → increment the counter by 1
  - `"dec"` → decrement the counter by 1, **the counter must never go below 0**
  - `"reset"` → set the counter back to 0
  - If the command is invalid → log an error via `printk` and return `-EINVAL`
- **open()** / **release()**: log via `printk` on every call (same as `hello_chardev`).

The driver must follow the same lifecycle you learned in `hello_chardev.c`:
`alloc_chrdev_region()` → `cdev_init()` + `cdev_add()` → `class_create()` → `device_create()`, and clean up in the reverse order inside `module_exit`.

> Note: this is an extension exercise built on top of `hello_chardev.c` and `temp_sensor.c` from class — **do not copy the sample driver as-is**; you must write your own read/write logic for the counter.

### Design Hints (optional)

```c
static int  counter_value = 0;
static char counter_buf[16];

/* Suggested command parsing inside write(): */
if (strncmp(cmd, "inc", 3) == 0) { ... }
else if (strncmp(cmd, "dec", 3) == 0) { ... }
else if (strncmp(cmd, "reset", 5) == 0) { ... }
else { return -EINVAL; }
```

### Suggested Approach (optional)

```
1. copy_from_user() the write data into a temporary buffer, null-terminate it
2. Parse the command string and update counter_value accordingly
3. printk the counter value after every change, for debugging
4. read(): snprintf counter_value into a buffer, copy_to_user like hello_chardev
5. Remember to reset *ppos after a write so the next read() returns the fresh value
```

### Expected Output (optional)

Test with `echo`/`cat` or your own test program (automatic build/run is not required):

```bash
$ echo "inc" | sudo tee /dev/counter
$ echo "inc" | sudo tee /dev/counter
$ cat /dev/counter
2
$ echo "dec" | sudo tee /dev/counter
$ cat /dev/counter
1
$ echo "reset" | sudo tee /dev/counter
$ cat /dev/counter
0
```

### Submission

```
Exercise_1/
└── counter_chardev.c    (required)
```

---

## Exercise_2 [review-only]

### Problem Statement

Write a character device driver called **`reverse_chardev`** that creates the device node `/dev/reverse`.

Behavior:

- **write()**: the user writes an arbitrary string (up to 255 characters) into the driver; the driver stores it in an internal buffer (the same way `hello_chardev` stores data in `kernel_buf`).
- **read()**: the driver returns the string with **its character order reversed**, not the original string.
  - Example: writing `"hello"` and then reading back must return `"olleh"`.
- If `read()` is called before any `write()` has happened, the driver returns 0 bytes (EOF), matching the default behavior of `hello_chardev` when the buffer is empty.
- The full init/exit lifecycle and class/device creation must be implemented (do not use the legacy `register_chrdev()`).

### Design Hints (optional)

```c
#define MAX_LEN 256

static char raw_buf[MAX_LEN];      /* string the user just wrote      */
static char reversed_buf[MAX_LEN]; /* reversed version of the string  */
static size_t data_len = 0;

/* You may reverse either in-place or into a separate buffer,
   inside read() or write() — your choice. */
```

### Suggested Approach (optional)

```
1. write(): copy_from_user into raw_buf, null-terminate it, store data_len
2. Reverse raw_buf into reversed_buf (write your own reverse function —
   strrev is not a standard glibc/kernel function, don't use it)
3. read(): copy_to_user from reversed_buf, handle *ppos like hello_chardev
   to support multiple/partial reads
4. printk both the original and the reversed string for easier debugging via dmesg
```

### Submission

```
Exercise_2/
└── reverse_chardev.c    (required)
```

---

## Exercise_3 [review-only]

### Problem Statement

Answer the following questions briefly (write your answers in `ANSWERS.md`, no code required), based on the drivers you wrote in Exercise_1 and Exercise_2, as well as the sample drivers `hello_chardev.c` / `temp_sensor.c` covered in class:

1. Why can't you copy a user-space pointer directly into `kernel_buf` — why must you use `copy_from_user()` / `copy_to_user()` instead? Give at least 2 technical reasons.
2. In your driver, what is the `*ppos` variable used for? If you forget to update `*ppos` inside `read()`, what happens when the user calls `read()` several times in a row?
3. Compare `alloc_chrdev_region()` with the legacy `register_chrdev()` — why should modern drivers prefer the former?
4. If two processes both open `/dev/counter` and call `write("inc")` at nearly the same time, could the counter value end up wrong (a race condition)? Why or why not? (No code changes needed, explanation only.)

### Submission

```
Exercise_3/
└── ANSWERS.md    (required)
```

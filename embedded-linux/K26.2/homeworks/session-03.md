# Assignment — session-03
**Deadline: 2026-08-16 23:59:00**

---

## Exercise_1 [build]

### Problem Statement

Write a C program that manages a list of students stored in a **binary file** `students.dat`. The program displays a menu in a loop and performs all file operations exclusively through Linux system calls — `fopen`, `fread`, `fwrite`, and `fprintf` are **not allowed**.

Required menu:

```
1. Add student
2. List all students
3. Find student by ID
4. Exit
```

Storage struct:

```c
typedef struct {
    int   id;
    char  name[64];
    int   age;
    float gpa;
} Student;
```

Behaviour of each option:

- **Add**: read fields from stdin, append one `Student` record to the end of the file
- **List**: read the entire file sequentially and print all fields of every record
- **Find**: scan records one by one; print the first record whose `id` matches; report not found otherwise
- **Exit**: close the file descriptor and terminate

Data must **persist between runs** — records added in one session must be readable in the next.

### Submission

```
Exercise_1/
├── main.c      (required)
└── Makefile    (required — targets: all, clean)
```

---

## Exercise_2 [build]

### Problem Statement

Write a C program that manages a list of products stored in a binary file `products.dat`. Every read or write targeting a specific record must use `lseek` to jump directly to the correct offset — **loading the entire file into memory is not acceptable**.

Required menu:

```
1. Add product
2. Show product by index
3. Update quantity by index
4. List all products
5. Exit
```

Storage struct:

```c
typedef struct {
    int    id;
    char   name[64];
    int    quantity;
    double price;
} Product;
```

Behaviour of each option:

- **Add**: append a new `Product` at the end of the file
- **Show by index**: `lseek` to `index × sizeof(Product)`, read and print one record
- **Update quantity**: `lseek` to the exact byte offset of the `quantity` field inside the target record, write only that field — do not rewrite the entire record
- **List all**: read and print every record sequentially

### Design Hints

```c
// Byte offset of the record at position [index]
off_t offset = (off_t)index * sizeof(Product);

// Byte offset of a specific field within that record
off_t field_offset = offset + offsetof(Product, quantity);
```

### Submission

```
Exercise_2/
├── main.c      (required)
└── Makefile    (required — targets: all, clean)
```

---

## Exercise_3 [build]

### Problem Statement

Build a multi-process logging system where several processes write to the **same log file** `system.log` concurrently. Implement and compare two locking strategies: `flock` and `fcntl`.

Every line written to the file must follow this format:

```
[PID:12345] [2025-05-21 14:02:33] [INFO] your message here
```

Build **two separate binaries**:

```bash
./writer_flock  "message text"
./writer_fcntl  "message text"
```

Each binary must: open the file with `O_WRONLY | O_APPEND | O_CREAT`, acquire an exclusive lock, format and write one log line, release the lock, close the file, and exit.

### Suggested Approach

**Part 1 — `flock`:**

```c
flock(fd, LOCK_EX);
// ... write log line ...
flock(fd, LOCK_UN);
```

**Part 2 — `fcntl` (blocking `F_SETLKW`):**

```c
struct flock fl = {
    .l_type   = F_WRLCK,
    .l_whence = SEEK_SET,
    .l_start  = 0,
    .l_len    = 0,
};
fcntl(fd, F_SETLKW, &fl);
// ... write log line ...
fl.l_type = F_UNLCK;
fcntl(fd, F_SETLK, &fl);
```

After completing both parts, add the following comparison table as a comment block at the top of each source file:

| Property | `flock` | `fcntl` |
|---|---|---|
| Lock granularity | Whole file only | Byte range supported |
| Works over NFS | No | Yes |
| Inherited across `fork` | Yes | No |
| Best used when | Simple local file locking | Network FS or byte-range locking |

### Submission

```
Exercise_3/
├── writer_flock.c      (required)
├── writer_fcntl.c      (required)
└── Makefile            (required — targets: all, test_flock, test_fcntl, clean)
```

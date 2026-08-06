# Assignment — session-04
**Deadline: 2026-08-16 23:59:00**

---

## Exercise_1 [build]

### Problem Statement

Write a single-file C program `manager.c` that simulates a multi-process order processing system. Each order is handled by a separate child process running concurrently; the parent process tracks the result of each child after it finishes.

Hardcode an array of **3 orders** in `main()`:

```c
typedef struct {
    int   id;
    char  name[50];
    int   quantity;
    float unit_price;
} Order;

Order orders[3] = {
    {1, "Backpack", 2, 350000},
    {2, "Shoes",    1, 500000},
    {3, "Hat",      3, 120000}
};
```

The execution flow requires **two separate loops**:

- **Loop 1**: call `fork()` 3 times, saving each child's PID into `pids[]`. The child calls `process_order()` then `exit(0)`.
- **Loop 2**: call `waitpid(pids[i], ...)` for each child; use `WIFEXITED` + `WEXITSTATUS` to inspect the result.

After both loops, the parent prints a **summary**: total orders and total revenue.

### Design Hints

```c
void process_order(Order o) {
    float total = o.quantity * o.unit_price;
    printf("[CHILD-%d] PID: %d | PPID: %d\n", o.id, getpid(), getppid());
    printf("[CHILD-%d] %s x%d — Total: %.0f VND\n",
           o.id, o.name, o.quantity, total);
    printf("[CHILD-%d] Processing... (sleep 2s)\n\n", o.id);
    sleep(2);
}
```

### Suggested Approach

```
Loop 1: i = 0..2
  fflush(stdout)        ← important: prevents the child from re-printing the parent's buffered output
  fork()
    [child]  → process_order(orders[i]) → exit(0)
    [parent] → pids[i] = pid

Loop 2: i = 0..2
  waitpid(pids[i], &status, 0)
  WIFEXITED + WEXITSTATUS → print result

Print summary
```

> Why two separate loops: if `fork()` and `waitpid()` are inside the same loop, the parent waits for each child before spawning the next one — destroying concurrency and making execution effectively sequential.

### Expected Output

```
$ ./manager

===================================================
   ORDER PROCESSING SYSTEM — MANAGER (fork+wait)
===================================================
[MANAGER] PID: 1021 — spawning 3 child processes...

[MANAGER] fork() order #1 → child PID: 1022
[MANAGER] fork() order #2 → child PID: 1023
[MANAGER] fork() order #3 → child PID: 1024
[MANAGER] All 3 children spawned. Starting waitpid()...

--- [child output order may interleave — this is normal] ---

[CHILD-1] PID: 1022 | PPID: 1021
[CHILD-1] Backpack x2 — Total: 700000 VND
[CHILD-1] Processing... (sleep 2s)

[CHILD-2] PID: 1023 | PPID: 1021
[CHILD-2] Shoes x1 — Total: 500000 VND
[CHILD-2] Processing... (sleep 2s)

[CHILD-3] PID: 1024 | PPID: 1021
[CHILD-3] Hat x3 — Total: 360000 VND
[CHILD-3] Processing... (sleep 2s)

--- [~2 seconds later, all 3 children call exit(0)] ---

[MANAGER] waitpid(1022) — order #1: exit code=0 → SUCCESS
[MANAGER] waitpid(1023) — order #2: exit code=0 → SUCCESS
[MANAGER] waitpid(1024) — order #3: exit code=0 → SUCCESS

================= SUMMARY =================
  Total orders    : 3
  Successful      : 3
  Failed          : 0
  Total revenue   : 1,560,000 VND
===========================================
```

The print order of the three children may vary on each run — this is concurrent execution, not a bug. The `waitpid()` section always prints in fixed order because the parent calls it with specific PIDs.

### Submission

```
Exercise_1/
├── manager.c   (required)
└── Makefile    (required — targets: all, clean)
```

---

## Exercise_2 [build]

### Problem Statement

Build a student record lookup system consisting of **two separate files**: `manager.c` and `searcher.c`. The manager reads a student ID from stdin, spawns a child process to perform the search via `fork()` + `execve()`, then reads the outcome from the child's exit code.

**`manager.c`** (parent process):
1. Loop reading a student ID from stdin (enter `"quit"` to exit).
2. For each ID: `fork()` → child calls `execve("./searcher")` → parent calls `waitpid()`.
3. Read the exit code via `WIFEXITED` + `WEXITSTATUS` and print the appropriate message.
4. The line immediately after `execve()` must be `perror()` + `exit(2)`, with a comment explaining why this line is normally never reached.

**`searcher.c`** (child process — launched by execve):
1. Receive `argv[1]` = student ID to find, `argv[2]` = path to the data file.
2. Open `students.txt`, read line by line, use `strtok(line, "|")` to split fields.
3. Found → print full record + grade classification → `exit(0)`.
4. Not found → print a message → `exit(1)`.
5. File or argument error → `perror()` → `exit(2)`.

Data file `students.txt` (pipe `|` as delimiter):

```
SV001|Nguyen Van An|KTPM01|8.5
SV002|Tran Thi Bich|KTPM01|7.2
SV003|Le Van Cuong|HTTT02|9.1
SV004|Pham Thi Dung|HTTT02|6.8
SV005|Hoang Van Em|KHMT03|5.0
```

Grade classification by GPA: ≥ 8.5 → Excellent · ≥ 7.0 → Good · ≥ 5.0 → Average · < 5.0 → Poor

`searcher` exit code contract:

| Exit code | Meaning |
|---|---|
| `0` | Student found — print full record |
| `1` | Student not found — print message |
| `2` | File or argument error — `perror()` |

### Suggested Approach

```
manager: read student ID from stdin
  fork()
    [child]  → execve("./searcher", {student_id, "students.txt", NULL}, environ)
               /* Only reached if execve() FAILS */
               perror("execve failed"); exit(2)
    [parent] → waitpid() → read exit code → print result
  repeat until user enters "quit"

searcher: launched by execve()
  print getpid(), getppid()
  open students.txt
  fgets() each line → strtok(line, "|") → compare field[0] with argv[1]
  match found → print record + grade → exit(0)
  end of file  → exit(1)
```

### Expected Output

```
$ ./manager

=============================================
   STUDENT LOOKUP SYSTEM — MANAGER
   (fork + execve | file: students.txt)
=============================================
[MANAGER] PID: 2040
Enter student ID ('quit' to exit).

---------------------------------------------
Student ID: SV003

[MANAGER] fork() → child PID: 2041
[MANAGER] Waiting for child (waitpid)...

[SEARCHER] PID: 2041 | PPID: 2040
[SEARCHER] Searching for "SV003" in students.txt...

========== SEARCH RESULT ==========
  ID      : SV003
  Name    : Le Van Cuong
  Class   : HTTT02
  GPA     : 9.1
  Grade   : Excellent
====================================

[MANAGER] Child (PID 2041) exited. code=0 → Found

---------------------------------------------
Student ID: SV999

[MANAGER] fork() → child PID: 2042
[MANAGER] Waiting for child (waitpid)...

[SEARCHER] PID: 2042 | PPID: 2040
[SEARCHER] Searching for "SV999" in students.txt...
[SEARCHER] No student found with ID: SV999

[MANAGER] Child (PID 2042) exited. code=1 → Not found

---------------------------------------------
Student ID: quit
[MANAGER] Exiting. Goodbye!
```

Note: child PID increments on every lookup — each search spawns a brand new process. The PPID of `searcher` is always the manager's PID because `execve()` replaces the image, not the process identity.

### Submission

```
Exercise_2/
├── manager.c       (required)
├── searcher.c      (required)
├── students.txt    (required)
└── Makefile        (required — targets: all, clean)
```

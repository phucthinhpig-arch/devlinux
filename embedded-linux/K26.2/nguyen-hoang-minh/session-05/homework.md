# Assignment — session-05
**Deadline: 2026-06-14 23:59:00**

---

## Exercise_1 [build]

### Problem Statement

Write a single-file C program `booking.c` that simulates a bus ticket booking system: **5 agents** (5 threads) simultaneously handle reservation requests for a trip with **10 available seats**. Each agent must check whether enough seats remain, deduct them, and confirm the booking — the check-and-deduct operation must be protected as a **single atomic critical section** by a mutex.

Hardcode an array of 5 requests in `main()`:

```c
typedef struct {
    int  agent_id;
    char customer[50];
    int  seats_wanted;
} BookingRequest;

BookingRequest requests[5] = {
    {1, "Nguyen Van An",  2},
    {2, "Tran Thi Bich",  1},
    {3, "Le Van Cuong",   3},
    {4, "Pham Thi Dung",  1},
    {5, "Hoang Van Em",   2}
};

int seats_available = 10;
pthread_mutex_t seat_lock;
```

After all threads finish, print a summary: total seats, seats sold, seats remaining, and failed bookings.

In a comment block, explain why the check and the deduct must be inside the **same** lock/unlock block — splitting them into two separate lock acquisitions is not correct.

### Suggested Approach

```
main()
  pthread_mutex_init(&seat_lock, NULL)
  pthread_create × 5  (pass &requests[i])
  pthread_join  × 5
  print summary
  pthread_mutex_destroy(&seat_lock)

book_ticket(arg)
  sleep(1)                           ← force real concurrency
  pthread_mutex_lock(&seat_lock)
    if seats_available >= wanted → deduct seats, print confirmation
    else                         → print "sold out"
  pthread_mutex_unlock(&seat_lock)
```

> Use `pthread_self()` to print each thread's TID so that interleaved output from different threads is visible.

### Expected Output

```
==============================================
   TICKET BOOKING SYSTEM (5 agents, 10 seats)
==============================================
[Agent 1 | TID 139...] Booking 2 seats for Nguyen Van An...
[Agent 3 | TID 139...] Booking 3 seats for Le Van Cuong...
[Agent 2 | TID 139...] Booking 1 seat  for Tran Thi Bich...
[Agent 4 | TID 139...] Booking 1 seat  for Pham Thi Dung...
[Agent 5 | TID 139...] Booking 2 seats for Hoang Van Em...

--- [all agents reach critical section after sleep(1)] ---

[Agent 3] CONFIRMED: 3 seats for Le Van Cuong.  Remaining: 7
[Agent 1] CONFIRMED: 2 seats for Nguyen Van An. Remaining: 5
[Agent 2] CONFIRMED: 1 seat  for Tran Thi Bich. Remaining: 4
[Agent 4] CONFIRMED: 1 seat  for Pham Thi Dung. Remaining: 3
[Agent 5] SOLD OUT:  needs 2 seats, only 3 left — booking failed.

================ SUMMARY ================
  Total seats     : 10
  Seats sold      : 7
  Seats remaining : 3
  Failed bookings : 1
=========================================
```

The order agents print their confirmations varies on each run — this is concurrent execution in action, not a bug.

### Submission

```
Exercise_1/
├── booking.c   (required)
└── Makefile    (required — targets: all, clean; compile flag: -pthread)
```

---

## Exercise_2 [build]

### Problem Statement

Write a single-file C program `print_queue.c` that simulates an office print queue: **3 producer threads** continuously submit documents to a shared queue; **1 printer thread** pulls and prints documents one at a time. The queue holds at most **5 documents**.

- Queue full → the producer must **wait** until a slot becomes available
- Queue empty → the printer must **wait** until a document is submitted

Each producer submits exactly 3 documents (9 total). The printer runs until all 9 have been printed.

```c
typedef struct {
    int  doc_id;
    char filename[60];
    int  pages;
} Document;

Document queue[5];
int head = 0, tail = 0, count = 0;
int all_sent = 0;           /* set to 1 by main after joining all producers */

pthread_mutex_t q_lock;
pthread_cond_t  not_full;   /* producers wait here when count == 5 */
pthread_cond_t  not_empty;  /* printer  waits here when count == 0 */
```

After joining all 3 producers, main sets `all_sent = 1` and calls `pthread_cond_broadcast(&not_empty)` to wake the printer, then joins the printer.

In a comment block, explain: why must `pthread_cond_wait()` be inside a `while` loop rather than an `if`? What is a **spurious wakeup**?

### Suggested Approach

```
producer(arg)
  repeat 3 times:
    pthread_mutex_lock(&q_lock)
    while (count == 5)
      pthread_cond_wait(&not_full, &q_lock)    ← sleep if queue is full
    enqueue(doc)
    pthread_cond_signal(&not_empty)            ← wake the printer
    pthread_mutex_unlock(&q_lock)

printer(arg)
  loop:
    pthread_mutex_lock(&q_lock)
    while (count == 0 && !all_sent)
      pthread_cond_wait(&not_empty, &q_lock)   ← sleep if queue is empty
    if (count == 0 && all_sent) → unlock → break
    dequeue(doc)
    pthread_cond_signal(&not_full)             ← wake a waiting producer
    pthread_mutex_unlock(&q_lock)
    sleep(1)    ← simulate printing time
```

### Expected Output

```
==============================================
   OFFICE PRINT QUEUE (3 producers, 1 printer)
   Queue capacity: 5 documents
==============================================

[Producer 1] Submitting: report_Q1.pdf (12 pages) — queue: 1/5
[Producer 2] Submitting: contract.pdf  (5 pages)  — queue: 2/5
[Producer 3] Submitting: invoice.pdf   (3 pages)  — queue: 3/5
[Producer 1] Submitting: slides.pdf    (20 pages) — queue: 4/5
[Producer 2] Submitting: memo.pdf      (2 pages)  — queue: 5/5
[Producer 3] Queue full — waiting...
[Printer]    Printing:   report_Q1.pdf (12 pages) — queue: 4/5
[Producer 3] Submitting: proposal.pdf  (8 pages)  — queue: 5/5
[Printer]    Printing:   contract.pdf  (5 pages)  — queue: 4/5
[Producer 1] Submitting: summary.pdf   (4 pages)  — queue: 5/5
[Printer]    Printing:   invoice.pdf   (3 pages)  — queue: 4/5
[Producer 2] Submitting: budget.pdf    (7 pages)  — queue: 5/5
[Printer]    Printing:   slides.pdf    (20 pages) — queue: 4/5
[Printer]    Printing:   memo.pdf      (2 pages)  — queue: 3/5
[Printer]    Printing:   proposal.pdf  (8 pages)  — queue: 2/5
[Printer]    Printing:   summary.pdf   (4 pages)  — queue: 1/5
[Printer]    Printing:   budget.pdf    (7 pages)  — queue: 0/5
[Printer]    All documents printed. Exiting.

================ SUMMARY ================
  Documents submitted : 9
  Documents printed   : 9
  Total pages printed : 66
=========================================
```

The interleaving of producers and printer varies on each run. The queue count shown is always in `[0, 5]` — the condition variables enforce the bound.

### Submission

```
Exercise_2/
├── print_queue.c   (required)
└── Makefile        (required — targets: all, clean; compile flag: -pthread)
```

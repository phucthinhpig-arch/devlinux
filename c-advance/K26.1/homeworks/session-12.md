> **📣 Message from your instructor:**
>
> Hi folks,
>
> This advanced C programming course recommends young engineers to code on your own!
> If possible, let's first try to write code from scratch. If it's hard, you guys can
> ask AI coding tool assistant! Don't let AI agent generate code for you!!
>
> Happy coding geeks! 🚀

---

# Assignment — Session 12: Applied Data Structures (Linked Lists & Ring Buffers)

**Deadline: 2026-09-05 23:59:00**

> [!NOTE]
> **Build System Change:** This homework uses **CMake** instead of Makefile, and must include **Unity unit tests**. This reinforces what you learned in Lecture 9 (CMake & TDD).
>
> **How to build & test:**
>
> ```bash
> cmake -S . -B build && cmake --build build    # Build
> ./build/<executable_name>                      # Run main program
> ctest --test-dir build -V                      # Run unit tests
> ```

---

## Exercise_1 [build]

### Problem Statement

**Sorted Sensor Event List with Bounded Capacity & Node Deletion**

**Scenario:**
In embedded telemetry and logging systems, sensor events arrive asynchronously and out of order. Before transmitting or logging them, they must be sorted by timestamp in ascending order.

> 💡 **Educational Focus — Traditional (Non-Intrusive) Linked List:**
> In this exercise, you implement a **traditional singly-linked list** where the data payload (`timestamp`, `sensor_id`, `reading`) and the next pointer (`p_next`) are tightly coupled inside a single struct (`event_node_t`).
>
> - **Limitation:** Notice that this list cannot be reused for other data types (e.g., motor events or CAN packets) without rewriting the entire list code. Furthermore, each node requires an individual heap allocation (`malloc`).
> - In **Exercise 2**, you will learn how the industry solves this problem by separating the data payload from the list node using **Intrusive Linked Lists**.

However, embedded systems have finite RAM. To prevent unbounded heap allocations or memory exhaustion (the "medical monitor bug"), the list must maintain a **bounded maximum capacity** (e.g., `MAX_EVENTS = 5`). If adding a new event would exceed capacity, the oldest event must be automatically removed. Furthermore, the system must support explicit node deletion by Sensor ID when an event is acknowledged.

**Requirements:**

1. Define an `event_node_t` struct with `timestamp` (`uint32_t`), `sensor_id` (`uint8_t`), `reading` (`int16_t`), and `p_next` pointer.
2. Implement `event_node_t* create_event_node(uint32_t timestamp, uint8_t sensor_id, int16_t reading)` to allocate and initialize a new node (Single Responsibility Principle).
3. Implement `bool sorted_insert(event_node_t **pp_head, event_node_t *p_new, uint32_t max_capacity)`:
   - Inserts `p_new` in ascending timestamp order.
   - Handles all insertion locations: head (smallest timestamp), middle, or tail (largest timestamp).
   - If insertion causes list length to exceed `max_capacity`, automatically evict and `free()` the oldest node (smallest timestamp) from the head.
4. Implement `bool delete_event_by_id(event_node_t **pp_head, uint8_t sensor_id)`:
   - Finds the first node matching `sensor_id`, unlinks it from the chain, and calls `free()`.
   - Handles edge cases: deleting head, deleting middle/tail, deleting from single-node list, target ID not found, and empty list.
5. Implement `void print_events(const event_node_t *p_head)` to traverse and format the list.
6. Implement `void free_list(event_node_t **pp_head)` to safely free all nodes, avoiding use-after-free bugs by saving `p_next` before freeing.
7. In `main()`:
   - Insert 6 events with out-of-order timestamps with `max_capacity = 5` to demonstrate automatic eviction.
   - Delete an event in the middle by its ID.
   - Cleanly free the entire list at shutdown.

**Edge Cases & Warnings for Juniors:**

> [!WARNING]
>
> - **Use-After-Free in Traversal Deletion:** When deleting or freeing a node, _always_ save `p_curr->p_next` before calling `free(p_curr)`. Accessing `p_curr->p_next` after `free` is undefined behavior.
> - **Double Pointer Updates:** You must use `event_node_t **pp_head` so you can modify the caller's actual head pointer when inserting or deleting at the head.
> - **Empty List Checks:** Always check `if (pp_head == NULL || *pp_head == NULL)` before dereferencing.

**Rules:**

- Follow BARR-C coding style (fixed-width integers, mandatory braces, pointer naming with `p_` prefix).
- **Code Documentation:** All functions and data structures MUST be fully documented using Doxygen-style comments.
- Use `cppcheck` and `clang-tidy` to analyze, and make sure there are no warning or error messages.
- Build with CMake. Strict compiler flags (`-Wall -Wextra -pedantic -Werror -std=c11`) must be applied via `target_compile_options()`.
- Include Unity unit tests covering edge cases (see Expected Unit Test Output).

### Coding Standards Reference

**MISRA-C 2012 (Safety):**
| Rule | Category | Relevance to This Exercise |
|---|---|---|
| Rule 21.3 | Required | The memory allocation and deallocation functions of `<stdlib.h>` shall not be used → Understand the trade-off. Dynamic allocation is used here for educational list manipulation; bounded capacity prevents heap exhaustion. |

**CERT-C 2016 (Security):**
| Rule | Relevance to This Exercise |
|---|---|
| MEM31-C | Free dynamically allocated memory when no longer needed → Ensure `free_list()` and node deletion clean up without memory leaks or dangling pointers. |
| EXP34-C | Do not dereference null pointers → Verify `pp_head` and `*pp_head` before accessing list members. |

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs (under `C_Books/`) and read the full description of each rule above. After writing your code, verify your implementation follows these rules.

### Design Hints (optional)

**Source structure:**

```
Exercise_1/
├── CMakeLists.txt
├── src/
│   ├── main.c
│   └── sorted_list.c
├── include/
│   └── sorted_list.h
└── test/
    └── test_sorted_list.c
```

**Example function prototypes (`sorted_list.h`):**

```c
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct event_node {
    uint32_t timestamp;
    uint8_t  sensor_id;
    int16_t  reading;
    struct event_node *p_next;
} event_node_t;

event_node_t* create_event_node(uint32_t timestamp, uint8_t sensor_id, int16_t reading);
bool sorted_insert(event_node_t **pp_head, event_node_t *p_new, uint32_t max_capacity);
bool delete_event_by_id(event_node_t **pp_head, uint8_t sensor_id);
uint32_t get_list_length(const event_node_t *p_head);
void print_events(const event_node_t *p_head);
void free_list(event_node_t **pp_head);
```

**CMakeLists.txt hint:**

```cmake
cmake_minimum_required(VERSION 3.14)
project(exercise_1 C)

add_compile_options(-Wall -Wextra -pedantic -Werror -std=c11)

# Main executable
add_executable(exercise_1 src/main.c src/sorted_list.c)
target_include_directories(exercise_1 PRIVATE include)

# Unity test framework
include(FetchContent)
FetchContent_Declare(unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG        v2.6.0
)
FetchContent_MakeAvailable(unity)

# Test executable
enable_testing()
add_executable(test_exercise_1 test/test_sorted_list.c src/sorted_list.c)
target_include_directories(test_exercise_1 PRIVATE include)
target_link_libraries(test_exercise_1 PRIVATE unity)
add_test(NAME test_exercise_1 COMMAND test_exercise_1)
```

### Acceptance Criteria (Scoring)

- **[10%]** CMake project builds successfully (`cmake -S . -B build && cmake --build build`).
- **[10%]** All Unity tests pass (`ctest --test-dir build -V`).
- **[15%]** Code passes static analysis (`cppcheck`, `clang-tidy`).
- **[15%]** Code contains required Doxygen documentation.
- **[50%]** Logic correctly performs sorted insertion, properly bounds list capacity by evicting the oldest node, handles single-node and multi-node deletions by ID without leaks or use-after-free, and frees all memory.

### Expected Output

When run via `./build/exercise_1`:

```
========== L12 Lab 1: Sorted Sensor Event List ==========

[1] Inserting 6 events (out-of-order) with max_capacity = 5:
    Inserted: [t=100, ID=2, Val=250]
    Inserted: [t=50,  ID=1, Val=240]
    Inserted: [t=200, ID=3, Val=260]
    Inserted: [t=25,  ID=4, Val=210]
    Inserted: [t=150, ID=5, Val=220]
    Inserted: [t=300, ID=6, Val=280] -> Capacity exceeded! Evicted oldest [t=25].

--- Current Sorted Event List (Length: 5) ---
[t=50]  Sensor ID: 1, Reading: 240
[t=100] Sensor ID: 2, Reading: 250
[t=150] Sensor ID: 5, Reading: 220
[t=200] Sensor ID: 3, Reading: 260
[t=300] Sensor ID: 6, Reading: 280

[2] Deleting event with Sensor ID = 5 (middle node)...
    Deleted successfully.

--- Event List After Deletion (Length: 4) ---
[t=50]  Sensor ID: 1, Reading: 240
[t=100] Sensor ID: 2, Reading: 250
[t=200] Sensor ID: 3, Reading: 260
[t=300] Sensor ID: 6, Reading: 280

[3] Freeing entire list... done.
```

Exit code: `0` on success, non-zero on error.

### Expected Unit Test Output

When run via `ctest --test-dir build -V`:

```
test_sorted_list.c:XX:test_sorted_insert_maintains_order:PASS
test_sorted_list.c:XX:test_sorted_insert_enforces_capacity:PASS
test_sorted_list.c:XX:test_delete_head_node:PASS
test_sorted_list.c:XX:test_delete_middle_node:PASS
test_sorted_list.c:XX:test_delete_tail_node:PASS
test_sorted_list.c:XX:test_delete_non_existent_id:PASS
test_sorted_list.c:XX:test_free_list_nulls_head:PASS

-----------------------
7 Tests 0 Failures 0 Ignored
OK
```

### Submission

```
Exercise_1/
├── CMakeLists.txt               (required)
├── src/
│   ├── main.c                   (required)
│   └── sorted_list.c            (required)
├── include/
│   └── sorted_list.h            (required)
└── test/
    └── test_sorted_list.c       (required — Unity unit tests)
```

---

## Exercise_2 [build]

### Problem Statement

**Intrusive Linked List & Deferred Task Queue (`container_of`)**

**Scenario:**
In production RTOS kernels and embedded frameworks (such as **Zephyr RTOS** `sys_slist`, **FreeRTOS** `xList`, and the **Linux Kernel** `list_head`), standard linked lists are rarely used because they require separate metadata nodes and heap allocations. Instead, the industry standard is the **Intrusive Linked List**, where a lightweight node structure (`slist_node_t`) is embedded directly inside the application's payload struct.

The magical macro `CONTAINER_OF` uses compiler offset calculation (`offsetof`) to retrieve a pointer to the parent struct from a pointer to its embedded node, with **zero runtime overhead and zero dynamic memory allocation**.

**Real-World Application:**
You are building an **Interrupt Deferred Work Queue** for an embedded sensor hub. When high-frequency hardware interrupts fire (e.g., Timer, I2C, BLE packet ready), they cannot execute heavy processing in interrupt context. Instead, ISRs place pre-allocated task structs onto a deferred execution queue. The main loop pops tasks and processes them.

**Requirements:**

1. Define the intrusive list node:
   ```c
   typedef struct slist_node {
       struct slist_node *p_next;
   } slist_node_t;
   ```
2. Implement the `CONTAINER_OF` macro using `offsetof` from `<stddef.h>`:
   ```c
   #define CONTAINER_OF(ptr, type, member) \
       ((type *)((char *)(ptr) - offsetof(type, member)))
   ```
3. Define the `deferred_task_t` payload struct:

   ```c
   typedef void (*task_callback_t)(uint32_t arg);

   typedef struct {
       uint32_t        task_id;
       uint8_t         priority;
       task_callback_t callback;
       uint32_t        arg;
       slist_node_t    node;   /* Embedded intrusive list node */
   } deferred_task_t;
   ```

4. Implement generic singly-linked intrusive list operations:
   - `void slist_push(slist_node_t **pp_head, slist_node_t *p_node);` (O(1) LIFO stack push)
   - `slist_node_t* slist_pop(slist_node_t **pp_head);` (O(1) LIFO stack pop)
   - `void slist_append(slist_node_t **pp_head, slist_node_t *p_node);` (O(N) FIFO queue append)
   - `bool slist_remove(slist_node_t **pp_head, slist_node_t *p_target);` (Remove specific node)
5. Implement a dispatcher function:
   - `void dispatch_deferred_tasks(slist_node_t **pp_head);` that pops all tasks, recovers the `deferred_task_t` pointer via `CONTAINER_OF`, executes their callbacks, and prints task info.
6. In `main()`:
   - Statically declare an array of 4 `deferred_task_t` tasks (no `malloc`!).
   - Push tasks to the queue, remove one task explicitly with `slist_remove()`, and dispatch all remaining tasks.

**Edge Cases & Warnings for Juniors:**

> [!WARNING]
>
> - **Pointer Arithmetic Safety:** When implementing `CONTAINER_OF`, always cast `ptr` to `(char *)` before subtracting the offset. Subtracting byte offsets from `void*` is a GNU extension (not ISO C standard), and subtracting from typed pointers divides by `sizeof(type)`.
> - **Cycle Detection:** Never insert an intrusive node that is already present in a list without removing it first; doing so creates an infinite loop in traversal.
> - **Pop on Empty Queue:** `slist_pop()` on an empty list (`*pp_head == NULL`) must safely return `NULL` without crashing.

**Rules:**

- Follow BARR-C coding style.
- **Code Documentation:** All functions and data structures MUST be fully documented using Doxygen-style comments.
- Use `cppcheck` and `clang-tidy` to analyze, and make sure there are no warning or error messages.
- Build with CMake. Strict compiler flags (`-Wall -Wextra -pedantic -Werror -std=c11`) must be applied via `target_compile_options()`.
- Include Unity unit tests (see Expected Unit Test Output).

### Coding Standards Reference

**MISRA-C 2012 (Safety):**
| Rule | Category | Relevance to This Exercise |
|---|---|---|
| Rule 11.4 | Advisory | A conversion should not be performed between a pointer to object and an integer type → `offsetof` internally yields `size_t`, pointer arithmetic with `(char *)` must maintain alignment. |
| Rule 11.8 | Required | A conversion shall not remove any `const` or `volatile` qualification from the type pointed to by a pointer. |

**CERT-C 2016 (Security):**
| Rule | Relevance to This Exercise |
|---|---|
| EXP36-C | Do not cast pointers to types with stricter alignment requirements → Because `deferred_task_t` contains pointers and `uint32_t`, ensure recovered addresses match alignment. |
| EXP34-C | Do not dereference null pointers → Verify `p_node` and `pp_head` before operations. |

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs (under `C_Books/`) and read the full description of each rule above. After writing your code, verify your implementation follows these rules.

### Design Hints (optional)

**Source structure:**

```
Exercise_2/
├── CMakeLists.txt
├── src/
│   ├── main.c
│   └── intrusive_list.c
├── include/
│   └── intrusive_list.h
└── test/
    └── test_intrusive_list.c
```

**Example function prototypes (`intrusive_list.h`):**

```c
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define CONTAINER_OF(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

typedef struct slist_node {
    struct slist_node *p_next;
} slist_node_t;

typedef void (*task_callback_t)(uint32_t arg);

typedef struct {
    uint32_t        task_id;
    uint8_t         priority;
    task_callback_t callback;
    uint32_t        arg;
    slist_node_t    node;
} deferred_task_t;

void slist_push(slist_node_t **pp_head, slist_node_t *p_node);
slist_node_t* slist_pop(slist_node_t **pp_head);
void slist_append(slist_node_t **pp_head, slist_node_t *p_node);
bool slist_remove(slist_node_t **pp_head, slist_node_t *p_target);
void dispatch_deferred_tasks(slist_node_t **pp_head);
```

**CMakeLists.txt hint:**

```cmake
cmake_minimum_required(VERSION 3.14)
project(exercise_2 C)

add_compile_options(-Wall -Wextra -pedantic -Werror -std=c11)

add_executable(exercise_2 src/main.c src/intrusive_list.c)
target_include_directories(exercise_2 PRIVATE include)

# Unity
include(FetchContent)
FetchContent_Declare(unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG        v2.6.0
)
FetchContent_MakeAvailable(unity)

enable_testing()
add_executable(test_exercise_2 test/test_intrusive_list.c src/intrusive_list.c)
target_include_directories(test_exercise_2 PRIVATE include)
target_link_libraries(test_exercise_2 PRIVATE unity)
add_test(NAME test_exercise_2 COMMAND test_exercise_2)
```

### Acceptance Criteria (Scoring)

- **[10%]** CMake project builds successfully (`cmake -S . -B build && cmake --build build`).
- **[10%]** All Unity tests pass (`ctest --test-dir build -V`).
- **[15%]** Code passes static analysis (`cppcheck`, `clang-tidy`).
- **[15%]** Code contains required Doxygen documentation.
- **[50%]** Logic correctly implements intrusive list manipulation, correctly utilizes `CONTAINER_OF` without memory corruption or segmentation faults, handles node removal/popping properly, and requires zero dynamic allocation.

### Expected Output

When run via `./build/exercise_2`:

```
========== L12 Lab 2: Intrusive List & Deferred Task Queue ==========

[1] Initializing static tasks (zero heap allocation):
    Task 1: ID=101, Pri=1 (Read Battery ADC)
    Task 2: ID=102, Pri=3 (Process BLE Packet)
    Task 3: ID=103, Pri=2 (Flush Flash Buffer)
    Task 4: ID=104, Pri=4 (Emergency Temperature Cutoff)

[2] Pushing tasks onto Deferred Work Stack...
[3] Removing Task 103 from list using slist_remove()... OK

[4] Dispatching remaining tasks (LIFO execution order):
    --> Executing Task ID: 104 [Pri: 4] | Callback arg: 0xDEAD
    --> Executing Task ID: 102 [Pri: 3] | Callback arg: 0xFACE
    --> Executing Task ID: 101 [Pri: 1] | Callback arg: 0x1234

All deferred tasks dispatched successfully. Queue is empty.
```

Exit code: `0` on success, non-zero on error.

### Expected Unit Test Output

When run via `ctest --test-dir build -V`:

```
test_intrusive_list.c:XX:test_container_of_macro_offset:PASS
test_intrusive_list.c:XX:test_slist_push_and_pop_lifo:PASS
test_intrusive_list.c:XX:test_slist_append_fifo:PASS
test_intrusive_list.c:XX:test_slist_remove_head:PASS
test_intrusive_list.c:XX:test_slist_remove_middle:PASS
test_intrusive_list.c:XX:test_slist_pop_empty_returns_null:PASS

-----------------------
6 Tests 0 Failures 0 Ignored
OK
```

### Submission

```
Exercise_2/
├── CMakeLists.txt               (required)
├── src/
│   ├── main.c                   (required)
│   └── intrusive_list.c         (required)
├── include/
│   └── intrusive_list.h         (required)
└── test/
    └── test_intrusive_list.c    (required — Unity unit tests)
```

---

## Exercise_3 [build]

### Problem Statement

**SPSC Lock-Free Ring Buffer (Waste-One-Slot Strategy)**

**Scenario:**
In embedded systems, continuous data streams (like UART RX characters arriving via an ISR) must be passed to a background processing task in the main loop.
If we use a shared `count` variable to track the buffer level, both the ISR and the main loop will perform concurrent read-modify-write operations on `count`, causing fatal race conditions unless interrupts are disabled.

To achieve **zero latency, deterministic O(1) execution, and true lock-free SPSC (Single-Producer Single-Consumer) operation**, we eliminate the `count` variable and adopt the industry-standard **Waste-One-Slot** strategy:

- The **Producer (ISR)** only modifies `head` and only reads `tail`.
- The **Consumer (Task)** only modifies `tail` and only reads `head`.
- There are no shared write variables, making the pipeline lock-free!

**Requirements:**

1. Define a `ring_buf_lf_t` struct with a fixed power-of-2 size `RB_CAPACITY` (e.g. 8), `buffer` array, and `volatile uint32_t head` and `volatile uint32_t tail` indices.
   _(Note: Do NOT include a `count` variable!)_
2. Implement lock-free operations:
   - `void rb_init(ring_buf_lf_t *p_rb);`
   - `bool rb_is_empty(const ring_buf_lf_t *p_rb);` -> returns `true` when `head == tail`.
   - `bool rb_is_full(const ring_buf_lf_t *p_rb);` -> returns `true` when `((head + 1) & RB_MASK) == tail`.
   - `bool rb_push(ring_buf_lf_t *p_rb, int16_t data);` -> writes at `head`, updates `head = (head + 1) & RB_MASK`. Returns `false` if full without overwriting.
   - `bool rb_pop(ring_buf_lf_t *p_rb, int16_t *p_data);` -> reads from `tail`, updates `tail = (tail + 1) & RB_MASK`. Returns `false` if empty.
   - `bool rb_peek(const ring_buf_lf_t *p_rb, int16_t *p_data);` -> reads from `tail` without advancing `tail`.
   - `uint32_t rb_available(const ring_buf_lf_t *p_rb);` -> computes unread elements using `(head - tail) & RB_MASK`.
3. In `main()`:
   - Demonstrate that for `RB_CAPACITY = 8`, the maximum usable capacity is `7` items (1 slot is preserved empty).
   - Push 7 items until full. Attempt an 8th push and verify rejection.
   - Pop 3 items, push 3 more (demonstrating wrap-around index manipulation), and pop all remaining items.

**Edge Cases & Warnings for Juniors:**

> [!WARNING]
>
> - **Usable Capacity is `CAPACITY - 1`:** Juniors often assume an 8-element waste-one-slot buffer holds 8 items. If you fill all 8 slots, `head == tail`, which is indistinguishable from empty! Therefore, exactly 1 slot must remain empty when full.
> - **Power-of-2 Rule:** The bitwise mask `& RB_MASK` (where `RB_MASK = CAPACITY - 1`) **only works if `RB_CAPACITY` is a power of 2** (e.g., 4, 8, 16, 256). If `CAPACITY` is 10, bitwise AND produces catastrophic out-of-bounds memory accesses!
> - **`volatile` Keyword:** `head` and `tail` MUST be qualified with `volatile`. Without `volatile`, optimizing compilers will cache index values in CPU registers, failing to detect changes made by concurrent ISRs.

**Rules:**

- Follow BARR-C coding style.
- **Code Documentation:** All functions and data structures MUST be fully documented using Doxygen-style comments.
- Use `cppcheck` and `clang-tidy` to analyze, and make sure there are no warning or error messages.
- Build with CMake. Strict compiler flags (`-Wall -Wextra -pedantic -Werror -std=c11`) must be applied via `target_compile_options()`.
- Include Unity unit tests (see Expected Unit Test Output).

### Coding Standards Reference

**MISRA-C 2012 (Safety):**
| Rule | Category | Relevance to This Exercise |
|---|---|---|
| Rule 10.4 | Required | Both operands of an operator in which the usual arithmetic conversions are performed shall have the same essential type category → ensure bitwise operators for wrap-around use compatible unsigned integers (`uint32_t`). |

**CERT-C 2016 (Security):**
| Rule | Relevance to This Exercise |
|---|---|
| INT30-C | Ensure that unsigned integer operations do not wrap unexpectedly → verify index calculations correctly use power-of-2 bitwise masking. |
| CON32-C | Prevent data races when accessing shared objects → SPSC waste-one-slot pattern decouples producer and consumer write variables. |

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs (under `C_Books/`) and read the full description of each rule above. After writing your code, verify your implementation follows these rules.

### Design Hints (optional)

**Source structure:**

```
Exercise_3/
├── CMakeLists.txt
├── src/
│   ├── main.c
│   └── ring_buffer_lf.c
├── include/
│   └── ring_buffer_lf.h
└── test/
    └── test_ring_buffer_lf.c
```

**Example function prototypes (`ring_buffer_lf.h`):**

```c
#include <stdint.h>
#include <stdbool.h>

#define RB_CAPACITY 8U
#define RB_MASK     (RB_CAPACITY - 1U)

typedef struct {
    int16_t           buffer[RB_CAPACITY];
    volatile uint32_t head;
    volatile uint32_t tail;
} ring_buf_lf_t;

void rb_init(ring_buf_lf_t *p_rb);
bool rb_is_empty(const ring_buf_lf_t *p_rb);
bool rb_is_full(const ring_buf_lf_t *p_rb);
bool rb_push(ring_buf_lf_t *p_rb, int16_t data);
bool rb_pop(ring_buf_lf_t *p_rb, int16_t *p_data);
bool rb_peek(const ring_buf_lf_t *p_rb, int16_t *p_data);
uint32_t rb_available(const ring_buf_lf_t *p_rb);
```

**CMakeLists.txt hint:**

```cmake
cmake_minimum_required(VERSION 3.14)
project(exercise_3 C)

add_compile_options(-Wall -Wextra -pedantic -Werror -std=c11)

add_executable(exercise_3 src/main.c src/ring_buffer_lf.c)
target_include_directories(exercise_3 PRIVATE include)

# Unity
include(FetchContent)
FetchContent_Declare(unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG        v2.6.0
)
FetchContent_MakeAvailable(unity)

enable_testing()
add_executable(test_exercise_3 test/test_ring_buffer_lf.c src/ring_buffer_lf.c)
target_include_directories(test_exercise_3 PRIVATE include)
target_link_libraries(test_exercise_3 PRIVATE unity)
add_test(NAME test_exercise_3 COMMAND test_exercise_3)
```

### Acceptance Criteria (Scoring)

- **[10%]** CMake project builds successfully (`cmake -S . -B build && cmake --build build`).
- **[10%]** All Unity tests pass (`ctest --test-dir build -V`).
- **[15%]** Code passes static analysis (`cppcheck`, `clang-tidy`).
- **[15%]** Code contains required Doxygen documentation.
- **[50%]** Logic strictly adheres to the lock-free SPSC waste-one-slot design (no `count` member), correctly distinguishes full from empty, prevents overwriting on full buffer, handles bitwise wrap-around, and passes all edge case unit tests.

### Expected Output

When run via `./build/exercise_3`:

```
========== L12 Lab 3: SPSC Lock-Free Ring Buffer ==========

[!] Initializing Ring Buffer (Capacity: 8, Max Usable: 7)...
    State: Empty=TRUE, Full=FALSE, Available=0

[!] Pushing 7 items until buffer is full:
    Push 10 -> OK (Available: 1)
    Push 20 -> OK (Available: 2)
    Push 30 -> OK (Available: 3)
    Push 40 -> OK (Available: 4)
    Push 50 -> OK (Available: 5)
    Push 60 -> OK (Available: 6)
    Push 70 -> OK (Available: 7) [FULL]

[!] Attempting 8th push (data=80) into full buffer:
    Push 80 -> REJECTED (Buffer Full, Waste-One-Slot preserved)

[!] Popping 3 items:
    Popped: 10 (Available: 6)
    Popped: 20 (Available: 5)
    Popped: 30 (Available: 4)

[!] Pushing 3 more items (forcing head pointer wrap-around):
    Push 80  -> OK (Available: 5)
    Push 90  -> OK (Available: 6)
    Push 100 -> OK (Available: 7)

[!] Popping all remaining items:
    Popped: 40, 50, 60, 70, 80, 90, 100
    Buffer is now empty.
```

Exit code: `0` on success, non-zero on error.

### Expected Unit Test Output

When run via `ctest --test-dir build -V`:

```
test_ring_buffer_lf.c:XX:test_initial_state_empty:PASS
test_ring_buffer_lf.c:XX:test_push_until_full_waste_one_slot:PASS
test_ring_buffer_lf.c:XX:test_push_full_returns_false:PASS
test_ring_buffer_lf.c:XX:test_pop_empty_returns_false:PASS
test_ring_buffer_lf.c:XX:test_wrap_around_pointer_math:PASS
test_ring_buffer_lf.c:XX:test_peek_does_not_advance_tail:PASS

-----------------------
6 Tests 0 Failures 0 Ignored
OK
```

### Submission

```
Exercise_3/
├── CMakeLists.txt               (required)
├── src/
│   ├── main.c                   (required)
│   └── ring_buffer_lf.c         (required)
├── include/
│   └── ring_buffer_lf.h         (required)
└── test/
    └── test_ring_buffer_lf.c    (required — Unity unit tests)
```

---

## Exercise_4 [build]

### Problem Statement

**Moving Average Filter with O(1) Running Sum**

**Scenario:**
Analog-to-Digital Converters (ADCs) in real-world microcontrollers suffer from electrical noise, thermal drift, and supply fluctuations. To feed stable data into control loops (e.g., PID temperature control), sensor data must be smoothed.

A naive moving average recalculates the sum over $N$ samples on every reading ($O(N)$), wasting precious CPU cycles. By coupling a circular buffer with a **running sum**, the filter executes in deterministic **$O(1)$ time complexity**.

**Mechanism Deep Dive:**

- The circular buffer stores the latest $N$ samples (where window size $N$ is a power of 2, e.g., $N=4$ or $N=16$).
- The `head` index tracks the next write position.
- **The Key Insight:** When the window is full, `buffer[head]` **points precisely to the oldest sample** in the sliding window!
- **$O(1)$ Update Steps:**
  1. **Subtract Oldest:** `running_sum -= buffer[head];`
  2. **Add Newest:** `running_sum += new_sample;`
  3. **Overwrite Slot:** `buffer[head] = new_sample;`
  4. **Advance Head:** `head = (head + 1) & MA_WINDOW_MASK;`
  5. **Compute Average:** When full, use bitshift division `running_sum >> MA_SHIFT`. During startup ($count < N$), divide by `count`.

**Requirements:**

1. Define `ma_filter_t` struct with a sample buffer of size `MA_WINDOW_SIZE = 4U`, `head` index, `running_sum` (`int32_t`), and `count` (`uint32_t`).
2. Implement filter operations:
   - `void ma_init(ma_filter_t *p_filt);`
   - `int16_t ma_process(ma_filter_t *p_filt, int16_t new_sample);` -> processes a sample in $O(1)$ and returns the current filtered average.
   - `void ma_reset(ma_filter_t *p_filt);`
3. Feed 20 simulated noisy ADC temperature readings (in tenths of °C, e.g., 250 = 25.0°C with noise spikes) into the filter.
4. Format and print a side-by-side comparison table showing Raw Sample vs Filtered Value.

**Edge Cases & Warnings for Juniors:**

> [!WARNING]
>
> - **Startup Phase (Zero-Bias Prevention):** When the filter is freshly initialized (`count < MA_WINDOW_SIZE`), the buffer is not yet full. Subtracting `buffer[head]` would subtract uninitialized data or pre-filled zeros, and dividing `running_sum` by the full window size ($N$) would artificially bias initial readings toward zero! During startup, **do not subtract `buffer[head]`**, and compute the average as `running_sum / count`.
> - **Integer Overflow on Running Sum:** The `running_sum` variable **MUST be signed 32-bit (`int32_t`)**, never `int16_t`. Adding multiple 12-bit/16-bit ADC values together quickly overflows a 16-bit integer, causing disastrous wrap-around to negative numbers.
> - **Power-of-2 Bitshift Division:** For positive integer sums, `running_sum >> 2` is equivalent to `running_sum / 4`, but executes in a single clock cycle on ARM Cortex-M0/M3/M4.

**Rules:**

- Follow BARR-C coding style.
- **Code Documentation:** All functions and data structures MUST be fully documented using Doxygen-style comments.
- Use `cppcheck` and `clang-tidy` to analyze, and make sure there are no warning or error messages.
- Build with CMake. Strict compiler flags (`-Wall -Wextra -pedantic -Werror -std=c11`) must be applied via `target_compile_options()`.
- Include Unity unit tests (see Expected Unit Test Output).

### Coding Standards Reference

**MISRA-C 2012 (Safety):**
| Rule | Category | Relevance to This Exercise |
|---|---|---|
| Directive 4.12 | Required | Dynamic memory allocation shall not be used → The moving average filter uses fixed static circular storage, guaranteeing deterministic memory footprints. |

**CERT-C 2016 (Security):**
| Rule | Relevance to This Exercise |
|---|---|
| INT30-C | Ensure that unsigned integer operations do not wrap → Ensure `count` increments cannot overflow. |
| INT32-C | Ensure that operations on signed integers do not result in overflow → `running_sum` must use `int32_t` with sufficient headroom for `WINDOW_SIZE * MAX_SAMPLE_VAL`. |

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs (under `C_Books/`) and read the full description of each rule above. After writing your code, verify your implementation follows these rules.

### Design Hints (optional)

**Source structure:**

```
Exercise_4/
├── CMakeLists.txt
├── src/
│   ├── main.c
│   └── moving_average.c
├── include/
│   └── moving_average.h
└── test/
    └── test_moving_average.c
```

**Example function prototypes (`moving_average.h`):**

```c
#include <stdint.h>

#define MA_WINDOW_SIZE  4U
#define MA_WINDOW_MASK  (MA_WINDOW_SIZE - 1U)
#define MA_SHIFT        2U   /* 1 << 2 = 4 */

typedef struct {
    int16_t  buffer[MA_WINDOW_SIZE];
    uint32_t head;
    int32_t  running_sum;
    uint32_t count;
} ma_filter_t;

void ma_init(ma_filter_t *p_filt);
int16_t ma_process(ma_filter_t *p_filt, int16_t new_sample);
void ma_reset(ma_filter_t *p_filt);
```

**CMakeLists.txt hint:**

```cmake
cmake_minimum_required(VERSION 3.14)
project(exercise_4 C)

add_compile_options(-Wall -Wextra -pedantic -Werror -std=c11)

add_executable(exercise_4 src/main.c src/moving_average.c)
target_include_directories(exercise_4 PRIVATE include)

# Unity
include(FetchContent)
FetchContent_Declare(unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG        v2.6.0
)
FetchContent_MakeAvailable(unity)

enable_testing()
add_executable(test_exercise_4 test/test_moving_average.c src/moving_average.c)
target_include_directories(test_exercise_4 PRIVATE include)
target_link_libraries(test_exercise_4 PRIVATE unity)
add_test(NAME test_exercise_4 COMMAND test_exercise_4)
```

### Acceptance Criteria (Scoring)

- **[10%]** CMake project builds successfully (`cmake -S . -B build && cmake --build build`).
- **[10%]** All Unity tests pass (`ctest --test-dir build -V`).
- **[15%]** Code passes static analysis (`cppcheck`, `clang-tidy`).
- **[15%]** Code contains required Doxygen documentation.
- **[50%]** Logic correctly maintains $O(1)$ running sum without buffer re-traversal, properly uses `head` to overwrite the oldest sample, correctly handles the startup phase without zero-bias, and passes all edge case unit tests.

### Expected Output

When run via `./build/exercise_4`:

```
========== L12 Lab 4: Moving Average Filter ==========

[!] Feeding 20 noisy temperature ADC samples (Window Size = 4):

Sample # | Raw Temp (0.1°C) | Filtered (0.1°C) | Status / Notes
-----------------------------------------------------------------
   01    |       250        |       250        | Startup (1/4)
   02    |       262        |       256        | Startup (2/4)
   03    |       248        |       253        | Startup (3/4)
   04    |       265        |       256        | Full Window Reached
   05    |       245        |       255        | Oldest 250 replaced
   06    |       280 (Spike)|       259        | Noise smoothed
   07    |       252        |       260        | Smoothing active
   08    |       249        |       256        | Spike evicted
...
```

Exit code: `0` on success, non-zero on error.

### Expected Unit Test Output

When run via `ctest --test-dir build -V`:

```
test_moving_average.c:XX:test_startup_phase_no_zero_bias:PASS
test_moving_average.c:XX:test_constant_signal_returns_same_value:PASS
test_moving_average.c:XX:test_step_response_sliding_window:PASS
test_moving_average.c:XX:test_spike_smoothing_and_eviction:PASS
test_moving_average.c:XX:test_reset_clears_history:PASS

-----------------------
5 Tests 0 Failures 0 Ignored
OK
```

### Submission

```
Exercise_4/
├── CMakeLists.txt               (required)
├── src/
│   ├── main.c                   (required)
│   └── moving_average.c         (required)
├── include/
│   └── moving_average.h         (required)
└── test/
    └── test_moving_average.c    (required — Unity unit tests)
```

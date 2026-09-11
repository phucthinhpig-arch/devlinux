> **📣 Message from your instructor:**
>
> Hi folks,
>
> This advanced C programming course recommends young engineers to code on your own! If possible, let's first try to write code from scratch. If it's hard, you guys can ask AI coding tool assistant! Don't let AI agent generate code for you!!
>
> Happy coding geeks! 🚀

---

# Assignment — Session 02: Memory Layout & Stack Analysis
**Deadline: 2026-09-27 23:59:00**

---

## Exercise_1 [build]

### Problem Statement

**Memory Segment Analyzer — Map, Measure, and Verify**

**Scenario:**
In embedded systems, understanding exactly where your data lives in memory is not academic — it directly impacts performance, debuggability, and safety. A firmware engineer who cannot map variables to memory segments will struggle to diagnose stack overflows, heap corruption, and linker errors that are common in bare-metal and RTOS environments.

**Objective:**
Write a C program that declares variables in **each memory segment** (text/rodata, data, BSS, heap, stack), prints their addresses, and calculates the distance between segments using pointer arithmetic. Then verify your findings using `size` and `nm`.

**Requirements:**

Implement a function with the following prototype:

```c
void print_memory_map(void);
```

**Rules:**
- Declare at least one variable in each segment:
  - `.text` — function pointer (e.g., address of `main` or `print_memory_map`)
  - `.rodata` — `const` global variable (e.g., `const uint32_t global_const = 100;`)
  - `.data` — initialized global variable (e.g., `uint32_t global_init = 42;`)
  - `.bss` — uninitialized global variable (e.g., `uint32_t global_uninit;`)
  - heap — dynamically allocated variable using `malloc()`
  - stack — local variable inside a function
- Print the address of each variable using `printf` with `%p` format specifier and proper `(void *)` cast.
- Calculate and print the distance (in bytes) between segments using `uintptr_t` arithmetic from `<stdint.h>`.
- Free all dynamically allocated memory before the program exits.
- Follow BARR-C coding style (fixed-width integers, mandatory braces, pointer naming with `p_` prefix, Doxygen comments).
- Use `cppcheck` and `clang-tidy` to analysis, and make sure there are no warning or error message.
- Compile with strict flags: `-Wall -Wextra -pedantic -Werror -std=c99`.

### Coding Standards Reference

This exercise is a good opportunity to study the following industry rules. You don't need to memorize them — read the rule, understand *why* it exists, and apply the pattern in your code.

**MISRA-C 2012 (Safety):**
| Rule | Category | Relevance to This Exercise |
|---|---|---|
| Directive 4.12 | Required | Dynamic memory allocation shall not be used → you are using `malloc()`/`free()` in this exercise. Understand **why** MISRA forbids this in safety-critical systems (heap fragmentation, out-of-memory risk) even though this exercise requires it for learning. |
| Rule 9.1 | Mandatory | The value of an object with automatic storage duration shall not be read before it has been set → initialize all local (stack) variables before printing their addresses or values. |
| Rule 11.4 | Advisory | A conversion should not be performed between a pointer to object and an integer type → casting addresses to `uintptr_t` for distance calculations exercises this rule. Use `uintptr_t` from `<stdint.h>` as the safe integer type for pointer values. |
| Rule 11.6 | Required | A cast shall not be performed between pointer to void and an arithmetic type → `malloc()` returns `void *`; be careful when casting it. |
| Rule 22.1 | Required | All memory that is allocated dynamically shall be explicitly released → every `malloc()` must have a matching `free()`. |

**CERT-C 2016 (Security):**
| Rule | Relevance to This Exercise |
|---|---|
| EXP34-C | Do not dereference null pointers → check that `malloc()` does not return `NULL` before using the heap pointer. |
| EXP36-C | Do not cast pointers into more strictly aligned pointer types → be careful with pointer type conversions when printing addresses. |
| MEM30-C | Do not access freed memory → after calling `free()`, do not use the pointer again. Set it to `NULL` after freeing. |
| MEM31-C | Free dynamically allocated memory when no longer needed → ensure all `malloc()`'d memory is freed before program exit. |
| ERR33-C | Detect and handle standard library errors → `malloc()` can fail; check its return value. |

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs (under `C_Books/`) and read the full description of each rule above. After writing your code, verify your implementation follows these rules.


### Expected Output

Addresses will vary on your machine, but the format should be:

```
=== Memory Segment Map ===
[TEXT]   Address of main():            0x55a3b4001169
[RODATA] Address of global_const:      0x55a3b4002004
[DATA]   Address of global_init:       0x55a3b4004010
[BSS]    Address of global_uninit:     0x55a3b4004018
[HEAP]   Address of heap_var:          0x55a3b52922a0
[STACK]  Address of stack_var:         0x7ffd8b3e1a2c

=== Segment Distances ===
RODATA - TEXT:   ... bytes
DATA   - RODATA: ... bytes
BSS    - DATA:   ... bytes
HEAP   - BSS:    ... bytes
STACK  - HEAP:   ... bytes
```

Exit code: `0` on success, non-zero on error.

### Verification Step (Required)

After compiling, run the following commands and **include the output as a comment block** at the bottom of your `main.c`:

1. **`size` output** — shows text/data/bss segment sizes:
   ```bash
   size <your_binary>
   ```

2. **`nm` output** — verify at least 2 variables are in the expected segment:
   ```bash
   nm <your_binary> | grep -i global_init
   nm <your_binary> | grep -i global_uninit
   ```
   Look for the segment letter: `D` = data, `B` = BSS, `R` = rodata, `T` = text.

### Submission

```
Exercise_1/
├── main.c        (required)
├── Makefile      (required — targets: all, clean)
└── *.h           (if any)
```

---

## Exercise_2 [build]

### Problem Statement

**Stack Depth Monitor — Measure Stack Usage & Detect Overflow Risk**

**Scenario:**
Stack overflow is one of the most dangerous runtime failures in embedded systems — it silently corrupts memory and causes unpredictable crashes that are extremely hard to debug. In RTOS environments (FreeRTOS, Zephyr), each task has a fixed-size stack, and knowing exactly how much stack a function chain consumes is critical for setting safe stack sizes.

**Objective:**
Write a C program that implements a recursive function which monitors stack depth by recording the address of a local variable at each recursion level. The program must detect when stack consumption exceeds a configurable threshold and stop recursion safely — **before** a real stack overflow occurs.

**Requirements:**

Implement a function with the following prototype:

```c
int8_t recurse_with_monitor(uint32_t current_depth, uint32_t max_depth,
                             const uintptr_t stack_base_addr,
                             uint32_t stack_limit_bytes);
```

**Rules:**
- At each recursion level, declare a local variable (e.g., `uint8_t stack_marker;`) and record its address.
- Calculate current stack usage as the distance between `stack_base_addr` (captured in `main`) and the current `stack_marker` address.
- If stack usage exceeds `stack_limit_bytes`, print a warning and return an error code **without** recursing further (safe abort).
- Print the recursion depth, local variable address, and current stack usage at each level.
- Return `0` on success (max depth reached), or `-1` if the stack limit was hit.
- The `stack_base_addr` should be captured in `main()` using the address of a local variable cast to `uintptr_t`.
- Follow BARR-C coding style (fixed-width integers, mandatory braces, pointer naming with `p_` prefix, Doxygen comments).
- Use `cppcheck` and `clang-tidy` to analysis, and make sure there are no warning or error message.
- Compile with strict flags: `-Wall -Wextra -pedantic -Werror -std=c99`.

### Coding Standards Reference

This exercise is a good opportunity to study the following industry rules. You don't need to memorize them — read the rule, understand *why* it exists, and apply the pattern in your code.

**MISRA-C 2012 (Safety):**
| Rule | Category | Relevance to This Exercise |
|---|---|---|
| Directive 4.1 | Required | Run-time failures shall be minimized → stack overflow is a run-time failure. This exercise teaches you to **detect** it before it happens. |
| Rule 9.1 | Mandatory | The value of an object with automatic storage duration shall not be read before it has been set → initialize the `stack_marker` variable and all local variables before use. |
| Rule 11.4 | Advisory | A conversion should not be performed between a pointer to object and an integer type → you must cast `&stack_marker` to `uintptr_t` for stack distance arithmetic. Understand why this is necessary. |
| Rule 17.2 | Required | Functions shall not call themselves, either directly or indirectly (no recursion) → **This exercise intentionally uses recursion**. Study this rule and document in a comment **why** MISRA forbids recursion (unbounded stack growth) and how your `stack_limit_bytes` guard makes this usage controlled and safe. |
| Rule 15.5 | Advisory | A function should have a single point of exit → the function has multiple exit conditions (base case + stack limit); consider how to structure them. |

**CERT-C 2016 (Security):**
| Rule | Relevance to This Exercise |
|---|---|
| MEM05-C | Avoid large stack allocations → recursive functions consume stack per call. This is the core lesson: monitor and limit stack consumption to prevent exhaustion. |
| EXP34-C | Do not dereference null pointers → validate any pointer parameters if the design is extended. |
| INT32-C | Ensure that operations on signed integers do not result in overflow → stack depth calculations and comparisons with `stack_limit_bytes` must not overflow. Use unsigned types (`uint32_t`, `uintptr_t`). |
| ARR30-C | Do not form or use out-of-bounds pointers or array subscripts → if you store recursion data in an array for tracking, enforce bounds. |
| EXP33-C | Do not read uninitialized memory → the `stack_marker` variable and all tracking variables must be initialized before use. |

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs and read the full description of each rule above. After writing your code, verify your implementation follows these rules.

### Design Hints (optional)

```c
#include <stdint.h>
#include <stdio.h>

/**
 * @brief Monitor stack depth during recursion and abort if limit is exceeded.
 *
 * @param[in] current_depth     Current recursion depth (starts at 0).
 * @param[in] max_depth         Maximum recursion depth to attempt.
 * @param[in] stack_base_addr   Address of a local variable in main(), cast to uintptr_t.
 * @param[in] stack_limit_bytes Maximum allowed stack consumption in bytes.
 * @return 0 on success (max_depth reached), -1 if stack limit was exceeded.
 */
int8_t recurse_with_monitor(uint32_t current_depth, uint32_t max_depth,
                             const uintptr_t stack_base_addr,
                             uint32_t stack_limit_bytes);
```

### Suggested Approach (optional)

```
1. In main(): declare a local variable, capture its address as stack_base_addr
2. Call recurse_with_monitor(0, 100, stack_base_addr, 4096)
3. In recurse_with_monitor():
   a. Declare local uint8_t stack_marker
   b. Calculate: stack_used = stack_base_addr - (uintptr_t)&stack_marker
   c. Print depth, address, and stack_used
   d. If stack_used >= stack_limit_bytes → print WARNING, return -1
   e. If current_depth >= max_depth → return 0 (success)
   f. Otherwise → recurse: return recurse_with_monitor(current_depth + 1, ...)
4. In main(): print the final result
```

### Expected Output

Addresses will vary, but the format should be:

```
=== Stack Depth Monitor (limit: 4096 bytes) ===
[Depth  0] stack_marker addr: 0x7ffd8b3e1a00, stack used:    0 bytes
[Depth  1] stack_marker addr: 0x7ffd8b3e19c0, stack used:   64 bytes
[Depth  2] stack_marker addr: 0x7ffd8b3e1980, stack used:  128 bytes
...
[Depth 63] stack_marker addr: 0x7ffd8b3e0a00, stack used: 4032 bytes
[Depth 64] WARNING: Stack usage (4096 bytes) exceeds limit! Aborting recursion.
Result: -1 (stack limit reached at depth 64)
```

Exit code: `0` on success, non-zero on error.

### Submission

```
Exercise_2/
├── main.c        (required)
├── Makefile      (required — targets: all, clean)
└── *.h           (if any)
```

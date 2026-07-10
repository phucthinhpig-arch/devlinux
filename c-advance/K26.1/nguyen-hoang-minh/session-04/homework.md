> **📣 Message from your instructor:**
>
> Hi folks,
>
> This advanced C programming course recommends young engineers to code on your own! If possible, let's first try to write code from scratch. If it's hard, you guys can ask AI coding tool assistant! Don't let AI agent generate code for you!!
>
> Happy coding geeks! 🚀

---

# Assignment — Session 04: Object-Oriented C, Memory Optimization & Code Portability
**Deadline: 2026-07-12 23:59:00**

---

## Exercise_1 [build]

### Problem Statement

**Polymorphic Display Driver**

**Scenario:**
You are building an embedded system that needs to draw to a display. We want to achieve two core Object-Oriented principles in C:
1. **Encapsulation**: Hide the internal configuration data of the display driver using **Opaque Pointers** (which is just a common name for a forward-declared incomplete struct) so the application cannot tamper with it.
2. **Polymorphism**: Decouple our drawing logic from the exact hardware using a Hardware Abstraction Layer (HAL) built with structs of function pointers.

**Requirements:**
Write a C program that implements an encapsulated, polymorphic display driver:
1. Define a forward-declared opaque pointer (an incomplete struct used to hide data) `typedef struct display_config_s display_config_t;` in your header to hide the configuration data.
2. Define an interface `i_display_t` (a struct) with two function pointers:
   - `void (*init)(display_config_t *config);`
   - `void (*draw_pixel)(uint16_t x, uint16_t y, uint8_t color);`
3. Create two concrete implementations:
   - **Console Display**: `console_display_init()` and `console_display_draw_pixel()`. The `init` function must cast the opaque pointer back to its concrete struct (which you define inside `console_display.c`). The `draw_pixel()` function is basically just a `printf` log to the terminal (e.g. `[Console] Drawing pixel at (x,y) with color C`), simulating drawing on a screen.
   - **Dummy Display (Mock)**: `dummy_display_init()` and `dummy_display_draw_pixel()`. *Why do we need a dummy display?* In real-world embedded development, you often have to write application code before the physical hardware (like an OLED screen) is ready. A "dummy" or "mock" function safely does nothing but allows you to test your application logic without crashing. Here, `dummy_display_draw_pixel()` should just increment a static global counter `dummy_draw_count` so we can verify our logic called it the correct number of times.
4. Provide a factory function `display_config_t* console_config_create(uint32_t baud_rate)` that statically or dynamically allocates the config and returns the opaque pointer.
5. Create a global instance of `i_display_t` for both the console and the dummy display.
6. Write an application function `void draw_rectangle(i_display_t *disp)` that uses the interface to "draw" a 2x2 rectangle (4 pixels total) starting at (0,0). Since drawing is just printing logs, calling this function will result in 4 `printf` logs.
7. In `main()`, obtain the opaque config pointer, pass it to the console display's `init`, call `draw_rectangle`, and repeat for the dummy display. Print the `dummy_draw_count` at the end to prove it was called 4 times.

**Rules:**
- Check for `NULL` function pointers defensively before calling them.
- Use `cppcheck` and `clang-tidy` to analyze, and make sure there are no warning or error messages.
- Follow BARR-C coding style (fixed-width integers, mandatory braces, pointer naming with `p_` prefix).
- **Code Documentation:** All functions and data structures MUST be fully documented using Doxygen-style comments.
- Compile with strict flags: `-Wall -Wextra -pedantic -Werror -std=c99`.

### Coding Standards Reference

**MISRA-C 2012 (Safety):**
| Rule | Category | Relevance to This Exercise |
|---|---|---|
| Directive 4.8 | Advisory | If a pointer to a structure or union is never dereferenced within a translation unit, then the implementation of the object should be hidden → Enforces the use of opaque pointers for data hiding. |
| Directive 4.11 | Required | The validity of values passed to library functions shall be checked → validate `disp` and its function pointers for `NULL` before use. |
| Rule 11.3 | Required | A cast shall not be performed between a pointer to object type and a pointer to a different object type → Ensure function pointer signatures match exactly to avoid undefined behavior. |
| Rule 15.6 | Required | The body of an iteration-statement or a selection-statement shall be a compound-statement → Mandatory braces for the `NULL` checks. |

**CERT-C 2016 (Security):**
| Rule | Relevance to This Exercise |
|---|---|
| EXP34-C | Do not dereference null pointers → Extremely critical for function pointers in structs. Validate before calling. |

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs (under `C_Books/`) and read the full description of each rule above. After writing your code, verify your implementation follows these rules.

### Design Hints (optional)

**Suggested file structure:**
```
Exercise_1/
├── i_display.h          ← HAL interface + opaque pointer (shared by all)
├── console_display.h    ← factory function + extern driver instance
├── console_display.c    ← concrete struct definition + implementation
├── dummy_display.h      ← extern driver instance + call counter
├── dummy_display.c      ← mock implementation
├── main.c               ← application logic only
└── Makefile
```

**Key steps:**

1. In `i_display.h`, forward-declare the opaque type and define the interface:
```c
typedef struct display_config_s display_config_t; /* incomplete type — data hiding */

typedef struct i_display_s {
    void (*init)(display_config_t *p_config);
    void (*draw_pixel)(uint16_t x, uint16_t y, uint8_t color);
} i_display_t;
```

2. In `console_display.c` **only**, write the full `struct display_config_s { ... };` definition. This is what makes the data truly hidden — any other `.c` that tries to access a field will get a compiler error.

3. Provide a factory function that statically allocates the config and returns the opaque handle:
```c
/* console_display.h */
display_config_t *console_config_create(uint32_t baud_rate);
extern i_display_t console_display;
```

4. In `main.c`, you hold a `display_config_t *` pointer but the compiler will refuse to let you access any fields — that is the encapsulation at work. Try it: add `p_cfg->baud_rate` and see what the compiler says!

5. `draw_rectangle()` in `main.c` must accept only `i_display_t *` — it must not know anything about the concrete driver behind it.

### Acceptance Criteria (Scoring)
- **[20%]** Code builds successfully without warnings or errors.
- **[20%]** Code passes static analysis (`cppcheck`, `clang-tidy`).
- **[20%]** Code contains required Doxygen documentation.
- **[40%]** Logic correctly implements polymorphism and encapsulation (opaque pointers), and the output matches the expected output exactly.

### Expected Output

```
[Console] Drawing pixel at (0,0) with color 1
[Console] Drawing pixel at (1,0) with color 1
[Console] Drawing pixel at (0,1) with color 1
[Console] Drawing pixel at (1,1) with color 1
Dummy display was called 4 times.
```
Exit code: `0`

### Submission
```
Exercise_1/
├── main.c                   (required)
├── i_display.h              (required — contains the HAL interface and opaque pointer definition)
├── console_display.c / .h   (required — concrete implementation and config data hiding)
├── dummy_display.c / .h     (required — concrete implementation and config data hiding)
└── Makefile                 (required — targets: all, clean)
```

---

## Exercise_2 [build]

### Problem Statement

**The Object Pool Allocator**

**Scenario:**
Dynamic memory allocation (`malloc`/`free`) is forbidden in many safety-critical embedded systems due to fragmentation and non-deterministic execution time. Instead, we use Object Pools (block allocators) to manage statically allocated memory at runtime.

> 💡 **Why is this "Memory Optimization"?**
> When you write `static network_packet_t s_pool[5];`, the compiler places the entire pool array in the **`.bss`** section (zero-initialized static storage) — not on the Stack and not on the Heap.
> This means:
> - **Size is fixed at compile time** → the linker knows exactly how much RAM is needed before the firmware runs. No surprises at runtime.
> - **No fragmentation** → you always hand out fixed-size objects, so holes can never form.
> - **O(1) worst-case allocation** → scanning 5 slots is deterministic; `malloc` is not.
> - **Lives in RAM, costs zero Flash** → static variables in `.bss` are zero-initialised by the startup code, so no initial values need to be stored in Flash.
>
> This is exactly how **LwIP**, **FreeRTOS**, and **AUTOSAR** manage their internal objects.

**Requirements:**
Write a generic Object Pool manager in C to manage network packets.
1. Define a `network_packet_t` struct containing a `uint32_t id` and a `uint8_t payload[64]`.
2. Statically allocate an array of exactly **5** `network_packet_t` structs (the pool), and a boolean array `bool packet_in_use[5]` to track which packets are allocated.
3. Implement `network_packet_t* packet_alloc(void)` which searches for the first available packet, marks it as in-use, and returns its pointer. If the pool is full, it must return `NULL`.
4. Implement `void packet_free(network_packet_t *p_packet)` which marks the given packet as available again. Ensure it handles `NULL` pointers gracefully and validates that the pointer actually belongs to the pool.
5. In `main()`, write a test that:
   - Allocates 5 packets successfully.
   - Attempts to allocate a 6th packet and proves it fails (returns `NULL`).
   - Frees one packet.
   - Allocates a new packet and proves it succeeds.

**Rules:**
- **No dynamic memory allocation (`malloc`, `calloc`, `free`) is allowed.**
- Check for `NULL` pointers defensively.
- Ensure pointer arithmetic or array bounds are strictly respected.
- Use `cppcheck` and `clang-tidy` to analyze, and make sure there are no warning or error messages.
- Follow BARR-C coding style (fixed-width integers, mandatory braces, pointer naming with `p_` prefix).
- **Code Documentation:** All functions and data structures MUST be fully documented using Doxygen-style comments.
- Compile with strict flags: `-Wall -Wextra -pedantic -Werror -std=c99`.

### Coding Standards Reference

**MISRA-C 2012 (Safety):**
| Rule | Category | Relevance to This Exercise |
|---|---|---|
| Directive 4.12 | Required | Dynamic memory allocation shall not be used → The core principle of this exercise. |
| Rule 9.1 | Mandatory | The value of an object with automatic storage duration shall not be read before it has been set → Ensure the `packet_in_use` array is properly zero-initialized before use. |
| Rule 14.2 | Required | A `for` loop shall be well-formed → Iterating over the pool array must use deterministic bounds (`0` to `4`). |
| Rule 18.1 | Required | A pointer resulting from arithmetic on a pointer operand shall address an element of the same array → Validating that `p_packet` passed to `packet_free` is within the statically allocated pool array. |

**CERT-C 2016 (Security):**
| Rule | Relevance to This Exercise |
|---|---|
| EXP34-C | Do not dereference null pointers → Validate the pointer returned by `packet_alloc` before using it. |
| ARR30-C | Do not form or use out-of-bounds pointers or array subscripts → Ensure `packet_in_use[i]` strictly bounds checks `i < 5`. |

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs (under `C_Books/`) and read the full description of each rule above. After writing your code, verify your implementation follows these rules.

### Design Hints (optional)

```c
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define POOL_SIZE 5

typedef struct {
    uint32_t id;
    uint8_t payload[64];
} network_packet_t;

/**
 * @brief Initialize the object pool (mark all as free)
 */
void pool_init(void);

/**
 * @brief Allocate a packet from the pool.
 * @return Pointer to packet, or NULL if pool is full.
 */
network_packet_t* packet_alloc(void);

/**
 * @brief Free a packet back to the pool.
 * @param p_packet Pointer to the packet to free.
 */
void packet_free(network_packet_t *p_packet);
```

### Acceptance Criteria (Scoring)
- **[20%]** Code builds successfully without warnings or errors.
- **[20%]** Code passes static analysis (`cppcheck`, `clang-tidy`).
- **[20%]** Code contains required Doxygen documentation.
- **[40%]** Logic correctly handles full pool condition (returns NULL) and correctly frees packets.

### Expected Output

```
Allocating packet 1: Success
Allocating packet 2: Success
Allocating packet 3: Success
Allocating packet 4: Success
Allocating packet 5: Success
Allocating packet 6: Failed (Pool Full)
Freeing packet 2...
Allocating packet 6 again: Success
```
Exit code: `0`

### Submission
```
Exercise_2/
├── main.c        (required)
├── Makefile      (required — targets: all, clean)
└── *.h           (if any)
```

---

## Exercise_2 — Part B [review-only]

### Problem Statement

**Memory Layout Inspection**

After successfully building your `exercise_2` binary, run the following command and answer the questions below:

```bash
size exercise_2
```

You should see output like:
```
   text    data     bss     dec     hex filename
   XXXX       X     YYY    ZZZZ    ZZZZ exercise_2
```

**Questions (write your answers as comments at the top of your `main.c`):**

1. Look at the **`bss`** value. The pool array `s_pool[5]` and `s_in_use[5]` are both `static` variables with no explicit initialiser. Which memory section do they live in (`.text`, `.data`, or `.bss`) and why?

2. If you changed the pool declaration to initialise it explicitly:
   ```c
   static network_packet_t s_pool[5] = {0};
   ```
   Would it move from `.bss` to `.data`? Why or why not? *(Hint: the C standard says zero-initialised static storage is equivalent to uninitialized static storage.)*

3. The `packet_alloc()` and `packet_free()` function bodies (compiled machine code) live in which memory section? On a real MCU with 64KB Flash and 16KB RAM, where would each section physically reside?

4. If `POOL_SIZE` was increased from `5` to `50`, which section grows and by how much? Calculate the expected new `.bss` size in bytes, given `sizeof(network_packet_t) = 68` bytes.

### Acceptance Criteria (Part B)
- **[25%]** Correct identification of `.bss` for the pool array with a clear justification.
- **[25%]** Correct explanation of zero-init equivalence (`.bss` vs `.data`).
- **[25%]** Correct mapping of function code to `.text`, and Flash/RAM mapping for a real MCU.
- **[25%]** Correct `.bss` growth calculation for `POOL_SIZE = 50`.

---

## Exercise_3 [build]

### Problem Statement

**Endian-Safe Protocol Parser**

**Scenario:**
You are writing firmware for a Little-Endian ARM Cortex-M4 microcontroller. It receives a 6-byte raw data payload from an external sensor over SPI. The sensor transmits data in **Big-Endian (Network Byte Order)** format.

The payload format is:
- Bytes 0-1: `temperature` (16-bit unsigned integer)
- Bytes 2-5: `timestamp` (32-bit unsigned integer)

**Requirements:**
Write a C program that parses this raw byte array into a C struct safely, regardless of the host CPU's endianness.
1. Define a struct `sensor_data_t` containing `uint16_t temperature` and `uint32_t timestamp`.
2. Write a parsing function `void parse_sensor_data(const uint8_t *p_buffer, sensor_data_t *p_out_data)`.
3. In `main()`, simulate receiving the raw buffer: `{0x01, 0x2C, 0x00, 0x00, 0x1A, 0x0A}`.
4. Pass this buffer to your parsing function.
5. Print the resulting integer values in decimal to verify they are correct.

**Rules:**
- **Strict aliasing rules apply**: You are NOT allowed to cast the `uint8_t*` buffer to a `uint16_t*` or `uint32_t*`. This is strictly forbidden as it causes Unaligned Memory Access hardware faults.
- You must use **bitwise shifts (`<<`, `>>`) and bitwise OR (`|`)** to reconstruct the integers byte-by-byte. This guarantees endian-safe execution.
- Check for `NULL` pointers defensively.
- Use `cppcheck` and `clang-tidy` to analyze, and make sure there are no warning or error messages.
- Follow BARR-C coding style.
- **Code Documentation:** All functions and data structures MUST be fully documented using Doxygen-style comments.
- Compile with strict flags: `-Wall -Wextra -pedantic -Werror -std=c99`.

### Coding Standards Reference

**MISRA-C 2012 (Safety):**
| Rule | Category | Relevance to This Exercise |
|---|---|---|
| Rule 10.3 | Required | The value of an expression shall not be assigned to an object with a narrower essential type → Be careful when applying bitwise shifts to `uint8_t` values; cast to `uint32_t` before shifting by 24. |
| Rule 11.3 | Required | A cast shall not be performed between a pointer to object type and a pointer to a different object type → Prohibits unsafe casting of `uint8_t*` to `uint32_t*`. |
| Rule 11.4 | Advisory | A conversion should not be performed between a pointer to object and an integer type → Use bitwise math instead of memory tricks. |

**CERT-C 2016 (Security):**
| Rule | Relevance to This Exercise |
|---|---|
| EXP34-C | Do not dereference null pointers → Validate `p_buffer` and `p_out_data`. |
| INT31-C | Ensure that integer conversions do not result in lost or misinterpreted data → When shifting bytes into a 32-bit integer, ensure implicit integer promotion does not result in sign-extension bugs. |

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs (under `C_Books/`) and read the full description of each rule above. After writing your code, verify your implementation follows these rules.

### Design Hints (optional)

```c
#include <stdint.h>
#include <stdio.h>

typedef struct {
    uint16_t temperature;
    uint32_t timestamp;
} sensor_data_t;

/**
 * @brief Parses big-endian raw sensor data into a struct safely.
 *
 * @param[in]  p_buffer    Pointer to the 6-byte raw payload array.
 * @param[out] p_out_data  Pointer to the struct to populate.
 */
void parse_sensor_data(const uint8_t *p_buffer, sensor_data_t *p_out_data);
```

### Acceptance Criteria (Scoring)
- **[20%]** Code builds successfully without warnings or errors.
- **[20%]** Code passes static analysis (`cppcheck`, `clang-tidy`).
- **[20%]** Code contains required Doxygen documentation.
- **[40%]** Parsing logic strictly uses bitwise shifts and output values match the expected result.

### Expected Output

```
Temperature: 300
Timestamp: 6666
```
Exit code: `0`

### Submission
```
Exercise_3/
├── main.c        (required)
├── Makefile      (required — targets: all, clean)
└── *.h           (if any)
```

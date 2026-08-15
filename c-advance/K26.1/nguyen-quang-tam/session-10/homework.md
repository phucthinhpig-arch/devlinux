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

# Assignment — Session 10: String Memory Architecture & Advanced Manipulation
**Deadline: 2026-08-16 23:59:00**

> [!NOTE]
> **Build System Change:** Starting from this lecture, all homework exercises use **CMake** instead of Makefile, and must include **Unity unit tests**. This reinforces what you learned in Lecture 9 (CMake & TDD).
>
> **How to build & test:**
> ```bash
> cmake -S . -B build && cmake --build build    # Build
> ./build/<executable_name>                      # Run main program
> ctest --test-dir build -V                      # Run unit tests
> ```

---

## Exercise_1 [build]

### Problem Statement

**String Memory Inspector**

**Scenario:**
Understanding exactly where strings live in memory is crucial for embedded systems. A string allocated in RAM behaves differently than one in Flash (ROM), and modifying the wrong one leads to fatal crashes. 

**Requirements:**
Write a C program that demonstrates string memory placement and manipulation:

1. Declare three strings holding the exact same text (e.g., `"firmware"`):
   - A pointer to a string literal (`const char *`)
   - A stack-allocated character array (`char[]`)
   - A heap-allocated string (`malloc` + `strcpy`)
2. Print the memory address of each string using `%p` and identify which memory region they belong to (Read-Only Data, Stack, Heap).
   *(Hint: Use `nm` or `objdump` to verify variables in read-only data sections, and use `gdb` for run-time investigation of stack and heap addresses!)*
3. Print the `sizeof` and `strlen` for the stack array, and explain the difference in your printf output.
4. Demonstrate modifiability:
   - Modify the first character of the stack string.
   - Modify the first character of the heap string.
   - (Leave a commented-out line showing an attempt to modify the literal string, with a comment explaining that this causes Undefined Behavior).
5. Demonstrate the null terminator: create a `char[5]` array initialized character-by-character without a `'\0'`. Print what `strlen` returns (it will likely be garbage).
6. Properly `free` the heap string.

**Rules:**
- Follow BARR-C coding style (fixed-width integers, mandatory braces, pointer naming with `p_` prefix).
- **Code Documentation:** All functions and data structures MUST be fully documented using Doxygen-style comments.
- Use `cppcheck` and `clang-tidy` to analyze, and make sure there are no warning or error messages.
- Build with CMake. Strict compiler flags (`-Wall -Wextra -pedantic -Werror -std=c11`) must be applied via `target_compile_options()`.
- Include Unity unit tests (see Expected Unit Test Output).

### Coding Standards Reference

**MISRA-C 2012 (Safety):**
| Rule | Category | Relevance to This Exercise |
|---|---|---|
| Rule 7.4 | Required | A string literal shall not be assigned to an object unless the object's type is `const char *`. |
| Rule 9.1 | Mandatory | The value of an object with automatic storage duration shall not be read before it has been set. |
| Rule 18.1 | Required | Pointer arithmetic shall not result in a pointer that addresses an element outside the array bounds (relevant for manual string manipulation). |

**CERT-C 2016 (Security):**
| Rule | Relevance to This Exercise |
|---|---|
| STR30-C | Do not attempt to modify string literals → modifying `.rodata` causes crashes or undefined behavior. |
| EXP33-C | Do not read uninitialized memory → ensure all string buffers are properly initialized before printing. |

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs (under `C_Books/`) and read the full description of each rule above. After writing your code, verify your implementation follows these rules.

### Design Hints (optional)

**Source structure:**
```
Exercise_1/
├── CMakeLists.txt
├── src/
│   ├── main.c                    # Main program with printf demos
│   └── string_memory.c           # Functions for string memory inspection
├── include/
│   └── string_memory.h           # Function prototypes
└── test/
    └── test_string_memory.c      # Unity tests
```

**Example function prototypes (`string_memory.h`):**
```c
#include <stdint.h>
#include <stddef.h>

/**
 * @brief Get the memory region label for a given pointer.
 *
 * @param[in] p_addr  Pointer to inspect.
 * @return "Stack", "Heap", or "Read-Only Data" (heuristic based on address range).
 */
const char* get_memory_region(const void *p_addr);

/**
 * @brief Safely copy a string to a heap-allocated buffer.
 *
 * @param[in] p_src  Source string (null-terminated).
 * @return Heap-allocated copy of the string, or NULL on failure. Caller must free().
 */
char* heap_string_copy(const char *p_src);
```

**CMakeLists.txt hint:**
```cmake
cmake_minimum_required(VERSION 3.14)
project(exercise_1 C)

add_compile_options(-Wall -Wextra -pedantic -Werror -std=c11)

# Main executable
add_executable(exercise_1 src/main.c src/string_memory.c)
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
add_executable(test_exercise_1 test/test_string_memory.c src/string_memory.c)
target_include_directories(test_exercise_1 PRIVATE include)
target_link_libraries(test_exercise_1 PRIVATE unity)
add_test(NAME test_exercise_1 COMMAND test_exercise_1)
```

### Acceptance Criteria (Scoring)

- **[10%]** CMake project builds successfully (`cmake -S . -B build && cmake --build build`).
- **[10%]** All Unity tests pass (`ctest --test-dir build -V`).
- **[15%]** Code passes static analysis (`cppcheck`, `clang-tidy`).
- **[15%]** Code contains required Doxygen documentation.
- **[50%]** Logic correctly allocates, prints, modifies, and frees the strings in all three memory regions, and demonstrates the null terminator pitfall.

### Expected Output

When run via `./build/exercise_1`, the output should look similar to:

```
========== L10 Lab 1: String Memory Inspector ==========

--- Memory Locations ---
Literal String Address (Read-Only Data): 0x55...
Stack String Address   (Stack Memory)  : 0x7f...
Heap String Address    (Heap Memory)   : 0x55...

--- String Modification ---
Modified Stack String: Firmware
Modified Heap String : Firmware

--- sizeof() vs strlen() ---
sizeof(stack_str) : 9 bytes (includes '\0')
strlen(stack_str) : 8 chars (excludes '\0')

--- Missing Null Terminator ---
strlen(bad_str) returned: 14 (Garbage value > 5!)

Heap memory freed successfully.
```

Exit code: `0` on success, non-zero on error.

### Expected Unit Test Output

When run via `ctest --test-dir build -V`:

```
test_string_memory.c:XX:test_heap_string_copy_valid:PASS
test_string_memory.c:XX:test_heap_string_copy_null_input:PASS
test_string_memory.c:XX:test_stack_array_sizeof_vs_strlen:PASS
test_string_memory.c:XX:test_string_literal_is_readonly_region:PASS

-----------------------
4 Tests 0 Failures 0 Ignored
OK
```

### Submission

```
Exercise_1/
├── CMakeLists.txt               (required)
├── src/
│   ├── main.c                   (required)
│   └── string_memory.c          (required)
├── include/
│   └── string_memory.h          (required)
└── test/
    └── test_string_memory.c     (required — Unity unit tests)
```

---

## Exercise_2 [build]

### Problem Statement

**Mini Command Parser**

**Scenario:**
Embedded devices frequently receive commands over UART, Bluetooth, or network sockets. These commands arrive as strings (e.g., `"MOTOR:START:500"`). You must safely parse these strings into actionable data using the `<string.h>` and `<stdio.h>` toolkits.

**Requirements:**
Write a C program. Given an array of incoming command strings:
`const char *commands[] = {"LED:ON", "MOTOR:START:500", "TEMP:READ", "LED:OFF", "MOTOR:STOP"};`

For each command in the array:
1. Use `strchr()` to find the first `':'` delimiter and extract the device name into a local buffer. Ensure you use safe bounded copying (`strncpy` with manual null termination, or `snprintf`).
2. Use `strcmp()` to identify the device (e.g., `"LED"`, `"MOTOR"`, `"TEMP"`).
3. **DO NOT use `strtok()`**. It destroys the original string, uses hidden global state, and is unsafe for embedded systems. Instead, use the **Command Parser Pattern** taught in the lecture: a combination of `strchr` (to find delimiters), pointer arithmetic (to extract substrings), and `sscanf` (to parse values safely).
4. If the device is `"MOTOR"`, use `sscanf()` to extract the numeric RPM value.
5. Use `snprintf()` to format a response string for each command (e.g., `"[OK] LED set to ON"` or `"[OK] MOTOR started at 500 RPM"`).
6. Print the parsed results and the formatted response.

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
| Directive 4.11 | Required | The validity of values passed to library functions shall be checked → check for `NULL` returns from `strchr` or `strtok`. |
| Rule 21.17 | Required | Use of string handling functions shall not result in accesses beyond the bounds of the objects referenced. |

**CERT-C 2016 (Security):**
| Rule | Relevance to This Exercise |
|---|---|
| EXP34-C | Do not dereference null pointers → validate `strchr` return values before dereferencing. |
| STR31-C | Guarantee that storage for strings has sufficient space for character data and the null terminator → required when copying the device name into a local buffer. |
| ERR33-C | Detect and handle standard library errors → always check the return value of `sscanf()`. |

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs (under `C_Books/`) and read the full description of each rule above. After writing your code, verify your implementation follows these rules.

### Design Hints (optional)

**Source structure:**
```
Exercise_2/
├── CMakeLists.txt
├── src/
│   ├── main.c                    # Main program (loops over commands, prints results)
│   └── command_parser.c          # Parsing logic
├── include/
│   └── command_parser.h          # Function prototypes
└── test/
    └── test_command_parser.c     # Unity tests
```

**Example function prototypes (`command_parser.h`):**
```c
#include <stdint.h>
#include <stddef.h>

/**
 * @brief Parses a command string and generates a response.
 * 
 * @param[in]  p_cmd      The raw command string (e.g., "LED:ON").
 * @param[out] p_response Buffer to hold the formatted response.
 * @param[in]  resp_size  Size of the response buffer.
 * @return 0 on success, -1 on parse failure.
 */
int8_t parse_command(const char *p_cmd, char *p_response, size_t resp_size);

/**
 * @brief Extracts the device name from a command string.
 *
 * @param[in]  p_cmd       The raw command string.
 * @param[out] p_device    Buffer to hold the extracted device name.
 * @param[in]  device_size Size of the device buffer.
 * @return 0 on success, -1 if no delimiter found.
 */
int8_t extract_device_name(const char *p_cmd, char *p_device, size_t device_size);
```

**CMakeLists.txt hint:**
```cmake
cmake_minimum_required(VERSION 3.14)
project(exercise_2 C)

add_compile_options(-Wall -Wextra -pedantic -Werror -std=c11)

add_executable(exercise_2 src/main.c src/command_parser.c)
target_include_directories(exercise_2 PRIVATE include)

# Unity
include(FetchContent)
FetchContent_Declare(unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG        v2.6.0
)
FetchContent_MakeAvailable(unity)

enable_testing()
add_executable(test_exercise_2 test/test_command_parser.c src/command_parser.c)
target_include_directories(test_exercise_2 PRIVATE include)
target_link_libraries(test_exercise_2 PRIVATE unity)
add_test(NAME test_exercise_2 COMMAND test_exercise_2)
```

### Acceptance Criteria (Scoring)

- **[10%]** CMake project builds successfully (`cmake -S . -B build && cmake --build build`).
- **[10%]** All Unity tests pass (`ctest --test-dir build -V`).
- **[15%]** Code passes static analysis (`cppcheck`, `clang-tidy`).
- **[15%]** Code contains required Doxygen documentation.
- **[50%]** Logic correctly parses device names, extracts numeric parameters with `sscanf`, safely formats responses with `snprintf`, and handles invalid formats gracefully.

### Expected Output

When run via `./build/exercise_2`:

```
========== L10 Lab 2: String Toolkit & Parsing ==========

--- Command Parser ---
Raw: LED:ON           -> Formatted Response: [OK] LED set to ON
Raw: MOTOR:START:500  -> Formatted Response: [OK] MOTOR started at 500 RPM
Raw: TEMP:READ        -> Formatted Response: [OK] TEMP read requested
Raw: LED:OFF          -> Formatted Response: [OK] LED set to OFF
Raw: MOTOR:STOP       -> Formatted Response: [OK] MOTOR stopped
```

Exit code: `0` on success, non-zero on error.

### Expected Unit Test Output

When run via `ctest --test-dir build -V`:

```
test_command_parser.c:XX:test_parse_led_on:PASS
test_command_parser.c:XX:test_parse_motor_start_with_rpm:PASS
test_command_parser.c:XX:test_parse_temp_read:PASS
test_command_parser.c:XX:test_extract_device_name_valid:PASS
test_command_parser.c:XX:test_extract_device_name_no_delimiter:PASS
test_command_parser.c:XX:test_parse_null_input:PASS

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
│   └── command_parser.c         (required)
├── include/
│   └── command_parser.h         (required)
└── test/
    └── test_command_parser.c    (required — Unity unit tests)
```

---

## Exercise_3 [build]

### Problem Statement

**Safe String Operations**

**Scenario:**
Dangling pointers and buffer overflows are the bane of embedded C programming. In this exercise, you will implement string manipulation functions using safe patterns (caller-provides-buffer and bounded copying).

**Requirements:**
Write a C program.

1. Implement `void safe_concat(char *p_dst, size_t dst_size, const char *p_src);` that safely concatenates strings with bounds checking. You may use `strncat` or `snprintf` internally.
2. Implement `void format_sensor_report(char *p_buf, size_t buf_size, const char *p_sensor_name, int32_t value, const char *p_unit);` using `snprintf`.
3. Test both functions with buffers that are intentionally too small to show that they gracefully handle truncation (without overflowing memory).
4. Demonstrate the **dangling pointer problem**: 
   - Write a function `char* get_log_prefix_BAD(void)` that returns a pointer to a local stack array. 
   - Comment out the call to this function in `main()` with a comment explaining *why* it is dangerous.
   - Implement the safe alternative using the caller-provides-buffer pattern: `void get_log_prefix_GOOD(char *p_buf, size_t buf_size);`.

**Rules:**
- Follow BARR-C coding style.
- **Code Documentation:** All functions and data structures MUST be fully documented using Doxygen-style comments.
- Use `cppcheck` and `clang-tidy` to analyze, and make sure there are no warning or error messages.
- Build with CMake. Strict compiler flags (`-Wall -Wextra -pedantic -Werror -std=c11`) must be applied via `target_compile_options()`.
- Include Unity unit tests (see Expected Unit Test Output).

### Coding Standards Reference

**CERT-C 2016 (Security):**
| Rule | Relevance to This Exercise |
|---|---|
| STR31-C | Guarantee that storage for strings has sufficient space for character data and the null terminator → critical for the `safe_concat` and formatting functions. |
| DCL30-C | Declare objects with appropriate storage durations → returning a pointer to a local array violates this and creates a dangling pointer. |

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs (under `C_Books/`) and read the full description of each rule above. After writing your code, verify your implementation follows these rules.

### Design Hints (optional)

**Source structure:**
```
Exercise_3/
├── CMakeLists.txt
├── src/
│   ├── main.c                    # Main program with printf demos
│   └── safe_string.c             # Safe string operations
├── include/
│   └── safe_string.h             # Function prototypes
└── test/
    └── test_safe_string.c        # Unity tests
```

**Example function prototypes (`safe_string.h`):**
```c
#include <stdint.h>
#include <stddef.h>

/** @brief Safely concatenate p_src onto p_dst without overflowing dst_size. */
void safe_concat(char *p_dst, size_t dst_size, const char *p_src);

/** @brief Format a sensor report into p_buf using snprintf. */
void format_sensor_report(char *p_buf, size_t buf_size,
                          const char *p_sensor_name, int32_t value,
                          const char *p_unit);

/** @brief Safe log prefix using caller-provides-buffer pattern. */
void get_log_prefix_GOOD(char *p_buf, size_t buf_size);
```

**CMakeLists.txt hint:**
```cmake
cmake_minimum_required(VERSION 3.14)
project(exercise_3 C)

add_compile_options(-Wall -Wextra -pedantic -Werror -std=c11)

add_executable(exercise_3 src/main.c src/safe_string.c)
target_include_directories(exercise_3 PRIVATE include)

# Unity
include(FetchContent)
FetchContent_Declare(unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG        v2.6.0
)
FetchContent_MakeAvailable(unity)

enable_testing()
add_executable(test_exercise_3 test/test_safe_string.c src/safe_string.c)
target_include_directories(test_exercise_3 PRIVATE include)
target_link_libraries(test_exercise_3 PRIVATE unity)
add_test(NAME test_exercise_3 COMMAND test_exercise_3)
```

### Acceptance Criteria (Scoring)

- **[10%]** CMake project builds successfully (`cmake -S . -B build && cmake --build build`).
- **[10%]** All Unity tests pass (`ctest --test-dir build -V`).
- **[15%]** Code passes static analysis (`cppcheck`, `clang-tidy`).
- **[15%]** Code contains required Doxygen documentation.
- **[50%]** Logic correctly performs bounded string operations, demonstrates truncation handling safely, and documents the dangling pointer hazard.

### Expected Output

When run via `./build/exercise_3`:

```
========== L10 Lab 3: Safe String Operations ==========

--- Safe Concatenation ---
Normal Buffer Log: [SYS_LOG] Voltage low.
Tiny Buffer Log  : [SYS_L

--- Sensor Formatting ---
Sensor Data: [PRESSURE] VAL:1024 hPa
Truncated Sensor Data: [PRESS

--- Buffer Passing Pattern ---
Safe prefix generated: [SYS_LOG] 
```

Exit code: `0` on success, non-zero on error.

### Expected Unit Test Output

When run via `ctest --test-dir build -V`:

```
test_safe_string.c:XX:test_safe_concat_normal:PASS
test_safe_string.c:XX:test_safe_concat_truncation:PASS
test_safe_string.c:XX:test_safe_concat_null_input:PASS
test_safe_string.c:XX:test_format_sensor_report_normal:PASS
test_safe_string.c:XX:test_format_sensor_report_truncation:PASS
test_safe_string.c:XX:test_get_log_prefix_good:PASS

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
│   └── safe_string.c            (required)
├── include/
│   └── safe_string.h            (required)
└── test/
    └── test_safe_string.c       (required — Unity unit tests)
```

---

## Exercise_4 [build]

### Problem Statement

**Binary Data vs. Strings (`mem*` vs `str*`)**

**Scenario:**
Not all data is null-terminated strings. In embedded systems, binary protocols (CAN bus, SPI, network packets) contain raw bytes. These bytes can legitimately be `0x00`. If you use `<string.h>` functions on binary data, they will stop prematurely at the first `0x00`. You must use memory operations instead.

**Requirements:**
Write a C program.

1. **`memset`**: Zero-initialize a struct (e.g., `sensor_data_t { int id; float temp; }`) using `memset`. Do NOT assign fields manually.
2. **`memcpy` vs `strcpy`**: Define a binary payload array: `uint8_t payload[8] = {0x01, 0x00, 0xFF, 0x00, 0x10, 0x20, 0x30, 0x40};`. 
   - Attempt to copy it to a new buffer using `strncpy(dst, (char*)payload, 8)`. Print the resulting buffer (it will truncate!).
   - Copy it using `memcpy(dst, payload, 8)`. Print the result (it will succeed).
3. **`memcmp` vs `strcmp`**: Define two binary IDs: `uint8_t id1[] = {0, 1, 0, 5};` and `uint8_t id2[] = {0, 1, 0, 9};`.
   - Compare them using `strcmp()`. Note that it wrongly returns `0` (equal) because it stops at the `0x00` byte!
   - Compare them using `memcmp()`. Note that it correctly returns a non-zero value.
4. **`memmove`**: Create an array `char buf[16] = "123456789";`. 
   - Use `memmove` to safely shift the characters `"456789"` two positions to the right to make room for new data. (Avoid `memcpy` here because the source and destination regions overlap).

**Rules:**
- Follow BARR-C coding style.
- **Code Documentation:** All functions and data structures MUST be fully documented using Doxygen-style comments.
- Use `cppcheck` and `clang-tidy` to analyze, and make sure there are no warning or error messages.
- Build with CMake. Strict compiler flags (`-Wall -Wextra -pedantic -Werror -std=c11`) must be applied via `target_compile_options()`.
- Include Unity unit tests (see Expected Unit Test Output).

### Coding Standards Reference

**CERT-C 2016 (Security):**
| Rule | Relevance to This Exercise |
|---|---|
| MEM33-C | Allocate and copy structures containing a flexible array member dynamically → More broadly, CERT-C recommends using `memmove` anytime source and destination might overlap, as the behavior of `memcpy` with overlapping regions is explicitly undefined. |

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs (under `C_Books/`) and read the full description of each rule above. After writing your code, verify your implementation follows these rules.

### Design Hints (optional)

**Source structure:**
```
Exercise_4/
├── CMakeLists.txt
├── src/
│   ├── main.c                   # Main program for printing demos
│   └── memory_ops.c             # Memory operations
├── include/
│   └── memory_ops.h             # Function prototypes
└── test/
    └── test_memory_ops.c        # Unity tests
```

**CMakeLists.txt hint:**
(Use the same CMake + FetchContent pattern as the previous exercises).

### Acceptance Criteria (Scoring)

- **[10%]** CMake project builds successfully (`cmake -S . -B build && cmake --build build`).
- **[10%]** All Unity tests pass (`ctest --test-dir build -V`).
- **[15%]** Code passes static analysis (`cppcheck`, `clang-tidy`).
- **[15%]** Code contains required Doxygen documentation.
- **[50%]** Logic correctly demonstrates the behavioral differences between `str*` and `mem*` functions on binary data, and successfully uses `memmove` for overlapping regions.

### Expected Output

When run via `./build/exercise_4`:

```
========== L10 Lab 4: Memory vs String Operations ==========

--- memset ---
Struct initialized to zeros.

--- memcpy vs strcpy ---
Payload: 01 00 FF 00 10 20 30 40
strncpy result: 01 00 00 00 00 00 00 00 (TRUNCATED at 0x00!)
memcpy result : 01 00 FF 00 10 20 30 40 (SUCCESS!)

--- memcmp vs strcmp ---
strcmp("0105", "0109") -> 0 (INCORRECT: strings match due to early 0x00)
memcmp("0105", "0109") -> -4 (CORRECT: arrays differ)

--- memmove ---
Before shift: 123456789
After shift : 123456456789
```

Exit code: `0` on success, non-zero on error.

### Expected Unit Test Output

When run via `ctest --test-dir build -V`:

```
test_memory_ops.c:XX:test_struct_zero_init:PASS
test_memory_ops.c:XX:test_memcpy_handles_null_bytes:PASS
test_memory_ops.c:XX:test_strncpy_fails_on_null_bytes:PASS
test_memory_ops.c:XX:test_memcmp_detects_difference:PASS
test_memory_ops.c:XX:test_strcmp_fails_to_detect_difference:PASS
test_memory_ops.c:XX:test_memmove_handles_overlap:PASS

-----------------------
6 Tests 0 Failures 0 Ignored
OK
```

### Submission

```
Exercise_4/
├── CMakeLists.txt               (required)
├── src/
│   ├── main.c                   (required)
│   └── memory_ops.c             (required)
├── include/
│   └── memory_ops.h             (required)
└── test/
    └── test_memory_ops.c        (required — Unity unit tests)
```

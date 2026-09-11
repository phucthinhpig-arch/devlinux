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

# Assignment — Lecture 14: Fundamentals of Low-Level I/O & System Calls
**Deadline: 2026-09-14 23:59:00**

> [!IMPORTANT]
> **Quality Gate & CMake Mandatory Requirement (L13+)**
> Starting from Lecture 13, all homework submissions must adhere strictly to industry-standard **Quality Gate** project structures.
>
> For **every exercise**, your project must satisfy four automated verification gates:
> 1. **Gate 1 — Pre-commit Tool Check**: You must provide `.pre-commit-config.yaml` and `.clang-tidy`. Running `pre-commit run --all-files` locally must pass with zero errors.
> 2. **Gate 2 — Compiler & Clang-Tidy Build Check**: Your `CMakeLists.txt` must set `CMAKE_C_CLANG_TIDY` and strict flags (`-Wall -Wextra -pedantic -Werror -std=c11`). Running `cmake -B build && cmake --build build` must build cleanly with zero compiler warnings and zero `clang-tidy` diagnostics.
> 3. **Gate 3 — Static Analysis Check**: Your `CMakeLists.txt` must provide a custom target for `cppcheck` (and optionally `misra_check`). Running `cmake --build build --target cppcheck` must complete with zero warnings.
> 4. **Gate 4 — Sanitizer & Unit Test Check**: Your `CMakeLists.txt` must support `-DSANITIZER=asan+ubsan`. Running `cmake -B build-asan -DSANITIZER=asan+ubsan && cmake --build build-asan && ctest --test-dir build-asan -V` must execute all Unity unit tests with zero memory errors or undefined behavior crashes.
>
> **How to generate starter configuration files:**
> - Generate default `.clang-tidy`:
>   ```bash
>   clang-tidy -dump-config > .clang-tidy
>   ```
> - Generate default `.pre-commit-config.yaml`:
>   ```bash
>   pre-commit sample-config > .pre-commit-config.yaml
>   ```

> [!NOTE]
> **POSIX Feature Test Macro**
> Exercises 1 and 3 use POSIX system calls (`open()`, `read()`, `write()`, `close()`, `fcntl()`) which are **not part of ISO C11**. When compiling with `-std=c11` (strict ISO mode), you must define `_POSIX_C_SOURCE=200809L` so that system headers expose POSIX declarations. The CMake templates in each exercise already include this definition. If you compile manually:
> ```bash
> gcc -Wall -Wextra -pedantic -Werror -std=c11 -D_POSIX_C_SOURCE=200809L -o program src/main.c
> ```

---

## Exercise_1 [build] — POSIX Structured Log Writer with stdio Comparison

### 📂 File Deliverables Breakdown

| File Path | Status | Description |
|---|---|---|
| `include/posix_logger.h` | **[PROVIDED INTERFACE]** | POSIX logger API declarations (`posix_write_all`, `posix_log_open`, `posix_log_write`, `posix_log_close`, `posix_set_nonblocking`). |
| `src/posix_logger.c` | **[MUST IMPLEMENT FROM SCRATCH]** | Implement all POSIX logger functions using `open()`/`write()`/`close()`/`fcntl()` system calls with full error handling. |
| `src/main.c` | **[MUST IMPLEMENT FROM SCRATCH]** | Demo program: Part A writes with stdio, Part B writes with POSIX, Part C demonstrates `fcntl()`. |
| `test/test_posix_logger.c` | **[MUST IMPLEMENT / EXTEND]** | Unity unit test suite verifying `write_all`, log formatting, file creation, and `fcntl` flag setting. |
| `CMakeLists.txt` | **[PROVIDED STARTER / CONFIGURE]** | Quality Gate CMake configuration integrating `clang-tidy`, `cppcheck`, `ASan+UBSan`, and `Unity`. |
| `.clang-tidy` | **[PROVIDED STARTER / VERIFY]** | Static analysis configuration file. |
| `.pre-commit-config.yaml` | **[PROVIDED STARTER / VERIFY]** | Pre-commit hook configuration file. |

---

### Problem Statement

You are developing a sensor data logging module for an **embedded Linux IoT gateway** (e.g., a Raspberry Pi running Yocto Linux). The gateway collects temperature and humidity readings from connected sensors and must persist them to a log file. Your task is to implement the logger using **both** standard C I/O (stdio) and POSIX system calls, then compare the two approaches.

**Requirements:**

1. **Implement `posix_write_all()`** — A robust write function that handles **partial writes** in a loop. This is the safe pattern from the lecture (Section §1.3). If `write()` returns fewer bytes than requested (due to signals, kernel buffer limits, etc.), retry until all bytes are written or an error occurs. Handle `EINTR` (interrupted by signal) by retrying.

2. **Implement `posix_log_open()`** — Opens a log file using `open()` with flags `O_WRONLY | O_CREAT | O_APPEND` and file permission `0644`. Returns the file descriptor on success, `-1` on error.

3. **Implement `posix_log_write()`** — Formats a single sensor reading into a local `char` buffer using `snprintf()`, then writes it to the file using `posix_write_all()`. Format each entry as:
   ```
   [NNNNN] SENSOR: temp=XX.XC humidity=YY%
   ```
   where `NNNNN` is a zero-padded 5-digit entry ID.

4. **Implement `posix_log_close()`** — Closes the file descriptor using `close()` and **checks the return value** (yes, `close()` can fail — e.g., on NFS write errors).

5. **Implement `posix_set_nonblocking()`** — Uses `fcntl()` with `F_GETFL` and `F_SETFL` to add the `O_NONBLOCK` flag to an open file descriptor without disturbing other flags. This is essential for embedded Linux daemons that must never block on device file reads.

6. **In `main()`**, demonstrate three parts:
   - **Part A (stdio)**: Write 3 sensor entries to `stdio_sensor.log` using `fopen()`/`fprintf()`/`fclose()`. Print progress to stdout.
   - **Part B (POSIX)**: Write the same 3 sensor entries to `posix_sensor.log` using your POSIX logger functions. Print progress including byte counts.
   - **Part C (fcntl)**: Open a file descriptor, set non-blocking mode with `posix_set_nonblocking()`, confirm success.
   - **Summary**: Print a comparison table explaining the key differences between stdio and POSIX approaches.

7. All error paths must be handled with `perror()` and proper cleanup (close open FDs before returning).

> [!IMPORTANT]
> **Critical Concept: The Null Terminator (`'\0'`) and File I/O**
>
> When writing strings to files with `write()`, you must use `strlen()` to get the character count — **do NOT include the null terminator `'\0'`**. Here's why:
>
> - `'\0'` is an **in-memory C convention only**: C uses `'\0'` so that functions like `strlen()`, `printf("%s")`, and `strcpy()` know where a string ends **in RAM**.
> - **Files do NOT need a terminating byte.** Files track their exact size via filesystem metadata and file offsets (EOF). Writing `'\0'` to a text file creates a corrupt/binary byte in your log.
> - **When reading text back** from a file with `read()` or `fread()`, the file content will NOT have a `'\0'`. You **must manually append it** in your buffer before treating the data as a C string:
>   ```c
>   ssize_t n = read(fd, buf, sizeof(buf) - 1U);  /* Reserve 1 byte for '\0' */
>   if (n > 0) {
>       buf[n] = '\0';  /* NOW it's a valid C string */
>   }
>   ```
>
> **Summary:** Write only the printable characters (including `'\n'` if needed) using `strlen()`. Add `'\0'` only in memory after reading.

> [!TIP]
> **Understanding the Relationship Between stdio and POSIX**
>
> | Aspect | `FILE *` (stdio.h) | File Descriptor (POSIX) |
> |:---|:---|:---|
> | Type | `FILE *` (opaque struct with buffer) | `int` (small non-negative integer) |
> | Buffering | User-space buffer managed by libc | None (raw kernel transfer) |
> | Portability | ISO C — works everywhere | POSIX only (Linux, macOS, *BSD) |
> | Standard handles | `stdin`, `stdout`, `stderr` | `0` (stdin), `1` (stdout), `2` (stderr) |
> | Error reporting | `ferror()`, `errno` | Return value `-1`, `errno` |
> | Headers | `<stdio.h>` | `<unistd.h>`, `<fcntl.h>` |
>
> `fopen("file", "w")` **internally calls** `open("file", O_WRONLY | O_CREAT | O_TRUNC, ...)` to get a file descriptor, then wraps it in a `FILE` struct with a buffer. They are **layers**, not alternatives.

**Quality Gate Rules:**
- **Local Pre-commit Gate**: Run `pre-commit run --all-files` — must pass cleanly.
- **Build & Clang-Tidy Gate**: Run `cmake -B build -DCMAKE_BUILD_TYPE=Debug && cmake --build build` — must compile with `-Wall -Wextra -pedantic -Werror -std=c11` and pass `clang-tidy` with 0 warnings.
- **Static Analysis Gate**: Run `cmake --build build --target cppcheck` — must report 0 defects.
- **Sanitizer & Test Gate**: Run `cmake -B build-asan -DSANITIZER=asan+ubsan -DCMAKE_BUILD_TYPE=Debug && cmake --build build-asan && ctest --test-dir build-asan -V` — all Unity unit tests must pass with 0 sanitizer violations.
- **Coding Style & Documentation**: Follow BARR-C coding standard (`p_` pointer prefixes, explicit `uint8_t`/`uint32_t` types, mandatory braces) and document all functions/structures with Doxygen comments.

---

### Design Hints (optional)

**1. POSIX Logger Interface (`include/posix_logger.h`):**
```c
#ifndef POSIX_LOGGER_H
#define POSIX_LOGGER_H

#include <stdint.h>
#include <sys/types.h>  /* ssize_t */

/**
 * @brief Write ALL bytes to a file descriptor, handling partial writes.
 *
 * Retries on partial writes and EINTR. This is the safe pattern
 * for POSIX write() — never assume a single write() transfers all bytes.
 *
 * @param[in] fd     Open file descriptor.
 * @param[in] p_buf  Pointer to data buffer.
 * @param[in] total  Total number of bytes to write.
 * @return Total bytes written on success, -1 on error.
 */
ssize_t posix_write_all(int32_t fd, const uint8_t *p_buf, size_t total);

/**
 * @brief Open a log file using POSIX open() with append mode.
 *
 * Creates the file if it doesn't exist (permission 0644).
 * Opens in append mode so multiple runs append to the same log.
 *
 * @param[in] p_path  Path to the log file.
 * @return File descriptor (>= 0) on success, -1 on error.
 */
int32_t posix_log_open(const char *p_path);

/**
 * @brief Write a formatted sensor log entry to the file.
 *
 * Formats the entry with snprintf() into a local buffer, then writes
 * using posix_write_all(). Does NOT write the null terminator to the file.
 *
 * @param[in] fd          Open file descriptor.
 * @param[in] entry_id    Zero-padded 5-digit entry number.
 * @param[in] temperature Temperature reading (e.g., 23.5).
 * @param[in] humidity    Humidity percentage (e.g., 55).
 * @return 0 on success, -1 on error.
 */
int32_t posix_log_write(int32_t fd, uint32_t entry_id,
                        float temperature, uint32_t humidity);

/**
 * @brief Close the log file descriptor with error checking.
 * @param[in] fd  File descriptor to close.
 * @return 0 on success, -1 on error.
 */
int32_t posix_log_close(int32_t fd);

/**
 * @brief Set a file descriptor to non-blocking mode using fcntl().
 *
 * Uses F_GETFL/F_SETFL to add O_NONBLOCK without disturbing other flags.
 *
 * @param[in] fd  Open file descriptor.
 * @return 0 on success, -1 on error.
 */
int32_t posix_set_nonblocking(int32_t fd);

#endif /* POSIX_LOGGER_H */
```

**2. Quality Gate CMake Template (`CMakeLists.txt`):**
```cmake
cmake_minimum_required(VERSION 3.16)
project(exercise_1 VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# POSIX feature test macro — required for open(), read(), write(), fcntl()
add_compile_definitions(_POSIX_C_SOURCE=200809L)

# Option: Sanitizers
set(SANITIZER "none" CACHE STRING "Sanitizer (none|asan|ubsan|asan+ubsan)")
set_property(CACHE SANITIZER PROPERTY STRINGS none asan ubsan "asan+ubsan")

# Gate 1: clang-tidy integration
find_program(CLANG_TIDY_EXE NAMES clang-tidy)
if(CLANG_TIDY_EXE)
    set(CMAKE_C_CLANG_TIDY ${CLANG_TIDY_EXE} --header-filter=${CMAKE_SOURCE_DIR}/include)
    message(STATUS "[Quality Gate] clang-tidy: ENABLED")
endif()

# Gate 2: cppcheck custom target
find_program(CPPCHECK_EXE NAMES cppcheck)
if(CPPCHECK_EXE)
    add_custom_target(cppcheck
        COMMAND ${CPPCHECK_EXE}
            --enable=all --inconclusive --std=c11
            --suppress=missingIncludeSystem
            --project=${CMAKE_BINARY_DIR}/compile_commands.json
            --error-exitcode=1
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "[Quality Gate] Running cppcheck..."
    )
endif()

# Library & Main Executable
add_library(posix_logger STATIC src/posix_logger.c)
target_include_directories(posix_logger PUBLIC include)
target_compile_options(posix_logger PRIVATE -Wall -Wextra -pedantic -Werror)

add_executable(exercise_1 src/main.c)
target_link_libraries(exercise_1 PRIVATE posix_logger)
target_compile_options(exercise_1 PRIVATE -Wall -Wextra -pedantic -Werror)

# Apply Sanitizers
if(SANITIZER STREQUAL "asan+ubsan")
    target_compile_options(posix_logger PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(posix_logger PUBLIC -fsanitize=address,undefined)
    target_compile_options(exercise_1 PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(exercise_1 PUBLIC -fsanitize=address,undefined)
endif()

# Gate 3: Unity Unit Tests via FetchContent
include(FetchContent)
FetchContent_Declare(
    unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG        v2.6.0
)
FetchContent_MakeAvailable(unity)
set_target_properties(unity PROPERTIES C_CLANG_TIDY "")

enable_testing()
add_executable(test_exercise_1 test/test_posix_logger.c)
target_link_libraries(test_exercise_1 PRIVATE posix_logger unity)
target_compile_options(test_exercise_1 PRIVATE -Wall -Wextra -pedantic -Werror)
if(SANITIZER STREQUAL "asan+ubsan")
    target_compile_options(test_exercise_1 PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(test_exercise_1 PUBLIC -fsanitize=address,undefined)
endif()
add_test(NAME test_exercise_1 COMMAND test_exercise_1)
```

**3. Starter `.clang-tidy`:**
```yaml
Checks: >
  -*,
  bugprone-*,
  cert-*,
  clang-analyzer-*,
  misc-*,
  -bugprone-easily-swappable-parameters
WarningsAsErrors: >
  bugprone-null-dereference,
  cert-err34-c,
  clang-analyzer-core.*
HeaderFilterRegex: 'include/.*\.h$'
```

**4. Starter `.pre-commit-config.yaml`:**
```yaml
repos:
  - repo: https://github.com/pre-commit/mirrors-clang-format
    rev: v17.0.6
    hooks:
      - id: clang-format
        types_or: [c, c++]
  - repo: https://github.com/pocc/pre-commit-hooks
    rev: v1.3.5
    hooks:
      - id: clang-tidy
        args: [-p=build]
```

**5. Unity Test Skeleton (`test/test_posix_logger.c`):**
```c
#include "unity.h"
#include "posix_logger.h"
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

static const char *TEST_LOG_PATH = "/tmp/test_posix_logger.log";

void setUp(void) {
    /* Remove test file before each test */
    unlink(TEST_LOG_PATH);
}

void tearDown(void) {
    unlink(TEST_LOG_PATH);
}

void test_write_all_basic(void) {
    int32_t fd = open(TEST_LOG_PATH, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    TEST_ASSERT_TRUE(fd >= 0);

    const char *msg = "hello";
    ssize_t written = posix_write_all(fd, (const uint8_t *)msg, strlen(msg));
    TEST_ASSERT_EQUAL_INT(5, (int)written);
    close(fd);

    /* Read back and verify */
    fd = open(TEST_LOG_PATH, O_RDONLY);
    char buf[16] = {0};
    ssize_t n = read(fd, buf, sizeof(buf) - 1U);
    buf[n] = '\0';  /* Must add '\0' manually after read()! */
    TEST_ASSERT_EQUAL_STRING("hello", buf);
    close(fd);
}

void test_log_open_creates_file(void) {
    int32_t fd = posix_log_open(TEST_LOG_PATH);
    TEST_ASSERT_TRUE(fd >= 0);
    posix_log_close(fd);

    /* Verify file was created by opening it read-only */
    int32_t fd_verify = open(TEST_LOG_PATH, O_RDONLY);
    TEST_ASSERT_TRUE(fd_verify >= 0);
    close(fd_verify);
}

void test_log_write_formats_correctly(void) {
    int32_t fd = posix_log_open(TEST_LOG_PATH);
    TEST_ASSERT_TRUE(fd >= 0);

    int32_t result = posix_log_write(fd, 1U, 23.5f, 55U);
    TEST_ASSERT_EQUAL_INT(0, result);
    posix_log_close(fd);

    /* Read back and verify format */
    int32_t fd_r = open(TEST_LOG_PATH, O_RDONLY);
    char buf[128] = {0};
    ssize_t n = read(fd_r, buf, sizeof(buf) - 1U);
    buf[n] = '\0';
    TEST_ASSERT_NOT_NULL(strstr(buf, "[00001]"));
    TEST_ASSERT_NOT_NULL(strstr(buf, "SENSOR"));
    close(fd_r);
}

void test_set_nonblocking_flag(void) {
    int32_t fd = posix_log_open(TEST_LOG_PATH);
    TEST_ASSERT_TRUE(fd >= 0);

    int32_t result = posix_set_nonblocking(fd);
    TEST_ASSERT_EQUAL_INT(0, result);

    /* Verify O_NONBLOCK is set */
    int32_t flags = fcntl(fd, F_GETFL, 0);
    TEST_ASSERT_TRUE((flags & O_NONBLOCK) != 0);
    posix_log_close(fd);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_write_all_basic);
    RUN_TEST(test_log_open_creates_file);
    RUN_TEST(test_log_write_formats_correctly);
    RUN_TEST(test_set_nonblocking_flag);
    return UNITY_END();
}
```

---

### Acceptance Criteria (Scoring)

- **[15%] Quality Gate Scaffolding**: Valid `.clang-tidy`, `.pre-commit-config.yaml`, and `CMakeLists.txt` with `CMAKE_C_CLANG_TIDY`, `cppcheck` target, `SANITIZER` cache option, and `_POSIX_C_SOURCE` definition.
- **[15%] Pre-commit & Static Analysis Gates**: `pre-commit run --all-files` and `cmake --build build --target cppcheck` execute with zero errors.
- **[15%] Sanitizers & Unit Tests Gate**: All Unity unit tests pass under ASan + UBSan (`ctest --test-dir build-asan -V`).
- **[15%] Code Documentation & Style**: Full Doxygen documentation on all functions/types, strict BARR-C naming (`p_buf`, `uint8_t`, `uint32_t`, mandatory braces).
- **[40%] Core Logic**: Correct `write_all()` partial-write loop with `EINTR` handling, proper `open()`/`close()` error checking, `fcntl()` non-blocking mode, stdio comparison in `main()`.

---

### Expected Output

```
=== Part A: Standard I/O (stdio) Log Writer ===
[stdio] Opened stdio_sensor.log with fopen()
[stdio] Entry [00001] written: temp=23.5C, humidity=55%
[stdio] Entry [00002] written: temp=24.1C, humidity=53%
[stdio] Entry [00003] written: temp=23.8C, humidity=56%
[stdio] Closed with fclose(). Buffered I/O complete.

=== Part B: POSIX System Call Log Writer ===
[posix] Opened posix_sensor.log with open() -> fd=3
[posix] Entry [00001] written (42 bytes via write_all): temp=23.5C, humidity=55%
[posix] Entry [00002] written (42 bytes via write_all): temp=24.1C, humidity=53%
[posix] Entry [00003] written (42 bytes via write_all): temp=23.8C, humidity=56%
[posix] Closed with close(). Raw system call I/O complete.

=== Part C: fcntl() Non-Blocking Demo ===
[fcntl] Set O_NONBLOCK on fd using fcntl(F_GETFL/F_SETFL)
[fcntl] Non-blocking mode enabled successfully.

=== Comparison Summary ===
stdio:  FILE * streams | Buffered by libc    | Portable (ISO C)
POSIX:  int fd         | Unbuffered (raw)    | Linux/macOS only (POSIX)
Note:   fopen() internally calls open(). They are layers, not alternatives.
```

### Expected Unit Test Output (L10+)

```
test_posix_logger.c:20:test_write_all_basic:PASS
test_posix_logger.c:34:test_log_open_creates_file:PASS
test_posix_logger.c:40:test_log_write_formats_correctly:PASS
test_posix_logger.c:56:test_set_nonblocking_flag:PASS
-----------------------
4 Tests 0 Failures 0 Ignored
OK
```

---

### Coding Standards Reference

**MISRA-C 2012 (Safety):**

| Rule | Category | Relevance to This Exercise |
|---|---|---|
| Directive 4.11 | Required | Validate values passed to library functions → validate `fd`, `p_buf`, and `p_path` parameters before use. |
| Directive 4.14 | Required | Validate values from external sources → check return values of `open()`, `write()`, `close()`, `fcntl()`. |
| Rule 15.5 | Advisory | Single point of exit → use a cleanup pattern with `goto cleanup` or structured single-return for error paths. |
| Rule 21.6 | Required | Standard I/O shall not be used (in safety-critical) → this exercise deliberately compares stdio and POSIX to show why POSIX is preferred in embedded Linux. |

**CERT-C 2016 (Security):**

| Rule | Relevance to This Exercise |
|---|---|
| ERR33-C | Detect and handle standard library errors → every `open()`, `read()`, `write()`, `close()`, `fcntl()` return value must be checked. |
| FIO02-C | Canonicalize path names from untrusted sources → if file path comes from user input, validate before passing to `open()`. |
| FIO42-C | Close files when they are no longer needed → prevent file descriptor leaks on all error paths. |
| EXP34-C | Do not dereference null pointers → validate `p_buf` and `p_path` before use. |

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs (under `C_Books/`) and read the full description of each rule above. After writing your code, verify your implementation follows these rules.

---

### Submission

```text
Exercise_1/
├── CMakeLists.txt              [PROVIDED STARTER / CONFIGURE] — Integrates clang-tidy, cppcheck, sanitizers, Unity
├── .clang-tidy                 [PROVIDED STARTER / VERIFY] — Static analysis rules
├── .pre-commit-config.yaml     [PROVIDED STARTER / VERIFY] — Pre-commit hook configuration
├── include/
│   └── posix_logger.h          [PROVIDED INTERFACE] — POSIX logger API prototypes
├── src/
│   ├── posix_logger.c          [MUST IMPLEMENT FROM SCRATCH] — POSIX I/O functions with error handling
│   └── main.c                  [MUST IMPLEMENT FROM SCRATCH] — stdio vs POSIX comparison demo
└── test/
    └── test_posix_logger.c     [MUST IMPLEMENT / EXTEND] — Unity unit test suite
```

---

## Exercise_2 [build] — Simulated Timer Peripheral Driver (CMSIS Macros & RMW)

### 📂 File Deliverables Breakdown

| File Path | Status | Description |
|---|---|---|
| `include/register_driver.h` | **[PROVIDED INTERFACE]** | Timer register struct, CMSIS-style macros (`_Pos`, `_Msk`, `_VAL2FLD`, `_FLD2VAL`), basic bitwise macros, and driver API prototypes. |
| `src/register_driver.c` | **[MUST IMPLEMENT FROM SCRATCH]** | Implement full timer driver lifecycle using volatile struct, CMSIS macros, and Read-Modify-Write pattern. |
| `src/main.c` | **[MUST IMPLEMENT FROM SCRATCH]** | Demo program: init → configure → start → simulate → read → stop. Print register state at each step. |
| `test/test_register_driver.c` | **[MUST IMPLEMENT / EXTEND]** | Unity unit test suite testing init, configure (prescaler + ARR), start/stop, RMW field preservation. |
| `CMakeLists.txt` | **[PROVIDED STARTER / CONFIGURE]** | Quality Gate CMake configuration integrating `clang-tidy`, `cppcheck`, `ASan+UBSan`, and `Unity`. |
| `.clang-tidy` | **[PROVIDED STARTER / VERIFY]** | Static analysis configuration file. |
| `.pre-commit-config.yaml` | **[PROVIDED STARTER / VERIFY]** | Pre-commit hook configuration file. |

---

### Problem Statement

You are writing a **bare-metal Timer peripheral driver** for an ARM Cortex-M microcontroller. Since you don't have the actual hardware on your desk, you'll use **simulated registers in memory** — the exact same technique professional teams use to unit-test peripheral drivers on CI servers without hardware (Section §4.8 of the lecture).

**Requirements:**

1. **Define basic bitwise macros** used throughout embedded C:
   ```c
   #define BIT(n)         (1UL << (n))
   #define SET_BIT(r, n)  ((r) |=  BIT(n))
   #define CLR_BIT(r, n)  ((r) &= ~BIT(n))
   #define GENMASK(h, l)  (((~0UL) << (l)) & (~0UL >> (31U - (h))))
   ```

2. **Define a `timer_reg_t` struct** with `volatile` fields mirroring a real timer peripheral:
   - `CR` (Control Register, R/W): bit 0 = `EN` (enable), bit 1 = `OPM` (one-pulse mode), bits [7:4] = `PSC` (prescaler, 0–15)
   - `SR` (Status Register, read-only in real HW): bit 0 = `UIF` (update interrupt flag)
   - `CNT` (Counter Register, R/W): current count value
   - `ARR` (Auto-Reload Register, R/W): reload value

3. **Define CMSIS-style macros** for every field following ARM's naming convention:
   ```c
   #define TIMER_CR_EN_Pos    (0U)
   #define TIMER_CR_EN_Msk    (0x1UL << TIMER_CR_EN_Pos)
   /* ... repeat for OPM, PSC, UIF ... */
   ```
   Also implement the generic helpers `_VAL2FLD(field, value)` and `_FLD2VAL(field, reg)`.

4. **Implement the following driver functions using the Read-Modify-Write (RMW) pattern:**
   - `timer_init(timer_reg_t *p_timer)` — zero all registers.
   - `timer_configure(timer_reg_t *p_timer, uint32_t prescaler, uint32_t reload)` — set the PSC field in CR using RMW and set ARR. Validate `prescaler <= 15`.
   - `timer_start(timer_reg_t *p_timer)` — set the EN bit in CR (preserve other bits).
   - `timer_stop(timer_reg_t *p_timer)` — clear the EN bit in CR (preserve other bits).
   - `timer_read_count(const timer_reg_t *p_timer)` — return current CNT value.
   - `timer_is_event(const timer_reg_t *p_timer)` — check if UIF bit is set in SR.
   - `timer_clear_event(timer_reg_t *p_timer)` — clear the UIF bit in SR.

5. **In `main()`**, create a simulated timer, run through the full lifecycle (init → configure → start → simulate count → check event → clear event → stop), and print register state at each step showing hex values.

6. **After stopping**, verify that the RMW pattern preserved all non-target fields (PSC should still be set after clearing EN).

> [!IMPORTANT]
> **Why Use Read-Modify-Write (RMW) Instead of Direct `|=` and `&=` on Registers?**
>
> On `volatile` registers, `*p_reg |= BIT(5)` actually generates a Read-Modify-Write sequence too — the CPU reads the register, ORs in the bit, and writes back. However, when you need to modify **multiple fields** in one register, doing `*p_reg |= X; *p_reg &= ~Y;` generates **two separate** RMW cycles (two reads + two writes to the peripheral bus).
>
> **Problems with multiple direct `|=`/`&=` on volatile:**
> 1. **Intermediate states**: The peripheral sees the register after the first write but before the second. It may act on the partially-configured state (e.g., enabling a timer before its prescaler is set).
> 2. **Efficiency**: Each `volatile` access forces a hardware bus transaction. Two accesses = two bus cycles instead of one.
> 3. **Read-side-effect registers**: Some status registers **clear their flags on read**. A second read (from the second `|=`) may lose flag data that was set between the two accesses.
>
> **The RMW pattern with a `temp` variable solves all three:**
> ```c
> uint32_t temp = p_timer->CR;             /* ONE read from hardware  */
> temp &= ~TIMER_CR_PSC_Msk;              /* Modify in CPU register  */
> temp |= _VAL2FLD(TIMER_CR_PSC, 8U);     /* Modify in CPU register  */
> p_timer->CR = temp;                      /* ONE write to hardware   */
> ```

> [!NOTE]
> **The `volatile`/`const` Hardware Access Matrix**
>
> | Declaration | Read? | Write? | Use Case |
> |:---|:---|:---|:---|
> | `volatile uint32_t reg` | ✅ | ✅ | Read/Write registers (CR, CNT, ARR) |
> | `volatile const uint32_t reg` | ✅ | ❌ | Read-only status registers (SR on real HW) |
> | `volatile uint32_t reg` + comment | ✅* | ✅ | Write-only registers (BSRR — C cannot express write-only) |
>
> In this exercise, use `volatile uint32_t` for SR since we need to simulate writes in tests. In real hardware, SR would be `volatile const uint32_t`. Add a comment noting this difference.

**Quality Gate Rules:**
- **Local Pre-commit Gate**: Run `pre-commit run --all-files` — must pass cleanly.
- **Build & Clang-Tidy Gate**: Run `cmake -B build -DCMAKE_BUILD_TYPE=Debug && cmake --build build` — must compile with `-Wall -Wextra -pedantic -Werror -std=c11` and pass `clang-tidy` with 0 warnings.
- **Static Analysis Gate**: Run `cmake --build build --target cppcheck` — must report 0 defects.
- **Sanitizer & Test Gate**: Run `cmake -B build-asan -DSANITIZER=asan+ubsan -DCMAKE_BUILD_TYPE=Debug && cmake --build build-asan && ctest --test-dir build-asan -V` — all Unity unit tests must pass with 0 sanitizer violations.
- **Coding Style & Documentation**: Follow BARR-C coding standard (`p_timer`, `uint32_t`, mandatory braces) and document all functions/structures with Doxygen comments.

---

### Design Hints (optional)

**1. Register Driver Header (`include/register_driver.h`):**
```c
#ifndef REGISTER_DRIVER_H
#define REGISTER_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

/* ================================================================
 * Basic Bitwise Macros (common in embedded C)
 * ================================================================ */

#define BIT(n)         (1UL << (n))
#define SET_BIT(r, n)  ((r) |=  BIT(n))
#define CLR_BIT(r, n)  ((r) &= ~BIT(n))
#define GENMASK(h, l)  (((~0UL) << (l)) & (~0UL >> (31U - (h))))

/* ================================================================
 * CMSIS-Style Generic Helpers
 * ================================================================ */

/** @brief Insert a value into a register field's position. */
#define _VAL2FLD(field, value) \
    (((uint32_t)(value) << field##_Pos) & field##_Msk)

/** @brief Extract a field's value from a register. */
#define _FLD2VAL(field, reg) \
    (((uint32_t)(reg) & field##_Msk) >> field##_Pos)

/* ================================================================
 * Timer Peripheral Register Field Definitions (CMSIS-style)
 * ================================================================ */

/* CR — Control Register */
#define TIMER_CR_EN_Pos     (0U)
#define TIMER_CR_EN_Msk     (0x1UL << TIMER_CR_EN_Pos)     /* Bit 0: Enable      */

#define TIMER_CR_OPM_Pos    (1U)
#define TIMER_CR_OPM_Msk    (0x1UL << TIMER_CR_OPM_Pos)    /* Bit 1: One-pulse   */

#define TIMER_CR_PSC_Pos    (4U)
#define TIMER_CR_PSC_Msk    (0xFUL << TIMER_CR_PSC_Pos)    /* Bits [7:4]: Prescaler (0–15) */

/* SR — Status Register */
#define TIMER_SR_UIF_Pos    (0U)
#define TIMER_SR_UIF_Msk    (0x1UL << TIMER_SR_UIF_Pos)    /* Bit 0: Update event flag */

/* ================================================================
 * Timer Register Struct (mirrors hardware layout)
 * ================================================================ */

/**
 * @brief Simulated Timer peripheral register block.
 *
 * On real hardware, this struct would be mapped to a fixed base address
 * using: #define TIM2 ((timer_reg_t *)0x40000000UL)
 *
 * For unit testing on a host PC, we allocate this struct in memory.
 */
typedef struct timer_reg_s {
    volatile uint32_t CR;    /**< Offset 0x00: Control Register (R/W).         */
    volatile uint32_t SR;    /**< Offset 0x04: Status Register (R/O on real HW). */
    volatile uint32_t CNT;   /**< Offset 0x08: Counter Register (R/W).         */
    volatile uint32_t ARR;   /**< Offset 0x0C: Auto-Reload Register (R/W).     */
} timer_reg_t;

/* ================================================================
 * Driver API Prototypes
 * ================================================================ */

void     timer_init(timer_reg_t *p_timer);
int32_t  timer_configure(timer_reg_t *p_timer, uint32_t prescaler,
                         uint32_t reload);
void     timer_start(timer_reg_t *p_timer);
void     timer_stop(timer_reg_t *p_timer);
uint32_t timer_read_count(const timer_reg_t *p_timer);
bool     timer_is_event(const timer_reg_t *p_timer);
void     timer_clear_event(timer_reg_t *p_timer);

#endif /* REGISTER_DRIVER_H */
```

**2. Quality Gate CMake Template (`CMakeLists.txt`):**
```cmake
cmake_minimum_required(VERSION 3.16)
project(exercise_2 VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Option: Sanitizers
set(SANITIZER "none" CACHE STRING "Sanitizer (none|asan|ubsan|asan+ubsan)")
set_property(CACHE SANITIZER PROPERTY STRINGS none asan ubsan "asan+ubsan")

# Gate 1: clang-tidy integration
find_program(CLANG_TIDY_EXE NAMES clang-tidy)
if(CLANG_TIDY_EXE)
    set(CMAKE_C_CLANG_TIDY ${CLANG_TIDY_EXE} --header-filter=${CMAKE_SOURCE_DIR}/include)
    message(STATUS "[Quality Gate] clang-tidy: ENABLED")
endif()

# Gate 2: cppcheck custom target
find_program(CPPCHECK_EXE NAMES cppcheck)
if(CPPCHECK_EXE)
    add_custom_target(cppcheck
        COMMAND ${CPPCHECK_EXE}
            --enable=all --inconclusive --std=c11
            --suppress=missingIncludeSystem
            --project=${CMAKE_BINARY_DIR}/compile_commands.json
            --error-exitcode=1
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "[Quality Gate] Running cppcheck..."
    )
endif()

# Library & Main Executable
add_library(register_driver STATIC src/register_driver.c)
target_include_directories(register_driver PUBLIC include)
target_compile_options(register_driver PRIVATE -Wall -Wextra -pedantic -Werror)

add_executable(exercise_2 src/main.c)
target_link_libraries(exercise_2 PRIVATE register_driver)
target_compile_options(exercise_2 PRIVATE -Wall -Wextra -pedantic -Werror)

# Apply Sanitizers
if(SANITIZER STREQUAL "asan+ubsan")
    target_compile_options(register_driver PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(register_driver PUBLIC -fsanitize=address,undefined)
    target_compile_options(exercise_2 PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(exercise_2 PUBLIC -fsanitize=address,undefined)
endif()

# Gate 3: Unity Unit Tests via FetchContent
include(FetchContent)
FetchContent_Declare(
    unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG        v2.6.0
)
FetchContent_MakeAvailable(unity)
set_target_properties(unity PROPERTIES C_CLANG_TIDY "")

enable_testing()
add_executable(test_exercise_2 test/test_register_driver.c)
target_link_libraries(test_exercise_2 PRIVATE register_driver unity)
target_compile_options(test_exercise_2 PRIVATE -Wall -Wextra -pedantic -Werror)
if(SANITIZER STREQUAL "asan+ubsan")
    target_compile_options(test_exercise_2 PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(test_exercise_2 PUBLIC -fsanitize=address,undefined)
endif()
add_test(NAME test_exercise_2 COMMAND test_exercise_2)
```

**3. Unity Test Skeleton (`test/test_register_driver.c`):**
```c
#include "unity.h"
#include "register_driver.h"

static timer_reg_t g_timer;

void setUp(void) {
    timer_init(&g_timer);
}

void tearDown(void) {}

void test_timer_init_zeros_all(void) {
    TEST_ASSERT_EQUAL_HEX32(0x00000000UL, g_timer.CR);
    TEST_ASSERT_EQUAL_HEX32(0x00000000UL, g_timer.SR);
    TEST_ASSERT_EQUAL_HEX32(0x00000000UL, g_timer.CNT);
    TEST_ASSERT_EQUAL_HEX32(0x00000000UL, g_timer.ARR);
}

void test_timer_configure_prescaler(void) {
    timer_configure(&g_timer, 8U, 1000U);
    uint32_t psc = _FLD2VAL(TIMER_CR_PSC, g_timer.CR);
    TEST_ASSERT_EQUAL_UINT32(8U, psc);
}

void test_timer_configure_reload(void) {
    timer_configure(&g_timer, 4U, 5000U);
    TEST_ASSERT_EQUAL_UINT32(5000U, g_timer.ARR);
}

void test_timer_start_sets_enable(void) {
    timer_start(&g_timer);
    TEST_ASSERT_TRUE((g_timer.CR & TIMER_CR_EN_Msk) != 0U);
}

void test_timer_stop_clears_enable(void) {
    timer_start(&g_timer);
    timer_stop(&g_timer);
    TEST_ASSERT_TRUE((g_timer.CR & TIMER_CR_EN_Msk) == 0U);
}

void test_timer_read_count(void) {
    g_timer.CNT = 500U;  /* Simulate hardware updating the counter */
    TEST_ASSERT_EQUAL_UINT32(500U, timer_read_count(&g_timer));
}

void test_rmw_preserves_other_fields(void) {
    /* Configure prescaler=8 and start timer */
    timer_configure(&g_timer, 8U, 1000U);
    timer_start(&g_timer);
    /* Stop timer — PSC field must be preserved */
    timer_stop(&g_timer);
    uint32_t psc_after = _FLD2VAL(TIMER_CR_PSC, g_timer.CR);
    TEST_ASSERT_EQUAL_UINT32(8U, psc_after);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_timer_init_zeros_all);
    RUN_TEST(test_timer_configure_prescaler);
    RUN_TEST(test_timer_configure_reload);
    RUN_TEST(test_timer_start_sets_enable);
    RUN_TEST(test_timer_stop_clears_enable);
    RUN_TEST(test_timer_read_count);
    RUN_TEST(test_rmw_preserves_other_fields);
    return UNITY_END();
}
```

---

### Acceptance Criteria (Scoring)

- **[15%] Quality Gate Scaffolding**: Valid `.clang-tidy`, `.pre-commit-config.yaml`, and `CMakeLists.txt` with `CMAKE_C_CLANG_TIDY`, `cppcheck` target, and `SANITIZER` cache option.
- **[15%] Pre-commit & Static Analysis Gates**: `pre-commit run --all-files` and `cmake --build build --target cppcheck` execute with zero errors.
- **[15%] Sanitizers & Unit Tests Gate**: All Unity unit tests pass under ASan + UBSan (`ctest --test-dir build-asan -V`).
- **[15%] Code Documentation & Style**: Full Doxygen documentation on all functions/types/macros, strict BARR-C naming (`p_timer`, `uint32_t`, mandatory braces).
- **[40%] Core Logic**: Correct CMSIS macro definitions, proper Read-Modify-Write implementation using temp variable, `_VAL2FLD`/`_FLD2VAL` field manipulation, and RMW field-preservation proof in the demo.

---

### Expected Output

```
=== Timer Peripheral Driver Demo ===

--- Step 1: Initialize ---
timer_init(): All registers reset to 0x00000000
  CR=0x00000000  SR=0x00000000  CNT=0x00000000  ARR=0x00000000

--- Step 2: Configure (prescaler=8, reload=1000) ---
timer_configure(): Using Read-Modify-Write pattern
  RMW: Read  CR = 0x00000000
  RMW: Clear PSC field [7:4], insert PSC=8
  RMW: Write CR = 0x00000080
  ARR set to 1000 (0x000003E8)
  CR=0x00000080  ARR=0x000003E8

--- Step 3: Start Timer ---
timer_start(): SET bit CR.EN
  CR=0x00000081 (EN=1, PSC=8)

--- Step 4: Simulate Count & Read ---
  CNT=500 (simulated hardware update)
  timer_read_count() = 500

--- Step 5: Simulate Update Event ---
  SR=0x00000001 (UIF=1, simulated)
  timer_is_event() = true
  timer_clear_event(): CLR bit SR.UIF
  SR=0x00000000 (UIF=0)

--- Step 6: Stop Timer ---
timer_stop(): CLR bit CR.EN
  CR=0x00000080 (EN=0, PSC=8 preserved)

--- Step 7: Verify RMW Preserved Other Fields ---
  PSC field after stop: 8 (PRESERVED by RMW)
  OPM field after stop: 0 (PRESERVED by RMW)
  PASS: Read-Modify-Write correctly preserved all non-target fields.
```

### Expected Unit Test Output (L10+)

```
test_register_driver.c:14:test_timer_init_zeros_all:PASS
test_register_driver.c:21:test_timer_configure_prescaler:PASS
test_register_driver.c:27:test_timer_configure_reload:PASS
test_register_driver.c:32:test_timer_start_sets_enable:PASS
test_register_driver.c:38:test_timer_stop_clears_enable:PASS
test_register_driver.c:43:test_timer_read_count:PASS
test_register_driver.c:49:test_rmw_preserves_other_fields:PASS
-----------------------
7 Tests 0 Failures 0 Ignored
OK
```

---

### Coding Standards Reference

**MISRA-C 2012 (Safety):**

| Rule | Category | Relevance to This Exercise |
|---|---|---|
| Rule 10.1 | Required | Operands shall not be of an inappropriate essential type → bitwise operators (`|`, `&`, `~`, `<<`, `>>`) must only be applied to **unsigned** types (`uint32_t`). Never shift or mask signed integers. |
| Rule 11.4 | Required | Integer-to-pointer conversion → casting `0x40000000UL` to `timer_reg_t *` is necessary for MMIO but requires a **deviation justification**. In this exercise, we use memory buffers, so no deviation needed. |
| Rule 11.6 | Required | No `void *` to arithmetic cast → use explicit `(timer_reg_t *)` casts, not `void *`. |
| Rule 12.2 | Required | Shift amount within valid range → shift amounts must be non-negative and less than bit width (32). Validate `prescaler` fits in 4 bits. |
| Directive 4.11 | Required | Validate values passed to library functions → validate `p_timer != NULL` and `prescaler <= 15U` before use. |

**CERT-C 2016 (Security):**

| Rule | Relevance to This Exercise |
|---|---|
| INT34-C | Do not shift by a negative number or by >= bit width → validate shift amounts in macros and functions. |
| EXP34-C | Do not dereference null pointers → validate `p_timer` in all driver functions. |
| EXP30-C | Do not depend on order of evaluation of side effects → avoid compound expressions on `volatile` registers (use RMW with temp variable). |
| INT30-C | Ensure unsigned operations do not wrap → validate prescaler fits in 4-bit field before insertion. |

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs (under `C_Books/`) and read the full description of each rule above. After writing your code, verify your implementation follows these rules.

---

### Submission

```text
Exercise_2/
├── CMakeLists.txt              [PROVIDED STARTER / CONFIGURE] — Integrates clang-tidy, cppcheck, sanitizers, Unity
├── .clang-tidy                 [PROVIDED STARTER / VERIFY] — Static analysis rules
├── .pre-commit-config.yaml     [PROVIDED STARTER / VERIFY] — Pre-commit hook configuration
├── include/
│   └── register_driver.h      [PROVIDED INTERFACE] — Timer struct, CMSIS macros, bitwise macros, API prototypes
├── src/
│   ├── register_driver.c      [MUST IMPLEMENT FROM SCRATCH] — Full driver lifecycle with RMW pattern
│   └── main.c                 [MUST IMPLEMENT FROM SCRATCH] — Demo with register-state printing
└── test/
    └── test_register_driver.c [MUST IMPLEMENT / EXTEND] — Unity unit test suite
```

---

## Exercise_3 [build] — Device File Reader with Bitwise Parsing (Bridging Both Worlds)

### 📂 File Deliverables Breakdown

| File Path | Status | Description |
|---|---|---|
| `include/device_file_io.h` | **[PROVIDED INTERFACE]** | Register field layout macros (`FIELD_GET`, `FIELD_PREP`), register snapshot write/read/parse API prototypes. |
| `src/device_file_io.c` | **[MUST IMPLEMENT FROM SCRATCH]** | Implement binary file write (`open`/`write`), binary file read (`open`/`read`), and bitwise field extraction using `FIELD_GET` macros. |
| `src/main.c` | **[MUST IMPLEMENT FROM SCRATCH]** | Demo program: create binary file, re-read it, parse and print all fields from each register snapshot. |
| `test/test_device_file_io.c` | **[MUST IMPLEMENT / EXTEND]** | Unity unit test suite testing `FIELD_GET` extraction and write/read round-trip integrity. |
| `CMakeLists.txt` | **[PROVIDED STARTER / CONFIGURE]** | Quality Gate CMake configuration integrating `clang-tidy`, `cppcheck`, `ASan+UBSan`, and `Unity`. |
| `.clang-tidy` | **[PROVIDED STARTER / VERIFY]** | Static analysis configuration file. |
| `.pre-commit-config.yaml` | **[PROVIDED STARTER / VERIFY]** | Pre-commit hook configuration file. |

---

### Problem Statement

In embedded Linux, hardware peripherals appear as **device files** under `/dev/`. When you `open()` and `read()` from a device file, you often receive raw binary data (register snapshots, sensor readings, DMA buffers) that must be parsed using **bitwise field extraction** — the same technique used in bare-metal MCU drivers.

This exercise bridges **both worlds** from the lecture: POSIX file I/O (§1) + bitwise register parsing (§3).

**Scenario:** Your IoT gateway periodically snapshots the registers of a connected sensor controller and saves them to a binary log file. You need to write a tool that:
1. **Creates** a binary file containing 4 simulated register snapshots.
2. **Reads** them back using POSIX system calls.
3. **Parses** each 32-bit register value to extract packed fields.

**Register Field Layout (each register is `uint32_t`):**

| Bit Range | Field Name | Size | Description |
|:---|:---|:---|:---|
| [0] | `ENABLED` | 1 bit | Sensor enabled flag (0=off, 1=on) |
| [3:1] | `MODE` | 3 bits | Operating mode (0–7) |
| [15:8] | `THRESHOLD` | 8 bits | Alert threshold value (0–255) |
| [31:16] | `SENSOR_RAW` | 16 bits | Raw sensor reading (0–65535) |

**Requirements:**

1. **Define `FIELD_GET()` and `FIELD_PREP()` macros** using the `_Pos` and `_Msk` convention taught in Section §3.3:
   ```c
   #define FIELD_GET(field, reg) \
       (((uint32_t)(reg) & field##_Msk) >> field##_Pos)

   #define FIELD_PREP(field, val) \
       (((uint32_t)(val) << field##_Pos) & field##_Msk)
   ```
   Define the corresponding `_Pos` and `_Msk` constants for each field (`REG_ENABLED`, `REG_MODE`, `REG_THRESHOLD`, `REG_SENSOR_RAW`).

2. **Implement `reg_snapshot_write()`** — Creates a binary file using `open()` with `O_WRONLY | O_CREAT | O_TRUNC`, writes an array of `uint32_t` register values using `write()`, and closes the file. Handle all errors.

3. **Implement `reg_snapshot_read()`** — Opens the binary file with `open()` (`O_RDONLY`), reads all register values using `read()` into a provided buffer, and closes the file. Returns the number of registers read. Handle partial reads.

4. **Implement `reg_snapshot_parse()`** — Takes a single `uint32_t` register value and extracts all four fields (`ENABLED`, `MODE`, `THRESHOLD`, `SENSOR_RAW`) into an output struct using your `FIELD_GET()` macros.

5. **In `main()`**, demonstrate the full pipeline:
   - Create 4 test register values with known field contents.
   - Write them to `registers.bin` using `reg_snapshot_write()`.
   - Read them back using `reg_snapshot_read()`.
   - Parse and print each register's fields using `reg_snapshot_parse()`.

6. All error paths must be handled. Close file descriptors on every exit path.

> [!TIP]
> **Real-World Context: When Would You Do This?**
>
> On an embedded Linux system, this is exactly how you'd process raw data from:
> - `/dev/spidev0.0` — SPI register reads from an external sensor IC
> - `/dev/i2c-1` — I2C register dumps from a peripheral
> - `/sys/bus/iio/devices/iio:device0/...` — Industrial I/O subsystem raw values
> - Binary log files from a data acquisition daemon
>
> The pattern is always the same: `open()` → `read()` raw binary → extract bitfields → interpret.

**Quality Gate Rules:**
- **Local Pre-commit Gate**: Run `pre-commit run --all-files` — must pass cleanly.
- **Build & Clang-Tidy Gate**: Run `cmake -B build -DCMAKE_BUILD_TYPE=Debug && cmake --build build` — must compile with `-Wall -Wextra -pedantic -Werror -std=c11` and pass `clang-tidy` with 0 warnings.
- **Static Analysis Gate**: Run `cmake --build build --target cppcheck` — must report 0 defects.
- **Sanitizer & Test Gate**: Run `cmake -B build-asan -DSANITIZER=asan+ubsan -DCMAKE_BUILD_TYPE=Debug && cmake --build build-asan && ctest --test-dir build-asan -V` — all Unity unit tests must pass with 0 sanitizer violations.
- **Coding Style & Documentation**: Follow BARR-C coding standard and document all functions/structures with Doxygen comments.

---

### Design Hints (optional)

**1. Device File I/O Interface (`include/device_file_io.h`):**
```c
#ifndef DEVICE_FILE_IO_H
#define DEVICE_FILE_IO_H

#include <stdint.h>

/* ================================================================
 * Register Field Definitions (bit positions and masks)
 * ================================================================ */

/* ENABLED: Bit [0] */
#define REG_ENABLED_Pos     (0U)
#define REG_ENABLED_Msk     (0x1UL << REG_ENABLED_Pos)

/* MODE: Bits [3:1] */
#define REG_MODE_Pos        (1U)
#define REG_MODE_Msk        (0x7UL << REG_MODE_Pos)

/* THRESHOLD: Bits [15:8] */
#define REG_THRESHOLD_Pos   (8U)
#define REG_THRESHOLD_Msk   (0xFFUL << REG_THRESHOLD_Pos)

/* SENSOR_RAW: Bits [31:16] */
#define REG_SENSOR_RAW_Pos  (16U)
#define REG_SENSOR_RAW_Msk  (0xFFFFUL << REG_SENSOR_RAW_Pos)

/* ================================================================
 * Generic Field Extraction / Insertion Macros
 * ================================================================ */

/** @brief Extract a field value from a register (Linux kernel FIELD_GET style). */
#define FIELD_GET(field, reg) \
    (((uint32_t)(reg) & field##_Msk) >> field##_Pos)

/** @brief Prepare a value for insertion into a field (Linux kernel FIELD_PREP style). */
#define FIELD_PREP(field, val) \
    (((uint32_t)(val) << field##_Pos) & field##_Msk)

/* ================================================================
 * Parsed Register Output Struct
 * ================================================================ */

/**
 * @brief Parsed fields from a single register snapshot.
 */
typedef struct reg_fields_s {
    uint32_t enabled;     /**< Bit [0]: 0=off, 1=on.           */
    uint32_t mode;        /**< Bits [3:1]: Operating mode 0–7. */
    uint32_t threshold;   /**< Bits [15:8]: Alert threshold.   */
    uint32_t sensor_raw;  /**< Bits [31:16]: Raw ADC reading.  */
} reg_fields_t;

/* ================================================================
 * API Prototypes
 * ================================================================ */

#define MAX_SNAPSHOTS 16U

/**
 * @brief Write an array of register snapshots to a binary file.
 * @return 0 on success, -1 on error.
 */
int32_t reg_snapshot_write(const char *p_path, const uint32_t *p_regs,
                           uint32_t count);

/**
 * @brief Read register snapshots from a binary file.
 * @return Number of registers read (>= 0), -1 on error.
 */
int32_t reg_snapshot_read(const char *p_path, uint32_t *p_regs,
                          uint32_t max_count);

/**
 * @brief Parse a raw register value into individual fields.
 */
void reg_snapshot_parse(uint32_t raw_reg, reg_fields_t *p_out);

#endif /* DEVICE_FILE_IO_H */
```

**2. Quality Gate CMake Template (`CMakeLists.txt`):**
```cmake
cmake_minimum_required(VERSION 3.16)
project(exercise_3 VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# POSIX feature test macro — required for open(), read(), write(), close()
add_compile_definitions(_POSIX_C_SOURCE=200809L)

# Option: Sanitizers
set(SANITIZER "none" CACHE STRING "Sanitizer (none|asan|ubsan|asan+ubsan)")
set_property(CACHE SANITIZER PROPERTY STRINGS none asan ubsan "asan+ubsan")

# Gate 1: clang-tidy integration
find_program(CLANG_TIDY_EXE NAMES clang-tidy)
if(CLANG_TIDY_EXE)
    set(CMAKE_C_CLANG_TIDY ${CLANG_TIDY_EXE} --header-filter=${CMAKE_SOURCE_DIR}/include)
    message(STATUS "[Quality Gate] clang-tidy: ENABLED")
endif()

# Gate 2: cppcheck custom target
find_program(CPPCHECK_EXE NAMES cppcheck)
if(CPPCHECK_EXE)
    add_custom_target(cppcheck
        COMMAND ${CPPCHECK_EXE}
            --enable=all --inconclusive --std=c11
            --suppress=missingIncludeSystem
            --project=${CMAKE_BINARY_DIR}/compile_commands.json
            --error-exitcode=1
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "[Quality Gate] Running cppcheck..."
    )
endif()

# Library & Main Executable
add_library(device_file_io STATIC src/device_file_io.c)
target_include_directories(device_file_io PUBLIC include)
target_compile_options(device_file_io PRIVATE -Wall -Wextra -pedantic -Werror)

add_executable(exercise_3 src/main.c)
target_link_libraries(exercise_3 PRIVATE device_file_io)
target_compile_options(exercise_3 PRIVATE -Wall -Wextra -pedantic -Werror)

# Apply Sanitizers
if(SANITIZER STREQUAL "asan+ubsan")
    target_compile_options(device_file_io PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(device_file_io PUBLIC -fsanitize=address,undefined)
    target_compile_options(exercise_3 PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(exercise_3 PUBLIC -fsanitize=address,undefined)
endif()

# Gate 3: Unity Unit Tests via FetchContent
include(FetchContent)
FetchContent_Declare(
    unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG        v2.6.0
)
FetchContent_MakeAvailable(unity)
set_target_properties(unity PROPERTIES C_CLANG_TIDY "")

enable_testing()
add_executable(test_exercise_3 test/test_device_file_io.c)
target_link_libraries(test_exercise_3 PRIVATE device_file_io unity)
target_compile_options(test_exercise_3 PRIVATE -Wall -Wextra -pedantic -Werror)
if(SANITIZER STREQUAL "asan+ubsan")
    target_compile_options(test_exercise_3 PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(test_exercise_3 PUBLIC -fsanitize=address,undefined)
endif()
add_test(NAME test_exercise_3 COMMAND test_exercise_3)
```

**3. Unity Test Skeleton (`test/test_device_file_io.c`):**
```c
#include "unity.h"
#include "device_file_io.h"
#include <unistd.h>

static const char *TEST_BIN_PATH = "/tmp/test_registers.bin";

void setUp(void) {
    unlink(TEST_BIN_PATH);
}

void tearDown(void) {
    unlink(TEST_BIN_PATH);
}

void test_field_get_enabled(void) {
    uint32_t reg = 0x00000005UL;  /* Bit 0 = 1 */
    reg_fields_t fields;
    reg_snapshot_parse(reg, &fields);
    TEST_ASSERT_EQUAL_UINT32(1U, fields.enabled);
}

void test_field_get_mode(void) {
    /* Mode = 5 → bits [3:1] = 101 → byte 0 = 0b00001010 = 0x0A */
    uint32_t reg = FIELD_PREP(REG_MODE, 5U);
    reg_fields_t fields;
    reg_snapshot_parse(reg, &fields);
    TEST_ASSERT_EQUAL_UINT32(5U, fields.mode);
}

void test_field_get_threshold(void) {
    uint32_t reg = FIELD_PREP(REG_THRESHOLD, 200U);
    reg_fields_t fields;
    reg_snapshot_parse(reg, &fields);
    TEST_ASSERT_EQUAL_UINT32(200U, fields.threshold);
}

void test_field_get_sensor(void) {
    uint32_t reg = FIELD_PREP(REG_SENSOR_RAW, 43981U);  /* 0xABCD */
    reg_fields_t fields;
    reg_snapshot_parse(reg, &fields);
    TEST_ASSERT_EQUAL_UINT32(43981U, fields.sensor_raw);
}

void test_write_read_roundtrip(void) {
    uint32_t regs_out[4] = {0x1A3C0B05UL, 0x5678FF02UL,
                            0x00001201UL, 0xABCD6409UL};
    int32_t ret = reg_snapshot_write(TEST_BIN_PATH, regs_out, 4U);
    TEST_ASSERT_EQUAL_INT32(0, ret);

    uint32_t regs_in[4] = {0};
    int32_t count = reg_snapshot_read(TEST_BIN_PATH, regs_in, 4U);
    TEST_ASSERT_EQUAL_INT32(4, count);

    TEST_ASSERT_EQUAL_HEX32(regs_out[0], regs_in[0]);
    TEST_ASSERT_EQUAL_HEX32(regs_out[1], regs_in[1]);
    TEST_ASSERT_EQUAL_HEX32(regs_out[2], regs_in[2]);
    TEST_ASSERT_EQUAL_HEX32(regs_out[3], regs_in[3]);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_field_get_enabled);
    RUN_TEST(test_field_get_mode);
    RUN_TEST(test_field_get_threshold);
    RUN_TEST(test_field_get_sensor);
    RUN_TEST(test_write_read_roundtrip);
    return UNITY_END();
}
```

---

### Acceptance Criteria (Scoring)

- **[15%] Quality Gate Scaffolding**: Valid `.clang-tidy`, `.pre-commit-config.yaml`, and `CMakeLists.txt` with `CMAKE_C_CLANG_TIDY`, `cppcheck` target, `SANITIZER` cache option, and `_POSIX_C_SOURCE` definition.
- **[15%] Pre-commit & Static Analysis Gates**: `pre-commit run --all-files` and `cmake --build build --target cppcheck` execute with zero errors.
- **[15%] Sanitizers & Unit Tests Gate**: All Unity unit tests pass under ASan + UBSan (`ctest --test-dir build-asan -V`).
- **[15%] Code Documentation & Style**: Full Doxygen documentation on all functions/types/macros, strict BARR-C naming (`p_regs`, `p_out`, `uint32_t`, mandatory braces).
- **[40%] Core Logic**: Correct `FIELD_GET`/`FIELD_PREP` macros, proper `open()`/`write()`/`read()`/`close()` binary file I/O with error handling, and accurate field extraction matching the defined bit layout.

---

### Expected Output

```
=== Device File Reader with Bitwise Parsing ===

--- Step 1: Creating binary register snapshot file ---
Writing 4 simulated registers to registers.bin...
  REG[0] = 0x1A3C0B05 (written)
  REG[1] = 0x5678FF02 (written)
  REG[2] = 0x00001201 (written)
  REG[3] = 0xABCD6409 (written)
Write complete (16 bytes).

--- Step 2: Reading register snapshots ---
Reading registers.bin with open()/read()...
Read 16 bytes (4 registers).

--- Step 3: Parsing register fields ---
Register[0] = 0x1A3C0B05:
  Bit[0]     Enabled:    YES
  Bits[3:1]  Mode:       2
  Bits[15:8] Threshold:  11
  Bits[31:16] Sensor:    6716

Register[1] = 0x5678FF02:
  Bit[0]     Enabled:    NO
  Bits[3:1]  Mode:       1
  Bits[15:8] Threshold:  255
  Bits[31:16] Sensor:    22136

Register[2] = 0x00001201:
  Bit[0]     Enabled:    YES
  Bits[3:1]  Mode:       0
  Bits[15:8] Threshold:  18
  Bits[31:16] Sensor:    0

Register[3] = 0xABCD6409:
  Bit[0]     Enabled:    YES
  Bits[3:1]  Mode:       4
  Bits[15:8] Threshold:  100
  Bits[31:16] Sensor:    43981

=== All register snapshots parsed successfully ===
```

### Expected Unit Test Output (L10+)

```
test_device_file_io.c:17:test_field_get_enabled:PASS
test_device_file_io.c:24:test_field_get_mode:PASS
test_device_file_io.c:32:test_field_get_threshold:PASS
test_device_file_io.c:39:test_field_get_sensor:PASS
test_device_file_io.c:46:test_write_read_roundtrip:PASS
-----------------------
5 Tests 0 Failures 0 Ignored
OK
```

---

### Coding Standards Reference

**MISRA-C 2012 (Safety):**

| Rule | Category | Relevance to This Exercise |
|---|---|---|
| Rule 10.1 | Required | Bitwise operators on unsigned types only → all `FIELD_GET`/`FIELD_PREP` macros must operate on `uint32_t`. |
| Rule 12.2 | Required | Shift amount within valid range → shift positions in macros must be less than 32. |
| Directive 4.11 | Required | Validate values passed to library functions → validate `p_regs`, `p_path`, `p_out` before use. |
| Directive 4.14 | Required | Validate values from external sources → check return values of `open()`, `read()`, `write()`, `close()`. |
| Rule 18.1 | Required | Pointer arithmetic within array bounds → ensure `read()` buffer size matches expected data. |

**CERT-C 2016 (Security):**

| Rule | Relevance to This Exercise |
|---|---|
| ERR33-C | Detect and handle standard library errors → check all `open()`/`read()`/`write()`/`close()` return values. |
| FIO42-C | Close files when no longer needed → close file descriptors on all exit paths (including error paths). |
| INT34-C | Do not shift by negative or >= bit width → validate shift amounts in field extraction macros. |
| EXP34-C | Do not dereference null pointers → validate `p_regs` and `p_out` in all functions. |
| ARR30-C | Do not form out-of-bounds pointers → ensure `read()` buffer has enough space for all registers. |

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs (under `C_Books/`) and read the full description of each rule above. After writing your code, verify your implementation follows these rules.

---

### Submission

```text
Exercise_3/
├── CMakeLists.txt              [PROVIDED STARTER / CONFIGURE] — Integrates clang-tidy, cppcheck, sanitizers, Unity
├── .clang-tidy                 [PROVIDED STARTER / VERIFY] — Static analysis rules
├── .pre-commit-config.yaml     [PROVIDED STARTER / VERIFY] — Pre-commit hook configuration
├── include/
│   └── device_file_io.h       [PROVIDED INTERFACE] — Field macros, parsed struct, API prototypes
├── src/
│   ├── device_file_io.c       [MUST IMPLEMENT FROM SCRATCH] — Binary file I/O + field extraction
│   └── main.c                 [MUST IMPLEMENT FROM SCRATCH] — Full pipeline demo
└── test/
    └── test_device_file_io.c  [MUST IMPLEMENT / EXTEND] — Unity unit test suite
```

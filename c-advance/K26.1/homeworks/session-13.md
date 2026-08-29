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

# Assignment — Lecture 13: Advanced Debugging & Quality Gates
**Deadline: 2026-09-05 23:59:00**

> [!IMPORTANT]
> **Quality Gate & CMake Mandatory Requirement (L13+)**
> Starting from this lecture, all homework submissions must adhere strictly to industry-standard **Quality Gate** project structures. 
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

---

## Exercise_1 [build] — Design-by-Contract Ring Buffer with Quality Gates

### 📂 File Deliverables Breakdown

| File Path | Status | Description |
|---|---|---|
| `include/contract.h` | **[MUST IMPLEMENT / COMPLETE]** | Complete the DbC macros (`REQUIRE`, `ENSURE`, `INVARIANT`, `NDEBUG` bypass, `CONTRACT_FAIL_`). |
| `include/ring_buffer.h` | **[PROVIDED INTERFACE]** | Header with `ring_buffer_t` struct definition and API declarations (given in hints). |
| `src/ring_buffer.c` | **[MUST IMPLEMENT FROM SCRATCH]** | Implement all ring buffer operations with mandatory DbC contracts embedded. |
| `src/main.c` | **[MUST IMPLEMENT FROM SCRATCH]** | Demo program verifying normal operations and testing intentional abort on contract violation. |
| `test/test_ring_buffer.c` | **[MUST IMPLEMENT / EXTEND]** | Unity unit test suite testing FIFO ordering, boundary conditions, empty/full detection. |
| `CMakeLists.txt` | **[PROVIDED STARTER / CONFIGURE]** | Quality Gate CMake configuration integrating `clang-tidy`, `cppcheck`, `ASan+UBSan`, and `Unity`. |
| `.clang-tidy` | **[PROVIDED STARTER / VERIFY]** | Static analysis configuration file. |
| `.pre-commit-config.yaml` | **[PROVIDED STARTER / VERIFY]** | Pre-commit hook configuration file. |

---

### Problem Statement

1. Implement the **Design-by-Contract (DbC)** macros in `include/contract.h`:
   - `REQUIRE(cond)`: Precondition check at function entry.
   - `ENSURE(cond)`: Postcondition check before function exit.
   - `INVARIANT(cond)`: Invariant check verifying internal consistency before and after operations.
   - All macros must print the failed condition expression, file name (`__FILE__`), line number (`__LINE__`), and function name (`__func__`), followed by `abort()`.
   - Under release builds (`#ifdef NDEBUG`), all macros must compile to `((void)0)` with zero performance overhead.

2. Implement a **Count-Tracking Ring Buffer** in `src/ring_buffer.c` (header in `include/ring_buffer.h`):
   > **Note on Ring Buffer Architecture:**
   > In embedded firmware, ring buffers come in multiple designs. Some lock-free Single-Producer Single-Consumer (SPSC) ring buffers avoid a `count` variable (or waste one slot) to avoid shared mutable state across threads. 
   > For this exercise, you are required to implement a **Count-Tracking Ring Buffer** (which explicitly maintains a `count` field alongside `head`, `tail`, and `capacity`). This architecture is chosen specifically because explicit tracking makes DbC preconditions (`REQUIRE(count < capacity)`), postconditions (`ENSURE(count == old_count + 1)`), and class invariants (`INVARIANT(count <= capacity)`) crystal clear to specify and verify.

3. Enforce the following contracts in your ring buffer implementation:
   - `REQUIRE(p_rb != NULL)` across all API functions.
   - `REQUIRE(p_byte != NULL)` in `ring_pop()`.
   - `REQUIRE(!ring_is_full(p_rb))` in `ring_push()`.
   - `REQUIRE(!ring_is_empty(p_rb))` in `ring_pop()`.
   - `ENSURE(p_rb->count > 0U)` after a successful push.
   - `INVARIANT(p_rb->count <= p_rb->capacity)` after initialization, push, and pop.

4. Establish the full **Quality Gate** structure around the project (`CMakeLists.txt`, `.clang-tidy`, `.pre-commit-config.yaml`).
5. Write thorough Unity unit tests in `test/test_ring_buffer.c` to test normal FIFO ordering, full/empty detection, and boundary conditions.

**Quality Gate Rules:**
- **Local Pre-commit Gate**: Run `pre-commit run --all-files` — must pass cleanly with all C files formatted and checked.
- **Build & Clang-Tidy Gate**: Run `cmake -B build -DCMAKE_BUILD_TYPE=Debug && cmake --build build` — must compile with `-Wall -Wextra -pedantic -Werror -std=c11` and pass `clang-tidy` with 0 warnings.
- **Static Analysis Gate**: Run `cmake --build build --target cppcheck` — must report 0 defects.
- **Sanitizer & Test Gate**: Run `cmake -B build-asan -DSANITIZER=asan+ubsan -DCMAKE_BUILD_TYPE=Debug && cmake --build build-asan && ctest --test-dir build-asan -V` — all Unity unit tests must pass with 0 sanitizer violations.
- **Coding Style & Documentation**: Follow BARR-C coding standard (`p_` pointer prefixes, explicit `uint8_t`/`uint32_t` types, mandatory braces) and document all functions/structures with Doxygen comments.

---

### Design Hints (optional)

**1. DbC Macros Starter Template (`include/contract.h`):**
```c
#ifndef CONTRACT_H
#define CONTRACT_H

#include <stdio.h>
#include <stdlib.h>

#ifdef NDEBUG
    #define REQUIRE(expr)   ((void)0)
    #define ENSURE(expr)    ((void)0)
    #define INVARIANT(expr) ((void)0)
#else
    #define CONTRACT_FAIL_(tag, expr_str) \
        do { \
            fprintf(stderr, "[%s VIOLATION] %s — in %s(), %s:%d\n", \
                    (tag), (expr_str), __func__, __FILE__, __LINE__); \
            abort(); \
        } while (0)

    #define REQUIRE(expr)   do { if (!(expr)) { CONTRACT_FAIL_("REQUIRE",   #expr); } } while (0)
    #define ENSURE(expr)    do { if (!(expr)) { CONTRACT_FAIL_("ENSURE",    #expr); } } while (0)
    #define INVARIANT(expr) do { if (!(expr)) { CONTRACT_FAIL_("INVARIANT", #expr); } } while (0)
#endif

#endif /* CONTRACT_H */
```

**2. Ring Buffer Header Interface (`include/ring_buffer.h`):**
```c
#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>
#include <stdbool.h>

#define RING_CAPACITY 8U

/**
 * @brief Count-Tracking Circular Ring Buffer.
 */
typedef struct {
    uint8_t  data[RING_CAPACITY]; /**< Internal storage array. */
    uint32_t head;                /**< Next write index.       */
    uint32_t tail;                /**< Next read index.        */
    uint32_t count;               /**< Current element count.  */
    uint32_t capacity;            /**< Maximum capacity.       */
} ring_buffer_t;

void     ring_init(ring_buffer_t *p_rb);
bool     ring_is_full(const ring_buffer_t *p_rb);
bool     ring_is_empty(const ring_buffer_t *p_rb);
uint32_t ring_count(const ring_buffer_t *p_rb);
void     ring_push(ring_buffer_t *p_rb, uint8_t byte);
uint8_t  ring_pop(ring_buffer_t *p_rb);

#endif /* RING_BUFFER_H */
```

**3. Quality Gate CMake Template (`CMakeLists.txt`):**
```cmake
cmake_minimum_required(VERSION 3.16)
project(exercise_1 VERSION 1.0.0 LANGUAGES C)

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
add_library(ring_buffer STATIC src/ring_buffer.c)
target_include_directories(ring_buffer PUBLIC include)
target_compile_options(ring_buffer PRIVATE -Wall -Wextra -pedantic -Werror)

add_executable(exercise_1 src/main.c)
target_link_libraries(exercise_1 PRIVATE ring_buffer)
target_compile_options(exercise_1 PRIVATE -Wall -Wextra -pedantic -Werror)

# Apply Sanitizers
if(SANITIZER STREQUAL "asan+ubsan")
    target_compile_options(ring_buffer PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(ring_buffer PUBLIC -fsanitize=address,undefined)
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

enable_testing()
add_executable(test_exercise_1 test/test_ring_buffer.c)
target_link_libraries(test_exercise_1 PRIVATE ring_buffer unity)
target_compile_options(test_exercise_1 PRIVATE -Wall -Wextra -pedantic -Werror)
if(SANITIZER STREQUAL "asan+ubsan")
    target_compile_options(test_exercise_1 PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(test_exercise_1 PUBLIC -fsanitize=address,undefined)
endif()
add_test(NAME test_exercise_1 COMMAND test_exercise_1)
```

**4. Starter `.clang-tidy`:**
```yaml
Checks: >
  -*,
  bugprone-*,
  cert-*,
  clang-analyzer-*,
  misc-*
WarningsAsErrors: >
  bugprone-*,
  cert-*
HeaderFilterRegex: 'include/.*\.h$'
```

**5. Starter `.pre-commit-config.yaml`:**
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

**6. Unity Test Skeleton (`test/test_ring_buffer.c`):**
```c
#include "unity.h"
#include "ring_buffer.h"

static ring_buffer_t g_rb;

void setUp(void) {
    ring_init(&g_rb);
}

void tearDown(void) {}

void test_ring_buffer_init_empty(void) {
    TEST_ASSERT_TRUE(ring_is_empty(&g_rb));
    TEST_ASSERT_FALSE(ring_is_full(&g_rb));
    TEST_ASSERT_EQUAL_UINT32(0U, ring_count(&g_rb));
}

void test_ring_buffer_push_pop_fifo(void) {
    ring_push(&g_rb, 0x11U);
    ring_push(&g_rb, 0x22U);
    TEST_ASSERT_EQUAL_UINT32(2U, ring_count(&g_rb));
    TEST_ASSERT_EQUAL_UINT8(0x11U, ring_pop(&g_rb));
    TEST_ASSERT_EQUAL_UINT8(0x22U, ring_pop(&g_rb));
    TEST_ASSERT_TRUE(ring_is_empty(&g_rb));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_ring_buffer_init_empty);
    RUN_TEST(test_ring_buffer_push_pop_fifo);
    return UNITY_END();
}
```

---

### Acceptance Criteria (Scoring)

- **[15%] Quality Gate Scaffolding**: Valid `.clang-tidy`, `.pre-commit-config.yaml`, and `CMakeLists.txt` with `CMAKE_C_CLANG_TIDY`, `cppcheck` target, and `SANITIZER` cache options.
- **[15%] Pre-commit & Static Analysis Gates**: `pre-commit run --all-files` and `cmake --build build --target cppcheck` execute with zero errors or warnings.
- **[20%] Sanitizers & Unit Tests Gate**: All Unity unit tests pass under ASan + UBSan (`ctest --test-dir build-asan -V`).
- **[15%] Code Documentation & Style**: Full Doxygen documentation on all functions/types, strict BARR-C naming (`p_rb`, `uint8_t`, `uint32_t`).
- **[35%] Core Logic & DbC Enforcement**: `REQUIRE`, `ENSURE`, and `INVARIANT` correctly defined, active on Debug builds, compiled out on `-DNDEBUG`, and correctly safeguarding the ring buffer.

---

### Expected Output

When running the main executable (`exercise_1`) demonstrating normal operations followed by an intentional contract violation (pushing to a full buffer):
```
=== Test 1: Normal push/pop operations ===
Pushed: 10 (count = 1)
Pushed: 20 (count = 2)
Popped: 10 (count = 1)
Popped: 20 (count = 0)
=== Test 2: Contract Violation Test ===
Buffer is full. Pushing one more item...
[REQUIRE VIOLATION] !ring_is_full(p_rb) — in ring_push(), src/ring_buffer.c:45
Aborted
```

### Expected Unit Test Output (L10+)

```
test_ring_buffer.c:16:test_ring_buffer_init_empty:PASS
test_ring_buffer.c:22:test_ring_buffer_push_pop_fifo:PASS
-----------------------
2 Tests 0 Failures 0 Ignored
OK
```

---

### Coding Standards Reference

- **Coding Standards Reference** (read the full rule description in the PDFs under `C_Books/`):
  - **MISRA-C 2012**: Dir 4.14 (Validate values from external sources), Rule 21.8 (The library functions `abort`, `exit` shall not be used — Note: justified deviation for DbC error handling), Dir 4.12 (No dynamic memory allocation in safety-critical code).
  - **CERT-C 2016**: ERR33-C (Detect and handle standard library errors), DCL39-C (Avoid information leakage in structures).

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs (under `C_Books/`) and read the full description of each rule above. After writing your code, verify your implementation follows these rules.

---

### Submission

```text
Exercise_1/
├── CMakeLists.txt              [PROVIDED STARTER / CONFIGURE] — Integrates clang-tidy, cppcheck, sanitizers, Unity
├── .clang-tidy                 [PROVIDED STARTER / VERIFY] — Static analysis rules
├── .pre-commit-config.yaml     [PROVIDED STARTER / VERIFY] — Pre-commit hook configuration
├── include/
│   ├── contract.h              [MUST IMPLEMENT / COMPLETE] — DbC macros (REQUIRE, ENSURE, INVARIANT)
│   └── ring_buffer.h           [PROVIDED INTERFACE] — Ring buffer struct & API prototypes
├── src/
│   ├── ring_buffer.c           [MUST IMPLEMENT FROM SCRATCH] — Full ring buffer logic & contracts
│   └── main.c                  [MUST IMPLEMENT FROM SCRATCH] — Demo & intentional abort test
└── test/
    └── test_ring_buffer.c      [MUST IMPLEMENT / EXTEND] — Unity unit test suite
```

---

## Exercise_2 [build] — Sanitizer Bug Hunt with Quality Gates

### 📂 File Deliverables Breakdown

| File Path | Status | Description |
|---|---|---|
| `include/sensor.h` | **[PROVIDED INTERFACE]** | Sensor driver interface header (given in hints). |
| `src/sensor.c` | **[PROVIDED BUGGY -> MUST FIX]** | Legacy starter code with 3 intentional bugs. You must diagnose and fix all root causes. |
| `test/test_sensor.c` | **[MUST IMPLEMENT / EXTEND]** | Unity unit test suite exercising all 3 functions, verifying clean pass under ASan/UBSan. |
| `CMakeLists.txt` | **[PROVIDED STARTER / CONFIGURE]** | Quality Gate CMake configuration integrating `clang-tidy`, `cppcheck`, `ASan+UBSan`, and `Unity`. |
| `.clang-tidy` | **[PROVIDED STARTER / VERIFY]** | Static analysis configuration file. |
| `.pre-commit-config.yaml` | **[PROVIDED STARTER / VERIFY]** | Pre-commit hook configuration file. |

---

### Problem Statement

You have received a legacy sensor driver module (`src/sensor.c`) containing **three critical bugs** that cause intermittent memory corruption and calculation errors in the field.
1. Place the provided buggy functions into `src/sensor.c` and declare them in `include/sensor.h`.
2. Construct the full **Quality Gate** CMake project structure (`CMakeLists.txt`, `.clang-tidy`, `.pre-commit-config.yaml`).
3. Write a Unity unit test suite in `test/test_sensor.c` that calls each sensor function.
4. Build with sanitizers enabled (`-DSANITIZER=asan+ubsan`) and run tests using `ctest --test-dir build-asan -V`.
5. Observe and document the runtime crash diagnostics produced by:
   - **AddressSanitizer (ASan)** detecting Bug 1 (Stack buffer overflow) and Bug 3 (Use-after-free / heap violation).
   - **UndefinedBehaviorSanitizer (UBSan)** detecting Bug 2 (Signed 32-bit integer overflow).
6. **Fix the root causes** of all three bugs in `src/sensor.c` so that:
   - Bug 1: Loop boundary correctly checks array limits.
   - Bug 2: Arithmetic avoids signed integer overflow using safe bounds checking or saturation.
   - Bug 3: Memory is allocated statically (avoiding dynamic memory per MISRA Dir 4.12) or pointer is invalidated immediately after free.
7. Verify that all Quality Gates pass with zero warnings and all Unity unit tests succeed cleanly under ASan + UBSan.

**Quality Gate Rules:**
- **Local Pre-commit Gate**: `pre-commit run --all-files` passes with 0 errors.
- **Build & Clang-Tidy Gate**: `cmake -B build && cmake --build build` compiles with `-Wall -Wextra -pedantic -Werror -std=c11` and `CMAKE_C_CLANG_TIDY` with 0 warnings.
- **Static Analysis Gate**: `cmake --build build --target cppcheck` passes with 0 warnings.
- **Sanitizer & Test Gate**: `cmake -B build-asan -DSANITIZER=asan+ubsan && cmake --build build-asan && ctest --test-dir build-asan -V` passes all tests with zero sanitizer errors.
- **Coding Style & Documentation**: Follow BARR-C coding standard and document all functions/structures with Doxygen comments.

---

### Design Hints (optional)

**1. Sensor Interface (`include/sensor.h`):**
```c
#ifndef SENSOR_H
#define SENSOR_H

#include <stdint.h>
#include <stdbool.h>

#define SENSOR_SAMPLE_COUNT 8U

void     sensor_process_readings(uint8_t *p_readings, uint32_t count);
int32_t  sensor_accumulate_data(int32_t current_val, int32_t sample);
uint32_t sensor_get_calibration(void);

#endif /* SENSOR_H */
```

**2. Legacy Buggy Implementation (`src/sensor.c` — provided starter code to be fixed):**
```c
#include "sensor.h"
#include <stdlib.h>
#include <stdio.h>

/* Bug 1: Stack Buffer Overflow (ASan detects) */
void sensor_process_readings(uint8_t *p_readings, uint32_t count)
{
    /* BUG: off-by-one index (<= count instead of < count) */
    for (uint32_t i = 0U; i <= count; i++)
    {
        p_readings[i] = (uint8_t)(p_readings[i] * 2U);
    }
}

/* Bug 2: Signed Integer Overflow (UBSan detects) */
int32_t sensor_accumulate_data(int32_t current_val, int32_t sample)
{
    /* BUG: Unchecked signed addition causing signed overflow (Undefined Behavior) */
    int32_t result = current_val + sample;
    return result;
}

/* Bug 3: Use-After-Free & Dynamic Allocation (ASan detects) */
uint32_t sensor_get_calibration(void)
{
    uint32_t *p_cal = (uint32_t *)malloc(sizeof(uint32_t) * 4U);
    if (p_cal == NULL)
    {
        return 0U;
    }

    p_cal[0] = 0xDEADBEEFU;
    free(p_cal);

    /* BUG: Reading from heap memory after it has been freed */
    uint32_t value = p_cal[0];
    return value;
}
```

**3. Unity Test Suite (`test/test_sensor.c`):**
```c
#include "unity.h"
#include "sensor.h"

void setUp(void) {}
void tearDown(void) {}

void test_sensor_process_readings_bounds(void) {
    uint8_t samples[SENSOR_SAMPLE_COUNT] = {1, 2, 3, 4, 5, 6, 7, 8};
    sensor_process_readings(samples, SENSOR_SAMPLE_COUNT);
    TEST_ASSERT_EQUAL_UINT8(2U, samples[0]);
    TEST_ASSERT_EQUAL_UINT8(16U, samples[7]);
}

void test_sensor_accumulate_overflow_safe(void) {
    int32_t base = 2147483640;
    int32_t sample = 100;
    int32_t res = sensor_accumulate_data(base, sample);
    /* Expect safe saturation or clamped result */
    TEST_ASSERT_TRUE(res > 0);
}

void test_sensor_get_calibration_valid(void) {
    uint32_t cal = sensor_get_calibration();
    TEST_ASSERT_EQUAL_HEX32(0xDEADBEEFU, cal);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_sensor_process_readings_bounds);
    RUN_TEST(test_sensor_accumulate_overflow_safe);
    RUN_TEST(test_sensor_get_calibration_valid);
    return UNITY_END();
}
```

---

### Acceptance Criteria (Scoring)

- **[15%] Quality Gate Scaffolding**: Valid `.clang-tidy`, `.pre-commit-config.yaml`, and `CMakeLists.txt` with sanitizers & static analysis configured.
- **[15%] Pre-commit & Static Analysis Gates**: `pre-commit run --all-files` and `cmake --build build --target cppcheck` execute with zero errors.
- **[20%] Sanitizers & Unit Tests Gate**: All Unity unit tests pass under ASan + UBSan with zero sanitizer crash reports.
- **[15%] Code Documentation & Style**: Full Doxygen comments on all functions, BARR-C compliance, no raw `malloc` in embedded driver (MISRA Dir 4.12).
- **[35%] Root Cause Fixes**: Proper loop bounds, safe integer overflow handling, and static memory allocation for calibration data.

---

### Expected Output

Running tests through CTest with sanitizers active on fixed code:
```
$ ctest --test-dir build-asan -V
UpdateCTestConfiguration  from :.../build-asan/DartConfiguration.tcl
Test project .../Exercise_2/build-asan
Constructing a list of tests
Done constructing a list of tests
Updating test list for fixtures
Added 1 tests to run.
Start 1: test_exercise_2
1: Test command: .../Exercise_2/build-asan/test_exercise_2
1: Working Directory: .../Exercise_2/build-asan
1: Test Timeout was unset.
1: test_sensor.c:13:test_sensor_process_readings_bounds:PASS
1: test_sensor.c:20:test_sensor_accumulate_overflow_safe:PASS
1: test_sensor.c:27:test_sensor_get_calibration_valid:PASS
1: -----------------------
1: 3 Tests 0 Failures 0 Ignored 
1: OK
1/1 Test #1: test_exercise_2 ..................   Passed    0.02 sec

100% tests passed, 0 tests failed out of 1
```

### Expected Unit Test Output (L10+)

```
test_sensor.c:13:test_sensor_process_readings_bounds:PASS
test_sensor.c:20:test_sensor_accumulate_overflow_safe:PASS
test_sensor.c:27:test_sensor_get_calibration_valid:PASS
-----------------------
3 Tests 0 Failures 0 Ignored
OK
```

---

### Coding Standards Reference

- **Coding Standards Reference** (read the full rule description in the PDFs under `C_Books/`):
  - **MISRA-C 2012**: Rule 18.1 (Pointer arithmetic shall not point outside array bounds), Dir 4.12 (Dynamic memory allocation shall not be used).
  - **CERT-C 2016**: ARR30-C (Do not form or use out-of-bounds pointers or array subscripts), INT32-C (Ensure that operations on signed integers do not result in overflow), MEM30-C (Do not access freed memory).

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs (under `C_Books/`) and read the full description of each rule above. After writing your code, verify your implementation follows these rules.

---

### Submission

```text
Exercise_2/
├── CMakeLists.txt              [PROVIDED STARTER / CONFIGURE] — Integrates clang-tidy, cppcheck, sanitizers, Unity
├── .clang-tidy                 [PROVIDED STARTER / VERIFY] — Static analysis rules
├── .pre-commit-config.yaml     [PROVIDED STARTER / VERIFY] — Pre-commit hook configuration
├── include/
│   └── sensor.h                [PROVIDED INTERFACE] — Sensor API header
├── src/
│   └── sensor.c                [PROVIDED BUGGY -> MUST FIX] — Fix 3 bugs (bounds, overflow, UAF)
└── test/
    └── test_sensor.c           [MUST IMPLEMENT / EXTEND] — Unity unit test suite
```

---

## Exercise_3 [build] — Advanced GDB & Post-Mortem Core Dump Analysis with VSCode

### 📂 File Deliverables Breakdown

| File Path | Status | Description |
|---|---|---|
| `.vscode/launch.json` | **[MUST CONFIGURE / COMPLETE]** | VSCode configuration for Live Debugging AND Post-Mortem Core Dump debugging. |
| `include/telemetry.h` | **[PROVIDED INTERFACE]** | Telemetry packet structures and parser prototypes (given in hints). |
| `src/telemetry.c` | **[PROVIDED BUGGY -> MUST FIX]** | Parser with crash vulnerability. Diagnose via GDB/Core dump and fix. |
| `src/main.c` | **[PROVIDED DEMO / HARNESS]** | Main harness that processes telemetry and simulates crash on `--crash` flag. |
| `test/test_telemetry.c` | **[MUST IMPLEMENT / EXTEND]** | Unity unit tests verifying clean handling of valid and corrupt telemetry frames. |
| `CMakeLists.txt` | **[PROVIDED STARTER / CONFIGURE]** | Quality Gate CMake configuration with debug symbols (`-g3`) and sanitizers. |
| `.clang-tidy` | **[PROVIDED STARTER / VERIFY]** | Static analysis configuration file. |
| `.pre-commit-config.yaml` | **[PROVIDED STARTER / VERIFY]** | Pre-commit hook configuration file. |

---

### Problem Statement

You are responsible for the telemetry communication module (`src/telemetry.c`) of an industrial IoT gateway. In the field, the gateway crashes intermittently with `Segmentation fault (core dumped)` when parsing corrupted telemetry streams.

Your mission:
1. **Part A: Configure VSCode for Live GDB Debugging**:
   - Create `.vscode/launch.json` with a `"Debug Telemetry (Live GDB)"` configuration.
   - Run the debugger in VSCode and set a **Conditional Breakpoint** in `telemetry_parse_packet()` to break only when `p_packet->channel_id > 8U`.
   - In the Debug Console / GDB prompt, set a **Hardware Watchpoint** (`watch g_gateway_state`) to observe state mutations.

2. **Part B: Post-Mortem Core Dump Debugging (GDB CLI & VSCode GUI)**:
   - Enable core dump generation on your machine:
     ```bash
     ulimit -c unlimited
     ```
   - Build the program with debug symbols (`-g3 -O0`) and run the crash generator:
     ```bash
     ./build/exercise_3 --crash
     ```
     This triggers a crash and produces a `core` dump file.
   
   > [!NOTE]
   > **OS-Specific Core Dump Handling:**
   > - **Linux / WSL2 (Ubuntu / Debian — Recommended):**
   >   - `ulimit -c unlimited` writes `core` or `core.<pid>` directly into the current directory (or managed by `coredumpctl`).
   >   - VSCode `launch.json` points to: `"coreDumpPath": "${workspaceFolder}/core"`.
   > - **macOS:**
   >   - macOS dumps cores into `/cores/core.<pid>`. Make sure `/cores` is writable: `sudo chmod 1777 /cores`.
   >   - VSCode `launch.json` points to: `"coreDumpPath": "/cores/core.<pid>"`. (Note: macOS uses LLDB as the default system debugger).
   > - **Windows:**
   >   - Windows native generates Windows `.dmp` files rather than ELF core dumps. For embedded C workflows, juniors on Windows should run via **WSL2 (Ubuntu)** or a **Dev Container / Docker**.

   - **CLI Analysis**: Load the core dump into GDB:
     ```bash
     gdb ./build/exercise_3 core
     ```
     Inspect the call stack (`bt`), the active frame variables (`info locals`), and the memory buffer (`x/16xb p_packet->payload`) to determine the exact cause of the crash.
   - **VSCode GUI Core Dump Analysis**:
     Add a `"Debug Core Dump (Post-Mortem)"` configuration to `.vscode/launch.json` containing `"coreDumpPath": "${workspaceFolder}/core"` (or `/cores/core.<pid>` on macOS).
     Press `F5` in VSCode to inspect the crash post-mortem visually (call stack, local variable values, and memory panel).

3. **Part C: Fix the Root Cause & Verify with Unity**:
   - In `src/telemetry.c`, implement defensive validation to reject corrupt packets, validate buffer lengths, and prevent NULL pointer dereferences.
   - Write Unity unit tests in `test/test_telemetry.c` that test valid packets, corrupted headers, and oversized payloads.
   - Ensure all Quality Gates pass with zero warnings.

**Quality Gate Rules:**
- **Local Pre-commit Gate**: `pre-commit run --all-files` passes with 0 errors.
- **Build & Clang-Tidy Gate**: `cmake -B build && cmake --build build` compiles cleanly with `-Wall -Wextra -pedantic -Werror -std=c11` and `CMAKE_C_CLANG_TIDY`.
- **Static Analysis Gate**: `cmake --build build --target cppcheck` reports 0 defects.
- **Sanitizer & Test Gate**: `cmake -B build-asan -DSANITIZER=asan+ubsan && cmake --build build-asan && ctest --test-dir build-asan -V` passes all tests with zero sanitizer errors.
- **Coding Style & Documentation**: Follow BARR-C coding standard and document all functions/structures with Doxygen comments.

---

### Design Hints (optional)

**1. Telemetry Interface (`include/telemetry.h`):**
```c
#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_PAYLOAD_SIZE 64U
#define MAX_CHANNELS     8U

typedef enum {
    TELEMETRY_OK = 0,
    TELEMETRY_ERR_NULL_PTR,
    TELEMETRY_ERR_INVALID_CHANNEL,
    TELEMETRY_ERR_OVERSIZED_PAYLOAD,
    TELEMETRY_ERR_CHECKSUM_MISMATCH
} telemetry_status_t;

typedef struct {
    uint8_t  channel_id;
    uint8_t  payload_len;
    uint8_t  payload[MAX_PAYLOAD_SIZE];
    uint16_t checksum;
} telemetry_packet_t;

extern volatile uint32_t g_gateway_state;

telemetry_status_t telemetry_parse_packet(const uint8_t *p_raw_buf, uint32_t raw_len, telemetry_packet_t *p_out_packet);
uint16_t           telemetry_calculate_checksum(const uint8_t *p_data, uint32_t len);

#endif /* TELEMETRY_H */
```

**2. Buggy Telemetry Implementation (`src/telemetry.c` — to be analyzed & fixed):**
```c
#include "telemetry.h"
#include <string.h>

volatile uint32_t g_gateway_state = 1U;

uint16_t telemetry_calculate_checksum(const uint8_t *p_data, uint32_t len)
{
    uint16_t sum = 0U;
    for (uint32_t i = 0U; i < len; i++)
    {
        sum = (uint16_t)(sum + p_data[i]);
    }
    return sum;
}

telemetry_status_t telemetry_parse_packet(const uint8_t *p_raw_buf, uint32_t raw_len, telemetry_packet_t *p_out_packet)
{
    /* BUG 1: Missing NULL pointer check on p_raw_buf and p_out_packet */
    
    /* BUG 2: Reads header without verifying raw_len >= 2U */
    p_out_packet->channel_id  = p_raw_buf[0];
    p_out_packet->payload_len = p_raw_buf[1];

    /* Mutate gateway state */
    g_gateway_state = (uint32_t)(p_out_packet->channel_id + 100U);

    /* BUG 3: Unchecked payload copy causing buffer overflow on oversized raw input */
    memcpy(p_out_packet->payload, &p_raw_buf[2], p_out_packet->payload_len);

    p_out_packet->checksum = telemetry_calculate_checksum(p_out_packet->payload, p_out_packet->payload_len);

    return TELEMETRY_OK;
}
```

**3. Main Harness with Crash Mode (`src/main.c`):**
```c
#include "telemetry.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    telemetry_packet_t packet;
    memset(&packet, 0, sizeof(packet));

    if (argc > 1 && strcmp(argv[1], "--crash") == 0)
    {
        printf("=== Simulating Malformed Packet Crash ===\n");
        fflush(stdout);
        /* Pass NULL to deliberately trigger SIGSEGV and create core dump */
        telemetry_parse_packet(NULL, 0U, &packet);
    }
    else
    {
        printf("=== Normal Telemetry Processing ===\n");
        uint8_t normal_raw[6] = {0x01U, 0x02U, 0xAAU, 0xBBU, 0x00U, 0x00U};
        telemetry_status_t status = telemetry_parse_packet(normal_raw, sizeof(normal_raw), &packet);
        printf("Status: %d, Channel: %u, Length: %u\n", (int)status, packet.channel_id, packet.payload_len);
    }

    return 0;
}
```

**4. How to Generate `.vscode/launch.json` via VSCode Shortcuts & Commands:**

> ⌨️ **No Manual JSON File Creation Needed!**
> VSCode provides built-in commands and GUI prompts to generate and edit `launch.json`:
>
> 1. **Generate Starter `launch.json`**:
>    - Press `Ctrl+Shift+D` (Windows/Linux) or `Cmd+Shift+D` (macOS) to switch to the **Run and Debug** view.
>    - Click the blue link: **"create a launch.json file"**.
>    - In the top dropdown menu, select **"C++ (GDB/LLDB)"** (or **"C/C++: gcc build and debug active file"**).
>    - VSCode automatically scaffolds the `.vscode/launch.json` file in your project root.
>
> 2. **Add New Debug Targets via Command Palette**:
>    - Open `.vscode/launch.json`.
>    - Press `Ctrl+Shift+P` (Windows/Linux) or `Cmd+Shift+P` (macOS) and run:
>      `Debug: Add Configuration...`
>    - Or click the blue **"Add Configuration..."** button at the bottom right corner of the editor.
>    - Select **`C/C++: (gdb) Launch`** to add a live debug target.
>    - Select **`C/C++: (gdb) Core Dump`** to add a post-mortem core dump target.
>    - Use `Ctrl+Space` anywhere inside the JSON to trigger IntelliSense autocomplete for fields like `"coreDumpPath"`, `"program"`, and `"setupCommands"`.

**Reference `.vscode/launch.json` Configuration:**
```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug Telemetry (Live GDB)",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build/exercise_3",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ],
            "preLaunchTask": "cmake-build"
        },
        {
            "name": "Debug Core Dump (Post-Mortem)",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build/exercise_3",
            "coreDumpPath": "${workspaceFolder}/core",
            "cwd": "${workspaceFolder}",
            "MIMode": "gdb",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ]
        }
    ]
}
```

> 💡 **Tip 1 — How to Build & Run Live Debugging in VSCode:**
> - **Step 1 (Build with CMake)**: `launch.json` only runs the debugger; it does not compile your code. You must first compile your debug binary using CMake:
>   ```bash
>   cmake -B build -DCMAKE_BUILD_TYPE=Debug
>   cmake --build build
>   ```
> - **Step 2 (Launch Debugger)**: Open the **Run and Debug** view (`Ctrl+Shift+D` / `Cmd+Shift+D`), select **"Debug Telemetry (Live GDB)"**, set breakpoints, and press **`F5`**.
>
> 💡 **Tip 2 — How to Debug Core Dumps (DO NOT REBUILD!):**
> 1. When `./build/exercise_3 --crash` crashes with `Segmentation fault (core dumped)`, ensure the file named `core` (or `core.<pid>` / `/cores/core.<pid>`) is available.
> 2. **DO NOT recompile your code!** The existing binary must match the core dump.
> 3. Open the **Run and Debug** view (`Ctrl+Shift+D` or `Cmd+Shift+D`).
> 4. Select **"Debug Core Dump (Post-Mortem)"** and press **`F5`**.
> 5. VSCode immediately highlights the exact line of code that caused the crash, showing all local variables and call stack frames at the moment of failure!

> [!IMPORTANT]
> **Core Dump Analysis: Do We Need the Matching Source Code & Binary?**
>
> 1. **The Matching Binary Rule (MANDATORY)**:
>    - The core dump file contains **only** memory pages, CPU registers, and the stack. It does **NOT** contain function names, variable names, or debug symbols.
>    - Therefore, GDB **MUST** have access to the **exact same ELF binary** (with `-g3` debug symbols) that was running when the crash occurred. If you recompile the code, the build ID and memory addresses will not match!
>
> 2. **What if the source code was moved or is missing?**
>    - **Can we still debug?** **YES!** If you have the matching ELF binary, GDB can still inspect the full backtrace (`bt`), CPU registers (`info registers`), raw assembly instructions (`disassemble`), and memory hex dumps (`x/32xb`).
>    - **Mapping moved source files**: If the source code path on your machine is different from where the binary was built (e.g. built in CI / Docker / Yocto), you can map the paths in `.vscode/launch.json` using `"sourceFileMap"`:
>      ```json
>      "sourceFileMap": {
>          "/builds/ci/project/src": "${workspaceFolder}/src"
>      }
>      ```
>      Or inside GDB prompt: `set substitute-path /builds/ci/project/src /local/workspace/src`.

#### 🔍 Critical Key-Value Configuration Pairs Explained

| Property Key | Configured Value in Exercise 3 | What It Does & Why It's Critical |
|---|---|---|
| **`"program"`** | `"${workspaceFolder}/build/exercise_3"` | Points GDB to your compiled ELF binary containing **debug symbols** (`-g3`). Without this, GDB cannot map memory addresses back to your C source code lines or variable names. |
| **`"coreDumpPath"`** | `"${workspaceFolder}/core"` *(or `/cores/core.<pid>`)* | **The magic key for post-mortem debugging.** When present, GDB does not launch a new process; instead, it reconstructs the exact memory, call stack, and register snapshot at the moment of the crash. |
| **`"MIMode"`** | `"gdb"` | Specifies the underlying Machine Interface debugger engine (`gdb` for Linux/WSL, `lldb` for macOS). |
| **`"args"`** | `["--crash"]` *(or `[]`)* | Passes command-line parameters to `int main(int argc, char *argv[])`. Useful for switching between normal operation and the crash trigger. |
| **`"stopAtEntry"`** | `false` *(or `true`)* | When set to `true`, GDB automatically breaks at the first line of `main()`, letting you step through startup before any functions run. |
| **`"setupCommands"`** | `"-enable-pretty-printing"` | Enables GDB pretty-printers so that structs (`telemetry_packet_t`), enums, and arrays display as formatted trees in the VSCode GUI rather than raw hexadecimal blocks. |
| **`"preLaunchTask"`** | `"cmake-build"` | Links to a VSCode task that automatically compiles your latest code changes before launching the debugger. |

---

**5. Essential Debug Compiler Flags (`target_compile_options`):**

> ⚠️ **Warning to Juniors: Why `-g` alone is often not enough!**
> When setting up your debug target in `CMakeLists.txt`, always apply these **3 essential debug flags**:
>
> 1. **`-g3` (Maximum Debug Symbols + Preprocessor Macros)**:
>    - Standard `-g` gives basic line numbers.
>    - **`-g3`** embeds `#define` macro definitions into DWARF tables, allowing GDB to expand macros dynamically (e.g. `(gdb) print MAX_PAYLOAD_SIZE`).
>
> 2. **`-O0` (Disable Optimizations)**:
>    - If optimization (`-O2` / `-O3`) is enabled, the compiler reorders instructions and discards local variables, resulting in frustrating `variable <value optimized out>` errors in GDB.
>    - `-O0` ensures a strict 1-to-1 correlation between your C source lines and assembly instructions.
>
> 3. **`-fno-omit-frame-pointer` (Preserve Stack Frame Pointers)**:
>    - Crucial for core dump and post-mortem analysis!
>    - Forces the compiler to maintain the frame pointer register (`RBP` on x86, `R7`/`FP` on ARM), ensuring GDB can reconstruct 100% reliable stack unwinding and backtraces (`bt`) when the program crashes.
>
> ```cmake
> # Recommended CMakeLists.txt configuration for Exercise 3:
> target_compile_options(exercise_3 PRIVATE
>     -Wall -Wextra -pedantic -Werror -std=c11
>     -g3                       # Level 3 debug symbols + macro info
>     -O0                       # No optimizations: avoids "<value optimized out>"
>     -fno-omit-frame-pointer   # Guarantees reliable call stack unwinding
> )
> ```

---

### Acceptance Criteria (Scoring)

- **[15%] Quality Gate Scaffolding**: Valid `.clang-tidy`, `.pre-commit-config.yaml`, and `CMakeLists.txt` integrating `clang-tidy`, `cppcheck`, and sanitizers.
- **[15%] VSCode Launch Configurations**: Valid `.vscode/launch.json` with working configurations for both live debugging and post-mortem core dump analysis (`coreDumpPath`).
- **[20%] Core Dump & GDB Diagnostics**: Successfully generate and inspect the core dump, identifying the NULL pointer dereference and buffer boundary errors.
- **[15%] Code Documentation & Style**: Full Doxygen comments, BARR-C compliance, no unhandled error returns.
- **[35%] Root Cause Fixes & Unity Tests**: Complete input validation in `telemetry_parse_packet()`, safe checksum computation, and 100% passing Unity tests under ASan + UBSan.

---

### Expected Output

**1. GDB Core Dump Post-Mortem Session (CLI):**
```text
$ gdb ./build/exercise_3 core
Core was generated by `./build/exercise_3 --crash'.
Program terminated with signal SIGSEGV, Segmentation fault.
#0  0x0000000000401234 in telemetry_parse_packet (p_raw_buf=0x0, raw_len=0, p_out_packet=0x7ffde480) at src/telemetry.c:20
20          p_out_packet->channel_id  = p_raw_buf[0];
(gdb) bt
#0  0x0000000000401234 in telemetry_parse_packet (p_raw_buf=0x0, raw_len=0, p_out_packet=0x7ffde480) at src/telemetry.c:20
#1  0x0000000000401567 in main (argc=2, argv=0x7ffde598) at src/main.c:16
(gdb) print p_raw_buf
$1 = (const uint8_t *) 0x0
```

**2. Unity Unit Tests on Fixed Code:**
```text
test_telemetry.c:15:test_telemetry_parse_valid_packet:PASS
test_telemetry.c:22:test_telemetry_parse_null_pointer_rejected:PASS
test_telemetry.c:29:test_telemetry_parse_oversized_payload_rejected:PASS
test_telemetry.c:36:test_telemetry_parse_invalid_channel_rejected:PASS
-----------------------
4 Tests 0 Failures 0 Ignored
OK
```

---

### Coding Standards Reference

- **Coding Standards Reference** (read the full rule description in the PDFs under `C_Books/`):
  - **MISRA-C 2012**: Dir 4.14 (Validate values from external sources), Rule 18.1 (Pointer arithmetic shall stay within array bounds), Rule 21.3 (Dynamic allocation restrictions).
  - **CERT-C 2016**: EXP34-C (Do not dereference null pointers), ERR33-C (Detect and handle standard library errors), ARR38-C (Guarantee that pointer arithmetic remains in bounds).

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs (under `C_Books/`) and read the full description of each rule above. After writing your code, verify your implementation follows these rules.

---

### Submission

```text
Exercise_3/
├── CMakeLists.txt              [PROVIDED STARTER / CONFIGURE] — Integrates clang-tidy, cppcheck, sanitizers, Unity
├── .clang-tidy                 [PROVIDED STARTER / VERIFY] — Static analysis rules
├── .pre-commit-config.yaml     [PROVIDED STARTER / VERIFY] — Pre-commit hook configuration
├── .vscode/
│   └── launch.json             [MUST CONFIGURE / COMPLETE] — Live GDB & Core Dump configurations
├── include/
│   └── telemetry.h             [PROVIDED INTERFACE] — Telemetry API header
├── src/
│   ├── main.c                  [PROVIDED DEMO / HARNESS] — Main app & crash scenario
│   └── telemetry.c             [PROVIDED BUGGY -> MUST FIX] — Fixed telemetry driver
└── test/
    └── test_telemetry.c        [MUST IMPLEMENT / EXTEND] — Unity unit test suite
```

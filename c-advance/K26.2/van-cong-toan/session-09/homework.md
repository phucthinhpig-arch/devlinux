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

# Assignment — Session 09: CMake & TDD
**Deadline: 2026-08-09 23:59:00**

---

## Overview

This homework covers the core concepts from Lecture 9: Modern CMake and Test-Driven Development (TDD) using Unity and FFF.

| Exercise | Topic | Difficulty |
|:---|:---|:---:|
| Exercise_1 | CMake Modular Build (`hw_lab1_cmake_modular`) | ★☆☆ |
| Exercise_2 | TDD with Unity (`hw_lab2_tdd_sensor.c`) | ★★☆ |
| Exercise_3 | Hardware Mocking with FFF (`hw_lab3_mock_uart.c`) | ★★★ |

---

## Exercise_1 [build]

### Problem Statement

**Build a 3-Module Project with CMake**

**Scenario:**
You need to build a modular C project using CMake. You will separate the code into three components: a CRC calculation library, a Ring Buffer library, and a main executable that links them together.

**Requirements:**

Create a project directory structure from scratch and implement the code using the snippets provided in the Design Hints below. Then, write a `CMakeLists.txt` file from scratch that:
1. Builds `crc_calc.c` as a static library (`crc_lib`).
2. Builds `ring_buffer.c` as a static library (`ring_buf_lib`).
3. Builds `main.c` as an executable (`hw_lab1`) and links both libraries to it.
4. Ensures the `include/` folders for each library are correctly propagated so `main.c` can `#include` them.

**Rules:**
- Follow BARR-C coding style.
- **Code Documentation:** All functions and data structures MUST be fully documented using Doxygen-style comments.
- Compile with strict flags: `-Wall -Wextra -pedantic -Werror -std=c11`.
- Your `CMakeLists.txt` must apply these flags to all targets.
- Use `cppcheck` and `clang-tidy` to analyze, and make sure there are no warning or error messages.

### Coding Standards Reference

**MISRA-C 2012 (Safety):**
| Rule | Category | Relevance to This Exercise |
|---|---|---|
| Section 5 | - | Traceable, reproducible builds. Using CMake to enforce strict compiler flags ensures the build is deterministic and meets this audit requirement. |

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs (under `C_Books/`)
> and read the full description of each rule above. After writing your code,
> verify your implementation follows these rules.

### Design Hints (optional)

Use the following snippets to create the source files for your modules:

**1. `include/crc_calc.h` and `src/crc_calc.c`**
```c
#ifndef CRC_CALC_H
#define CRC_CALC_H
#include <stdint.h>
uint8_t crc8_calc(const uint8_t *data, uint32_t length);
#endif

// In crc_calc.c:
#include "crc_calc.h"
uint8_t crc8_calc(const uint8_t *data, uint32_t length) {
    uint8_t crc = 0;
    for (uint32_t i = 0; i < length; i++) crc ^= data[i]; // Simplified CRC
    return crc;
}
```

**2. `include/ring_buffer.h` and `src/ring_buffer.c`**
```c
#ifndef RING_BUFFER_H
#define RING_BUFFER_H
#include <stdbool.h>
bool ring_buf_push(int val);
bool ring_buf_pop(int *val);
#endif

// In ring_buffer.c:
#include "ring_buffer.h"
bool ring_buf_push(int val) { (void)val; return true; }
bool ring_buf_pop(int *val) { *val = 3; return true; }
```

**3. `src/main.c`**
```c
#include <stdio.h>
#include "crc_calc.h"
#include "ring_buffer.h"

int main(void) {
    uint8_t data[] = {0x01, 0x02, 0x03};
    printf("=== CRC-8 of {0x01, 0x02, 0x03} = 0x%02X ===\n", crc8_calc(data, 3));
    
    int val = 0;
    if (ring_buf_push(3) && ring_buf_pop(&val)) {
        printf("=== Ring Buffer: Push 3, Pop %d — OK ===\n", val);
    }
    return 0;
}
```

### Acceptance Criteria (Scoring)

- **[20%]** The project is structured cleanly with separate `include` and `src` directories for the modules.
- **[40%]** `CMakeLists.txt` successfully builds the 2 static libraries and the executable.
- **[10%]** `CMakeLists.txt` correctly uses `PRIVATE`/`PUBLIC` scopes for `target_include_directories`.
- **[10%]** Strict C11 compiler flags are applied via CMake.
- **[20%]** Code builds, passes static analysis, and produces the exact expected output.

### Expected Output

When built and run using `cmake -S . -B build && cmake --build build && ./build/hw_lab1`, the output must be:

```
=== CRC-8 of {0x01, 0x02, 0x03} = 0x00 ===
=== Ring Buffer: Push 3, Pop 3 — OK ===
```

Exit code: `0` on success.

### Submission

```
Exercise_1/
├── CMakeLists.txt  (required)
├── include/        (required)
└── src/            (required)
```

---

## Exercise_2 [build]

### Problem Statement

**Red–Green–Refactor Cycle with Unity**

**Scenario:**
You need to implement a sensor driver module. Rather than writing the code first, you will practice the full TDD cycle by writing the tests first using the Unity test framework.

**Requirements:**

1. Review the `sensor.h` API provided in the Design Hints below.
2. **RED Phase**: Write `test_sensor.c` using Unity. Add at least **5 test cases** covering:
   - Zero input
   - Max valid input (4095)
   - Out-of-range input (4096) → must safely return 0
   - `moving_average` with a typical 4-sample input
   - `moving_average` with `count=0` → must return 0 (prevent divide-by-zero).
3. **GREEN Phase**: Write `sensor.c` to make all 5 tests pass.
4. Write a `CMakeLists.txt` that fetches Unity via `FetchContent`, compiles your test executable, and registers it with `add_test()`.

**Rules:**
- Follow BARR-C coding style.
- **Code Documentation:** All functions MUST be fully documented using Doxygen-style comments.
- Compile with strict flags: `-Wall -Wextra -pedantic -Werror -std=c11`.
- Use `cppcheck` and `clang-tidy` to analyze, and make sure there are no warning or error messages.

### Coding Standards Reference

**MISRA-C 2012 & CERT-C 2016:**
| Rule | Category | Relevance to This Exercise |
|---|---|---|
| MISRA Directive 4.11 | Required | Validate values passed to library functions. Verified by your tests for out-of-range values. |
| CERT INT33-C | Security | Ensure that division does not result in divide-by-zero. Verified by your zero-count test case. |

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs (under `C_Books/`)
> and read the full description of each rule above. After writing your code,
> verify your implementation follows these rules.

### Design Hints (optional)

Use this API definition for `sensor.h`:

```c
#ifndef SENSOR_H
#define SENSOR_H

#include <stdint.h>

/**
 * @brief Convert raw ADC (12-bit) to millivolts.
 * @param raw_adc Valid range: 0-4095.
 * @return Millivolts, or 0 if out of range.
 */
uint32_t sensor_adc_to_mv(uint32_t raw_adc);

/**
 * @brief Calculate the moving average of an array of samples.
 * @param samples Array of millivolt readings.
 * @param count Number of samples.
 * @return Average, or 0 if count is 0.
 */
uint32_t sensor_moving_average(const uint32_t *samples, uint32_t count);

#endif // SENSOR_H
```

### Acceptance Criteria (Scoring)

- **[30%]** The test file correctly uses Unity's `TEST_ASSERT_EQUAL_INT32` macros and covers all 5 requested edge cases.
- **[30%]** The production code in `sensor.c` correctly handles the edge cases (especially divide-by-zero prevention).
- **[20%]** CMake builds the test executable and registers it with CTest.
- **[20%]** Code passes static analysis and all Unity tests `PASS`.

### Expected Output

When running `cmake --build build && ctest --test-dir build -V`, the output must show all tests passing:

```
test_sensor.c:20:test_adc_zero:PASS
test_sensor.c:25:test_adc_max:PASS
test_sensor.c:30:test_adc_out_of_range:PASS
test_sensor.c:35:test_moving_average_valid:PASS
test_sensor.c:40:test_moving_average_zero_count:PASS

100% tests passed, 0 tests failed out of 1
```

### Submission

```
Exercise_2/
├── CMakeLists.txt  (required)
├── sensor.c        (required)
├── sensor.h        (required)
└── test_sensor.c   (required)
```

---

## Exercise_3 [build]

### Problem Statement

**Mock the Hardware (FFF)**

**Scenario:**
You need to test a high-level `command_handler.c` module. However, this module talks directly to the hardware using `uart_hal_transmit()` and `uart_hal_receive()`. Because you are testing on your laptop, you don't have real UART hardware. You must use the Fake Function Framework (FFF) to mock the hardware layer.

**Requirements:**

1. Review the `uart_hal.h` and `command_handler.c` provided in the Design Hints.
2. Define FFF fakes for `uart_hal_receive` and `uart_hal_transmit` inside a `fake_uart.h` file.
3. Write `test_handler.c` with two test cases:
   - `test_echo_success`: The fake `receive` function returns 5 bytes. Verify that the `transmit` function was called exactly once (`call_count == 1`).
   - `test_echo_no_data`: The fake `receive` function returns 0 bytes. Verify that the `transmit` function was **NOT** called (`call_count == 0`).
4. Write a `CMakeLists.txt` that fetches FFF and Unity, compiles the test, and registers it with CTest. **Do NOT link a real `uart_hal.c` implementation to the test executable.**

**Rules:**
- Follow BARR-C coding style.
- **Code Documentation:** All functions MUST be fully documented using Doxygen-style comments.
- Compile with strict flags: `-Wall -Wextra -pedantic -Werror -std=c11`.
- Use `cppcheck` and `clang-tidy` to analyze, and make sure there are no warning or error messages.

### Coding Standards Reference

**CERT-C 2016 (Security):**
| Rule | Relevance to This Exercise |
|---|---|
| ERR33-C | Check library function return values. Verified by the fact that your SUT correctly checks the returned length from `uart_hal_receive()` before blindly transmitting data. |

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs (under `C_Books/`)
> and read the full description of each rule above. After writing your code,
> verify your implementation follows these rules.

### Design Hints (optional)

Use these snippets to simulate the production code:

**1. `uart_hal.h` (Hardware Abstraction Layer)**
```c
#ifndef UART_HAL_H
#define UART_HAL_H
#include <stdint.h>
#include <stdbool.h>

uint32_t uart_hal_receive(uint8_t *buf, uint32_t max_len);
bool uart_hal_transmit(const uint8_t *buf, uint32_t len);

#endif // UART_HAL_H
```

**2. `command_handler.c` (System Under Test)**
```c
#include "uart_hal.h"

bool cmd_process_echo(void) {
    uint8_t buffer[64];
    uint32_t bytes_read = uart_hal_receive(buffer, sizeof(buffer));
    
    if (bytes_read > 0) {
        return uart_hal_transmit(buffer, bytes_read);
    }
    return false;
}
```

**3. `fake_uart.h` (Your Mock Definitions)**
```c
#ifndef FAKE_UART_H
#define FAKE_UART_H
#include "fff.h"
#include "uart_hal.h"

DECLARE_FAKE_VALUE_FUNC(uint32_t, uart_hal_receive, uint8_t*, uint32_t);
DECLARE_FAKE_VALUE_FUNC(bool, uart_hal_transmit, const uint8_t*, uint32_t);

#endif // FAKE_UART_H
```

### Acceptance Criteria (Scoring)

- **[20%]** CMake builds the test correctly, effectively using the "Link Seam" to swap the real hardware implementation for the FFF fake.
- **[30%]** The FFF macros are correctly initialized in the test file (`DEFINE_FFF_GLOBALS` and `RESET_FAKE`).
- **[30%]** Both tests use Unity assertions to explicitly verify the `call_count` property of the `transmit` fake.
- **[20%]** Code passes static analysis and all CTest tests pass.

### Expected Output

When running `cmake --build build && ctest --test-dir build -V`, the output must show all tests passing:

```
test_handler.c:20:test_echo_success:PASS
test_handler.c:30:test_echo_no_data:PASS

100% tests passed, 0 tests failed out of 1
```

### Submission

```
Exercise_3/
├── CMakeLists.txt      (required)
├── command_handler.c   (required)
├── fake_uart.h         (required)
├── uart_hal.h          (required)
└── test_handler.c      (required)
```

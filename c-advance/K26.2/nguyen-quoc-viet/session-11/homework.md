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

# Assignment — Session 11: Numerical Computations, Precision & Network Serialization
**Deadline: 2026-08-23 23:59:00**

> [!NOTE]
> **Build System:** All homework exercises use **CMake** instead of Makefile, and must include **Unity unit tests** (as established in Lecture 9).
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

**IEEE-754 Float Inspector & Safety Checker**

**Scenario:**
In embedded systems, floating-point bugs are silent killers — they compile without warnings, pass basic tests, and then crash a $370M rocket (Ariane 5, 1996) or cause a missile to miss its target (Patriot, 1991). Before you can avoid float traps, you must understand how floats are physically stored in memory.

**Requirements:**
Write a C program that inspects and validates IEEE-754 floating-point numbers:

1. **Bit Layout Inspector:** Implement a function `void inspect_float(float value)` that uses a `union` of `float` and `uint32_t` to extract and print the Sign bit (1 bit), Exponent field (8 bits, biased by 127), and Mantissa field (23 bits) of any `float` value.
   - Test with: `1.0f`, `-2.5f`, `0.0f`, `-0.0f`.

2. **Special Value Detector:** Implement `const char* classify_float(float value)` that returns a string classification:
   - `"+Infinity"` or `"-Infinity"` if `isinf(value)` is true.
   - `"NaN"` if `isnan(value)` is true.
   - `"Negative Zero"` if the value is `-0.0f` (check the sign bit via the union).
   - `"Normal"` otherwise.
   - Test with: `1.0f / 0.0f`, `-1.0f / 0.0f`, `0.0f / 0.0f`, `sqrtf(-1.0f)`, `-0.0f`, `42.0f`.

3. **Epsilon Comparison:** Implement `bool float_equal(float a, float b, float epsilon)` that returns `true` if `fabsf(a - b) < epsilon`. Demonstrate that `0.1f + 0.2f == 0.3f` fails with `==`, but passes with your epsilon function.

4. **The `memcmp` Trap:** Demonstrate that `memcmp(&neg_zero, &pos_zero, sizeof(float))` returns non-zero even though `-0.0f == 0.0f` is `true` mathematically. Print both results.

5. **Float Loop Counter Trap:** Show a loop `for (float f = 0.0f; f < 1.0f; f += 0.1f)` counting how many iterations it actually runs (it won't be 10!). Then show the safe integer-counter alternative.

**Rules:**
- Follow BARR-C coding style (fixed-width integers, mandatory braces, pointer naming with `p_` prefix).
- **Code Documentation:** All functions and data structures MUST be fully documented using Doxygen-style comments.
- Use `cppcheck` and `clang-tidy` to analyze, and make sure there are no warning or error messages.
- Build with CMake. Strict compiler flags (`-Wall -Wextra -pedantic -Werror -std=c11`) must be applied via `target_compile_options()`.
- Include Unity unit tests (see Expected Unit Test Output).
- You MUST `#include <math.h>` and link with `-lm` for math functions.
- Use `f` suffix on all single-precision constants (e.g., `3.14f` not `3.14`).

### Coding Standards Reference

**MISRA-C 2012 (Safety):**
| Rule | Category | Relevance to This Exercise |
|---|---|---|
| Rule 13.3 | Advisory | A full expression containing an increment or decrement shall have no other potential side effects → avoid `==` on float expressions. |
| Rule 14.1 | Required | A loop counter shall not have essentially floating type → the float loop counter trap demo. |
| Directive 4.11 | Required | Validate values passed to library functions → validate inputs to `sqrtf()` before calling. |

**CERT-C 2016 (Security):**
| Rule | Relevance to This Exercise |
|---|---|
| FLP32-C | Prevent or detect domain and range errors in math functions → pre-check `sqrtf(-1)` scenario. |
| FLP36-C | Preserve precision when converting integral values to floating-point → always use `f` suffix on float constants. |

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs (under `C_Books/`) and read the full description of each rule above. After writing your code, verify your implementation follows these rules.

### Design Hints (optional)

**Source structure:**
```
Exercise_1/
├── CMakeLists.txt
├── src/
│   ├── main.c                     # Main program with printf demos
│   └── float_inspector.c          # Float inspection logic
├── include/
│   └── float_inspector.h          # Function prototypes & union typedef
└── test/
    └── test_float_inspector.c     # Unity tests
```

**Example type and function prototypes (`float_inspector.h`):**
```c
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Union for inspecting IEEE-754 float bit layout.
 */
typedef union {
    float    f;
    uint32_t u;
    struct {
        uint32_t mantissa : 23;
        uint32_t exponent : 8;
        uint32_t sign     : 1;
    } bits;
} float_inspector_t;

/**
 * @brief Print the Sign, Exponent, and Mantissa of a float.
 * @param[in] value  The float to inspect.
 */
void inspect_float(float value);

/**
 * @brief Classify a float as Normal, +Infinity, -Infinity, NaN, or Negative Zero.
 * @param[in] value  The float to classify.
 * @return A string label for the float's classification.
 */
const char* classify_float(float value);

/**
 * @brief Compare two floats using epsilon tolerance.
 * @param[in] a       First float.
 * @param[in] b       Second float.
 * @param[in] epsilon Tolerance threshold.
 * @return true if |a - b| < epsilon, false otherwise.
 */
bool float_equal(float a, float b, float epsilon);
```

**CMakeLists.txt hint:**
```cmake
cmake_minimum_required(VERSION 3.14)
project(exercise_1 C)

add_compile_options(-Wall -Wextra -pedantic -Werror -std=c11)

# Main executable
add_executable(exercise_1 src/main.c src/float_inspector.c)
target_include_directories(exercise_1 PRIVATE include)
target_link_libraries(exercise_1 PRIVATE m)

# Unity test framework
include(FetchContent)
FetchContent_Declare(unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG        v2.6.0
)
FetchContent_MakeAvailable(unity)

# Test executable
enable_testing()
add_executable(test_exercise_1 test/test_float_inspector.c src/float_inspector.c)
target_include_directories(test_exercise_1 PRIVATE include)
target_link_libraries(test_exercise_1 PRIVATE unity m)
add_test(NAME test_exercise_1 COMMAND test_exercise_1)
```

### Acceptance Criteria (Scoring)

- **[10%]** CMake project builds successfully (`cmake -S . -B build && cmake --build build`).
- **[10%]** All Unity tests pass (`ctest --test-dir build -V`).
- **[15%]** Code passes static analysis (`cppcheck`, `clang-tidy`).
- **[15%]** Code contains required Doxygen documentation.
- **[50%]** Logic correctly inspects bit layout, classifies special values, demonstrates epsilon comparison, memcmp trap, and float loop counter trap.

### Expected Output

When run via `./build/exercise_1`:

```
========== L11 Exercise 1: IEEE-754 Float Inspector ==========

--- Bit Layout ---
Value: 1.000000   | Sign: 0 | Exp: 127 (Actual: 0)  | Mantissa: 0x000000
Value: -2.500000  | Sign: 1 | Exp: 128 (Actual: 1)  | Mantissa: 0x200000
Value: 0.000000   | Sign: 0 | Exp: 0   (Actual: -127)| Mantissa: 0x000000
Value: -0.000000  | Sign: 1 | Exp: 0   (Actual: -127)| Mantissa: 0x000000

--- Special Value Classification ---
1.0 / 0.0   => +Infinity
-1.0 / 0.0  => -Infinity
0.0 / 0.0   => NaN
sqrt(-1.0)  => NaN
-0.0        => Negative Zero
42.0        => Normal

--- Epsilon Comparison ---
0.1 + 0.2 == 0.3 ? NO  (raw == fails!)
float_equal(0.1+0.2, 0.3, 1e-6) ? YES  (epsilon passes)

--- memcmp Trap ---
-0.0 == 0.0 ? YES (math equality)
memcmp(-0.0, 0.0) == 0 ? NO (different bit patterns!)

--- Float Loop Counter Trap ---
Float loop ran 11 times (expected 10!)
Integer loop ran 10 times (correct)
```

Exit code: `0` on success, non-zero on error.

### Expected Unit Test Output

When run via `ctest --test-dir build -V`:

```
test_float_inspector.c:XX:test_classify_positive_infinity:PASS
test_float_inspector.c:XX:test_classify_negative_infinity:PASS
test_float_inspector.c:XX:test_classify_nan:PASS
test_float_inspector.c:XX:test_classify_negative_zero:PASS
test_float_inspector.c:XX:test_classify_normal:PASS
test_float_inspector.c:XX:test_float_equal_within_epsilon:PASS
test_float_inspector.c:XX:test_float_equal_outside_epsilon:PASS
test_float_inspector.c:XX:test_inspect_sign_bit:PASS

-----------------------
8 Tests 0 Failures 0 Ignored
OK
```

### Submission

```
Exercise_1/
├── CMakeLists.txt                  (required)
├── src/
│   ├── main.c                      (required)
│   └── float_inspector.c           (required)
├── include/
│   └── float_inspector.h           (required)
└── test/
    └── test_float_inspector.c      (required — Unity unit tests)
```

---

## Exercise_2 [build]

### Problem Statement

**Fixed-Point Audio DSP Pipeline (Q8.8 Format)**

**Scenario:**
You are writing firmware for a Cortex-M0 microcontroller that has **no hardware FPU**. The device must apply a gain factor to 16-bit audio samples in real time. Using `float` would link in a huge software emulation library (`libgcc`), making your firmware too slow and too large. You must use **fixed-point Q8.8 arithmetic** instead.

**Requirements:**
Write a C program that processes audio samples using the 4-stage fixed-point pipeline:

1. **Stage 1 — ACQUIRE (Convert In):** Implement `int16_t float_to_q8_8(float value)` that converts a floating-point gain factor (e.g., `1.75f`) into Q8.8 fixed-point representation (multiply by 256 and cast to `int16_t`).

2. **Stage 2 — PROCESS (Integer Domain):** Implement `int16_t q8_8_multiply(int16_t a, int16_t b)` that multiplies two Q8.8 values using a 32-bit intermediate to prevent overflow, then shifts back down by 8 bits.

3. **Stage 3 — DECIDE (Integer Logic):** Implement `bool q8_8_exceeds_threshold(int16_t sample, int16_t threshold)` that compares a Q8.8 sample against a threshold — entirely in the integer domain.

4. **Stage 4 — OUTPUT (Convert Out):** Implement `float q8_8_to_float(int16_t q_value)` that converts a Q8.8 value back to float for display purposes only.

5. **Full Pipeline Demo:** Given an array of raw audio samples `int16_t samples[] = {100, 200, -150, 32767, -32768, 0}`:
   - Convert each sample to Q8.8 (shift left by 8).
   - Apply a gain of `1.75` (in Q8.8 = `448`) to each sample using `q8_8_multiply`.
   - Check if the gained sample exceeds a threshold of `300.0` (in Q8.8 = `76800`).
   - Convert the result back to float for printing.
   - Print a comparison table: Original → Q8.8 → Gained → Float Result → Exceeds Threshold?

6. **Performance Note:** Print the sizes of `float` vs `int16_t` and explain in a comment why Q8.8 is preferred on Cortex-M0.

**Rules:**
- Follow BARR-C coding style (fixed-width integers, mandatory braces, pointer naming with `p_` prefix).
- **Code Documentation:** All functions and data structures MUST be fully documented using Doxygen-style comments.
- Use `cppcheck` and `clang-tidy` to analyze, and make sure there are no warning or error messages.
- Build with CMake. Strict compiler flags (`-Wall -Wextra -pedantic -Werror -std=c11`) must be applied via `target_compile_options()`.
- Include Unity unit tests (see Expected Unit Test Output).
- Do NOT use `float` in the processing pipeline (Stages 2–3). Only use float in Stage 1 (conversion in) and Stage 4 (display).

### Coding Standards Reference

**MISRA-C 2012 (Safety):**
| Rule | Category | Relevance to This Exercise |
|---|---|---|
| Rule 10.3 | Required | The value of an expression shall not be assigned to an object with a narrower essential type → cast carefully when shifting Q8.8 values. |
| Rule 10.4 | Required | Both operands of an operator in which the usual arithmetic conversions are performed shall have the same essential type category → ensure consistent int16_t/int32_t usage. |
| Rule 12.1 | Advisory | The precedence of operators within expressions should be made explicit → use parentheses in shift/multiply expressions. |

**CERT-C 2016 (Security):**
| Rule | Relevance to This Exercise |
|---|---|
| INT32-C | Ensure that operations on signed integers do not result in overflow → use int32_t intermediate for Q8.8 multiplication. |
| INT31-C | Ensure that integer conversions do not result in lost or misinterpreted data → validate range after Q8.8 gain application. |

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs (under `C_Books/`) and read the full description of each rule above. After writing your code, verify your implementation follows these rules.

### Design Hints (optional)

**Source structure:**
```
Exercise_2/
├── CMakeLists.txt
├── src/
│   ├── main.c                     # Main program: pipeline demo with comparison table
│   └── fixed_point.c              # Q8.8 conversion & arithmetic functions
├── include/
│   └── fixed_point.h              # Macros, prototypes
└── test/
    └── test_fixed_point.c         # Unity tests
```

**Example macros and prototypes (`fixed_point.h`):**
```c
#include <stdint.h>
#include <stdbool.h>

#define Q8_8_SHIFT     8
#define Q8_8_SCALE     (1 << Q8_8_SHIFT)   /* 256 */

/** @brief Convert float to Q8.8 fixed-point. */
int16_t float_to_q8_8(float value);

/** @brief Convert Q8.8 fixed-point back to float (for display). */
float q8_8_to_float(int16_t q_value);

/** @brief Multiply two Q8.8 values with 32-bit intermediate. */
int16_t q8_8_multiply(int16_t a, int16_t b);

/** @brief Check if a Q8.8 value exceeds a threshold. */
bool q8_8_exceeds_threshold(int16_t sample, int16_t threshold);
```

**CMakeLists.txt hint:**
```cmake
cmake_minimum_required(VERSION 3.14)
project(exercise_2 C)

add_compile_options(-Wall -Wextra -pedantic -Werror -std=c11)

add_executable(exercise_2 src/main.c src/fixed_point.c)
target_include_directories(exercise_2 PRIVATE include)

# Unity
include(FetchContent)
FetchContent_Declare(unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG        v2.6.0
)
FetchContent_MakeAvailable(unity)

enable_testing()
add_executable(test_exercise_2 test/test_fixed_point.c src/fixed_point.c)
target_include_directories(test_exercise_2 PRIVATE include)
target_link_libraries(test_exercise_2 PRIVATE unity)
add_test(NAME test_exercise_2 COMMAND test_exercise_2)
```

### Acceptance Criteria (Scoring)

- **[10%]** CMake project builds successfully (`cmake -S . -B build && cmake --build build`).
- **[10%]** All Unity tests pass (`ctest --test-dir build -V`).
- **[15%]** Code passes static analysis (`cppcheck`, `clang-tidy`).
- **[15%]** Code contains required Doxygen documentation.
- **[50%]** Logic correctly implements Q8.8 conversion, multiplication with 32-bit intermediate, threshold comparison, and the full 4-stage pipeline. No `float` usage in Stages 2–3.

### Expected Output

When run via `./build/exercise_2`:

```
========== L11 Exercise 2: Fixed-Point Audio DSP (Q8.8) ==========

--- Q8.8 Gain = 1.75 (Q8.8 = 448) ---

| Original | Q8.8 Raw | Gained   | Float Result | > 300.0? |
|----------|----------|----------|--------------|----------|
|      100 |    25600 |    44800 |       175.00 |       NO |
|      200 |    51200 |    89600 |       350.00 |      YES |
|     -150 |   -38400 |   -67200 |      -262.50 |       NO |
|    32767 |  overflow|        — |            — |        — |
|   -32768 |  overflow|        — |            — |        — |
|        0 |        0 |        0 |         0.00 |       NO |

--- Type Sizes ---
sizeof(float)  = 4 bytes
sizeof(int16_t)= 2 bytes
Note: On Cortex-M0 without FPU, int16_t ops are native hardware instructions.
```

Exit code: `0` on success, non-zero on error.

### Expected Unit Test Output

When run via `ctest --test-dir build -V`:

```
test_fixed_point.c:XX:test_float_to_q8_8_positive:PASS
test_fixed_point.c:XX:test_float_to_q8_8_negative:PASS
test_fixed_point.c:XX:test_float_to_q8_8_zero:PASS
test_fixed_point.c:XX:test_q8_8_to_float_roundtrip:PASS
test_fixed_point.c:XX:test_q8_8_multiply_positive:PASS
test_fixed_point.c:XX:test_q8_8_multiply_negative:PASS
test_fixed_point.c:XX:test_q8_8_exceeds_threshold_true:PASS
test_fixed_point.c:XX:test_q8_8_exceeds_threshold_false:PASS

-----------------------
8 Tests 0 Failures 0 Ignored
OK
```

### Submission

```
Exercise_2/
├── CMakeLists.txt                  (required)
├── src/
│   ├── main.c                      (required)
│   └── fixed_point.c               (required)
├── include/
│   └── fixed_point.h               (required)
└── test/
    └── test_fixed_point.c          (required — Unity unit tests)
```

---

## Exercise_3 [build]

### Problem Statement

**Endian-Safe Telemetry Serializer with TLV Framing**

**Scenario:**
You are developing firmware for an IoT sensor node that transmits telemetry data to a cloud gateway over TCP. The sensor runs on a Little-Endian ARM Cortex-M4, while the gateway runs on a Big-Endian PowerPC. You must serialize the telemetry struct into a portable byte stream that both endpoints can correctly interpret.

**Requirements:**
Write a C program that implements two serialization techniques:

**Part A — Fixed-Layout Serialization (`memcpy` + `htonl`):**

1. Define a telemetry struct:
   ```c
   typedef struct {
       uint32_t device_id;
       float    temperature;   /* degrees Celsius */
       float    humidity;      /* percentage */
   } telemetry_t;
   ```

2. Implement `size_t serialize_telemetry(const telemetry_t *p_src, uint8_t *p_buf)`:
   - Serialize `device_id` using `htonl()` + `memcpy()`.
   - Serialize `temperature` and `humidity` by first copying float bits to `uint32_t` via `memcpy()` (to avoid strict aliasing violation), then applying `htonl()`, then copying to the buffer.
   - Return the number of bytes written (should be exactly 12).

3. Implement `void deserialize_telemetry(const uint8_t *p_buf, telemetry_t *p_dst)`:
   - Reverse the process: `memcpy()` from buffer → `ntohl()` → `memcpy()` back to float.

4. Demonstrate a round-trip: serialize → print hex dump → deserialize → verify fields match.

**Part B — TLV Framing (Type-Length-Value):**

5. Define TLV type constants:
   ```c
   #define TLV_TYPE_DEVICE_ID    0x01
   #define TLV_TYPE_TEMPERATURE  0x02
   #define TLV_TYPE_HUMIDITY     0x03
   ```

6. Implement `size_t serialize_telemetry_tlv(const telemetry_t *p_src, uint8_t *p_buf)`:
   - Encode each field as a TLV triplet: `[Type: 1 byte][Length: 1 byte][Value: N bytes]`.
   - Values must be in Network Byte Order.

7. Implement `int32_t deserialize_telemetry_tlv(const uint8_t *p_buf, size_t buf_len, telemetry_t *p_dst)`:
   - Parse TLV entries in a loop. **Validate** that `offset + length <= buf_len` before reading each value (Heartbleed-style defense).
   - Skip unknown TLV types gracefully (forward compatibility).
   - Return `0` on success, `-1` on validation failure.

8. Demonstrate that TLV deserialization correctly handles a buffer containing an unknown TLV type (e.g., type `0xFF`) by skipping it.

**Rules:**
- Follow BARR-C coding style (fixed-width integers, mandatory braces, pointer naming with `p_` prefix).
- **Code Documentation:** All functions and data structures MUST be fully documented using Doxygen-style comments.
- Use `cppcheck` and `clang-tidy` to analyze, and make sure there are no warning or error messages.
- Build with CMake. Strict compiler flags (`-Wall -Wextra -pedantic -Werror -std=c11`) must be applied via `target_compile_options()`.
- Include Unity unit tests (see Expected Unit Test Output).
- Never cast a `float*` to `uint32_t*` directly — always use `memcpy()` for type-punning.

### Coding Standards Reference

**MISRA-C 2012 (Safety):**
| Rule | Category | Relevance to This Exercise |
|---|---|---|
| Rule 11.3 | Required | A cast shall not be performed between a pointer to object type and a pointer to a different object type → use `memcpy()` for float↔uint32_t, never pointer casts. |
| Directive 4.14 | Required | The validity of values received from external sources shall be checked → validate TLV length fields before reading. |
| Rule 18.1 | Required | A pointer resulting from arithmetic on a pointer operand shall address an element of the same array → bounds-check buffer offsets in TLV parsing. |

**CERT-C 2016 (Security):**
| Rule | Relevance to This Exercise |
|---|---|
| EXP39-C | Do not access a variable through a pointer of an incompatible type → strict aliasing rule; use `memcpy()` for float byte-swapping. |
| ARR30-C | Do not form or use out-of-bounds pointers or array subscripts → TLV buffer length validation. |
| MEM04-C | Do not perform zero-length allocations → validate TLV length > 0 before processing. |

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs (under `C_Books/`) and read the full description of each rule above. After writing your code, verify your implementation follows these rules.

### Design Hints (optional)

**Source structure:**
```
Exercise_3/
├── CMakeLists.txt
├── src/
│   ├── main.c                     # Main program: round-trip demos for both techniques
│   └── telemetry_codec.c          # serialize/deserialize functions (fixed + TLV)
├── include/
│   └── telemetry_codec.h          # Struct definition, TLV constants, prototypes
└── test/
    └── test_telemetry_codec.c     # Unity tests
```

**Example prototypes (`telemetry_codec.h`):**
```c
#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint32_t device_id;
    float    temperature;
    float    humidity;
} telemetry_t;

#define TLV_TYPE_DEVICE_ID    0x01
#define TLV_TYPE_TEMPERATURE  0x02
#define TLV_TYPE_HUMIDITY     0x03

/* Part A: Fixed-layout */
size_t serialize_telemetry(const telemetry_t *p_src, uint8_t *p_buf);
void   deserialize_telemetry(const uint8_t *p_buf, telemetry_t *p_dst);

/* Part B: TLV */
size_t  serialize_telemetry_tlv(const telemetry_t *p_src, uint8_t *p_buf);
int32_t deserialize_telemetry_tlv(const uint8_t *p_buf, size_t buf_len,
                                   telemetry_t *p_dst);
```

**CMakeLists.txt hint:**
```cmake
cmake_minimum_required(VERSION 3.14)
project(exercise_3 C)

add_compile_options(-Wall -Wextra -pedantic -Werror -std=c11)

add_executable(exercise_3 src/main.c src/telemetry_codec.c)
target_include_directories(exercise_3 PRIVATE include)

# Unity
include(FetchContent)
FetchContent_Declare(unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG        v2.6.0
)
FetchContent_MakeAvailable(unity)

enable_testing()
add_executable(test_exercise_3 test/test_telemetry_codec.c src/telemetry_codec.c)
target_include_directories(test_exercise_3 PRIVATE include)
target_link_libraries(test_exercise_3 PRIVATE unity)
add_test(NAME test_exercise_3 COMMAND test_exercise_3)
```

### Acceptance Criteria (Scoring)

- **[10%]** CMake project builds successfully (`cmake -S . -B build && cmake --build build`).
- **[10%]** All Unity tests pass (`ctest --test-dir build -V`).
- **[15%]** Code passes static analysis (`cppcheck`, `clang-tidy`).
- **[15%]** Code contains required Doxygen documentation.
- **[50%]** Logic correctly serializes/deserializes using both fixed-layout and TLV techniques, validates TLV lengths, handles unknown TLV types, and never uses pointer casts for float↔uint32_t conversion.

### Expected Output

When run via `./build/exercise_3`:

```
========== L11 Exercise 3: Telemetry Serializer ==========

--- Part A: Fixed-Layout (memcpy + htonl) ---
Original: id=4096, temp=24.50, hum=60.20
Serialized (12 bytes): 00 00 10 00 41 C4 00 00 42 70 CC CD
Deserialized: id=4096, temp=24.50, hum=60.20
Round-trip: PASS

--- Part B: TLV Framing ---
TLV Serialized (18 bytes): 01 04 00 00 10 00 02 04 41 C4 00 00 03 04 42 70 CC CD
TLV Deserialized: id=4096, temp=24.50, hum=60.20
TLV Round-trip: PASS

--- TLV Forward Compatibility ---
Buffer with unknown type 0xFF: Skipped gracefully.
Known fields decoded correctly: id=4096, temp=24.50
```

Exit code: `0` on success, non-zero on error.

### Expected Unit Test Output

When run via `ctest --test-dir build -V`:

```
test_telemetry_codec.c:XX:test_serialize_fixed_size:PASS
test_telemetry_codec.c:XX:test_roundtrip_fixed_layout:PASS
test_telemetry_codec.c:XX:test_float_byte_swap_safe:PASS
test_telemetry_codec.c:XX:test_serialize_tlv_size:PASS
test_telemetry_codec.c:XX:test_roundtrip_tlv:PASS
test_telemetry_codec.c:XX:test_tlv_rejects_short_buffer:PASS
test_telemetry_codec.c:XX:test_tlv_skips_unknown_type:PASS
test_telemetry_codec.c:XX:test_tlv_null_pointer:PASS

-----------------------
8 Tests 0 Failures 0 Ignored
OK
```

### Submission

```
Exercise_3/
├── CMakeLists.txt                  (required)
├── src/
│   ├── main.c                      (required)
│   └── telemetry_codec.c           (required)
├── include/
│   └── telemetry_codec.h           (required)
└── test/
    └── test_telemetry_codec.c      (required — Unity unit tests)
```

---

## Exercise_4 [build]

### Problem Statement

**Generic Sensor Registry with `qsort()` & `bsearch()`**

**Scenario:**
Your embedded device has 8 sensors. Each sensor has a unique ID and a calibration offset. On boot, the firmware loads the sensor metadata into an array — but the array is unsorted (loaded from flash in arbitrary order). You need to sort it for fast binary-search lookups during real-time operation.

**Requirements:**
Write a C program:

1. Define a sensor metadata struct:
   ```c
   typedef struct {
       uint32_t sensor_id;
       char     name[32];
       float    calibration_offset;
   } sensor_metadata_t;
   ```

2. Initialize an unsorted array of 8 sensors with realistic data (e.g., IDs: 1042, 1007, 1099, 1023, 1001, 1088, 1055, 1034).

3. **Safe Comparator:** Implement `int compare_by_id(const void *p_a, const void *p_b)` using the safe three-way comparison pattern (never `return *(int*)a - *(int*)b` — explain why in a comment).

4. **Sort:** Use `qsort()` to sort the array by `sensor_id`. Print the array before and after sorting.

5. **Search:** Use `bsearch()` to look up a sensor by ID. Test with:
   - A valid ID that exists in the array → print the sensor's name and calibration offset.
   - An invalid ID that does not exist → print "Sensor not found."

6. **The Subtraction Trap Demo:** Include a commented-out "dangerous comparator" showing `return *(int*)a - *(int*)b` with a comment explaining how `INT32_MAX - (-1)` overflows and produces a wrong sign, breaking `qsort()`.

**Rules:**
- Follow BARR-C coding style (fixed-width integers, mandatory braces, pointer naming with `p_` prefix).
- **Code Documentation:** All functions and data structures MUST be fully documented using Doxygen-style comments.
- Use `cppcheck` and `clang-tidy` to analyze, and make sure there are no warning or error messages.
- Build with CMake. Strict compiler flags (`-Wall -Wextra -pedantic -Werror -std=c11`) must be applied via `target_compile_options()`.
- Include Unity unit tests (see Expected Unit Test Output).
- The array MUST be sorted before calling `bsearch()`. Do not assume pre-sorted data.

### Coding Standards Reference

**MISRA-C 2012 (Safety):**
| Rule | Category | Relevance to This Exercise |
|---|---|---|
| Rule 9.1 | Mandatory | The value of an object with automatic storage duration shall not be read before it has been set → initialize all struct fields before use. |
| Rule 11.5 | Advisory | A conversion should not be performed from pointer to void into pointer to object → safe casting of `void*` in comparator and bsearch result. |

**CERT-C 2016 (Security):**
| Rule | Relevance to This Exercise |
|---|---|
| INT32-C | Ensure that operations on signed integers do not result in overflow → the comparator subtraction trap. |
| EXP34-C | Do not dereference null pointers → check `bsearch()` return value before use. |
| ARR30-C | Do not form or use out-of-bounds pointers → safe access after `bsearch()`. |

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs (under `C_Books/`) and read the full description of each rule above. After writing your code, verify your implementation follows these rules.

### Design Hints (optional)

**Source structure:**
```
Exercise_4/
├── CMakeLists.txt
├── src/
│   ├── main.c                     # Main program: sort, search, print
│   └── sensor_registry.c          # Comparator, lookup wrapper
├── include/
│   └── sensor_registry.h          # Struct definition, prototypes
└── test/
    └── test_sensor_registry.c     # Unity tests
```

**Example prototypes (`sensor_registry.h`):**
```c
#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint32_t sensor_id;
    char     name[32];
    float    calibration_offset;
} sensor_metadata_t;

/** @brief Safe comparator for qsort/bsearch by sensor_id. */
int compare_by_id(const void *p_a, const void *p_b);

/**
 * @brief Look up a sensor by ID in a sorted array.
 * @param[in] p_registry  Sorted array of sensors.
 * @param[in] count       Number of sensors.
 * @param[in] target_id   The sensor ID to find.
 * @return Pointer to the matching sensor, or NULL if not found.
 */
const sensor_metadata_t* find_sensor(const sensor_metadata_t *p_registry,
                                      size_t count, uint32_t target_id);
```

**CMakeLists.txt hint:**
(Use the same CMake + FetchContent pattern as the previous exercises).

### Acceptance Criteria (Scoring)

- **[10%]** CMake project builds successfully (`cmake -S . -B build && cmake --build build`).
- **[10%]** All Unity tests pass (`ctest --test-dir build -V`).
- **[15%]** Code passes static analysis (`cppcheck`, `clang-tidy`).
- **[15%]** Code contains required Doxygen documentation.
- **[50%]** Logic correctly sorts the array using `qsort()`, finds elements with `bsearch()`, handles not-found cases, uses a safe comparator, and documents the subtraction trap.

### Expected Output

When run via `./build/exercise_4`:

```
========== L11 Exercise 4: Sensor Registry (qsort + bsearch) ==========

--- Before Sorting ---
[0] ID: 1042  Name: Pressure_A     Offset: +0.120
[1] ID: 1007  Name: Temp_Inlet      Offset: -0.340
[2] ID: 1099  Name: Flow_Main       Offset: +0.050
[3] ID: 1023  Name: Voltage_Bus     Offset: +0.000
[4] ID: 1001  Name: Ambient_Temp    Offset: -0.150
[5] ID: 1088  Name: Humidity_Ext    Offset: +0.210
[6] ID: 1055  Name: Current_Motor   Offset: -0.080
[7] ID: 1034  Name: Pressure_B      Offset: +0.095

--- After Sorting (by sensor_id) ---
[0] ID: 1001  Name: Ambient_Temp    Offset: -0.150
[1] ID: 1007  Name: Temp_Inlet      Offset: -0.340
[2] ID: 1023  Name: Voltage_Bus     Offset: +0.000
[3] ID: 1034  Name: Pressure_B      Offset: +0.095
[4] ID: 1042  Name: Pressure_A      Offset: +0.120
[5] ID: 1055  Name: Current_Motor   Offset: -0.080
[6] ID: 1088  Name: Humidity_Ext    Offset: +0.210
[7] ID: 1099  Name: Flow_Main       Offset: +0.050

--- Lookup Tests ---
Search for ID 1055: FOUND -> Current_Motor (offset: -0.080)
Search for ID 9999: NOT FOUND
```

Exit code: `0` on success, non-zero on error.

### Expected Unit Test Output

When run via `ctest --test-dir build -V`:

```
test_sensor_registry.c:XX:test_comparator_less_than:PASS
test_sensor_registry.c:XX:test_comparator_greater_than:PASS
test_sensor_registry.c:XX:test_comparator_equal:PASS
test_sensor_registry.c:XX:test_find_sensor_existing:PASS
test_sensor_registry.c:XX:test_find_sensor_not_found:PASS
test_sensor_registry.c:XX:test_find_sensor_first_element:PASS
test_sensor_registry.c:XX:test_find_sensor_last_element:PASS

-----------------------
7 Tests 0 Failures 0 Ignored
OK
```

### Submission

```
Exercise_4/
├── CMakeLists.txt                  (required)
├── src/
│   ├── main.c                      (required)
│   └── sensor_registry.c           (required)
├── include/
│   └── sensor_registry.h           (required)
└── test/
    └── test_sensor_registry.c      (required — Unity unit tests)
```

---

## Exercise_5 [build]

### Problem Statement

**Wide Character & Locale Demo (I18N Basics)**

**Scenario:**
Your embedded Linux device has an LCD display that must show multilingual status messages — including accented European characters (ä, ö, ü, é), mathematical symbols (Ω, π), and CJK characters. Standard `char` (ASCII, 8-bit) cannot represent these. You must use C's wide character support.

**Requirements:**
Write a C program:

1. **Locale Setup:** Call `setlocale(LC_ALL, "")` to enable the system's default locale. Print the active locale string.

2. **Wide String Basics:** Declare wide strings using the `L""` prefix:
   - `wchar_t greeting[] = L"Héllo Wörld! π Ω";`
   - Print it using `wprintf(L"...", ...)`.
   - Print `wcslen()` (character count) and `sizeof()` (byte count) and explain the difference.

3. **Wide Character Classification:** Use `<wctype.h>` functions to classify individual wide characters:
   - `iswalpha(L'Ω')` → should return true (alphabetic).
   - `iswdigit(L'5')` → should return true.
   - `iswspace(L' ')` → should return true.

4. **The Portability Trap:** Print `sizeof(wchar_t)` and explain in a comment:
   - On Linux/macOS: 4 bytes (UTF-32).
   - On Windows: 2 bytes (UTF-16).
   - Why this makes `wchar_t` non-portable for network transmission, and why UTF-8 (`char[]`) is the industry standard for data exchange.

5. **Wide String Operations:** Demonstrate:
   - `wcscpy()` — copy a wide string.
   - `wcscat()` — concatenate two wide strings.
   - `wcscmp()` — compare two wide strings.

**Rules:**
- Follow BARR-C coding style (fixed-width integers, mandatory braces, pointer naming with `p_` prefix).
- **Code Documentation:** All functions and data structures MUST be fully documented using Doxygen-style comments.
- Use `cppcheck` and `clang-tidy` to analyze, and make sure there are no warning or error messages.
- Build with CMake. Strict compiler flags (`-Wall -Wextra -pedantic -Werror -std=c11`) must be applied via `target_compile_options()`.
- Include Unity unit tests (see Expected Unit Test Output).
- Do NOT mix `printf` and `wprintf` in the same program (use only `wprintf` for wide output, or set stream orientation explicitly).

### Coding Standards Reference

**MISRA-C 2012 (Safety):**
| Rule | Category | Relevance to This Exercise |
|---|---|---|
| Rule 21.17 | Required | Use of the string handling functions from `<string.h>` shall not result in accesses beyond the bounds of the objects referenced → applies equally to wide string functions from `<wchar.h>`. |

**CERT-C 2016 (Security):**
| Rule | Relevance to This Exercise |
|---|---|
| STR31-C | Guarantee that storage for strings has sufficient space → applies to `wchar_t` arrays (remember each element is 4 bytes on Linux). |

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs (under `C_Books/`) and read the full description of each rule above. After writing your code, verify your implementation follows these rules.

### Design Hints (optional)

**Source structure:**
```
Exercise_5/
├── CMakeLists.txt
├── src/
│   ├── main.c                     # Main program with wprintf demos
│   └── wide_string_utils.c        # Wide string helper functions
├── include/
│   └── wide_string_utils.h        # Prototypes
└── test/
    └── test_wide_string_utils.c   # Unity tests
```

**CMakeLists.txt hint:**
(Use the same CMake + FetchContent pattern as the previous exercises).

### Acceptance Criteria (Scoring)

- **[10%]** CMake project builds successfully (`cmake -S . -B build && cmake --build build`).
- **[10%]** All Unity tests pass (`ctest --test-dir build -V`).
- **[15%]** Code passes static analysis (`cppcheck`, `clang-tidy`).
- **[15%]** Code contains required Doxygen documentation.
- **[50%]** Logic correctly uses `setlocale`, prints wide strings, classifies wide characters, demonstrates `sizeof(wchar_t)` portability trap, and performs wide string operations.

### Expected Output

When run via `./build/exercise_5`:

```
========== L11 Exercise 5: Wide Characters & I18N ==========

--- Locale ---
Active locale: en_US.UTF-8

--- Wide Strings ---
Greeting: Héllo Wörld! π Ω
wcslen = 16 characters
sizeof = 68 bytes (each wchar_t = 4 bytes on this platform)

--- Wide Character Classification ---
iswalpha(L'Ω') = YES (alphabetic)
iswdigit(L'5') = YES (digit)
iswspace(L' ') = YES (whitespace)

--- Portability Trap ---
sizeof(wchar_t) = 4 bytes
Note: On Windows this would be 2 bytes (UTF-16). Use UTF-8 for network/file exchange.

--- Wide String Operations ---
wcscpy: Héllo Wörld! π Ω
wcscat: Héllo Wörld! π Ω — Status OK
wcscmp("ABC", "ABC") = 0 (equal)
wcscmp("ABC", "DEF") < 0 (less than)
```

Exit code: `0` on success, non-zero on error.

### Expected Unit Test Output

When run via `ctest --test-dir build -V`:

```
test_wide_string_utils.c:XX:test_wcslen_counts_characters:PASS
test_wide_string_utils.c:XX:test_wcscmp_equal:PASS
test_wide_string_utils.c:XX:test_wcscmp_not_equal:PASS
test_wide_string_utils.c:XX:test_wcscpy_copies_correctly:PASS
test_wide_string_utils.c:XX:test_wchar_size_is_4_bytes:PASS

-----------------------
5 Tests 0 Failures 0 Ignored
OK
```

### Submission

```
Exercise_5/
├── CMakeLists.txt                  (required)
├── src/
│   ├── main.c                      (required)
│   └── wide_string_utils.c         (required)
├── include/
│   └── wide_string_utils.h         (required)
└── test/
    └── test_wide_string_utils.c    (required — Unity unit tests)
```

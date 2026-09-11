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

# Assignment — Session 07: Macros, Variadic Functions & Logging
**Deadline: 2026-08-09 23:59:00**

---

## Overview

This homework covers the five major topics from Lecture 7:

| Exercise | Topic | Difficulty |
|:---|:---|:---:|
| Exercise_1 | Macro Pitfalls — Parentheses, Double-Eval & `do-while(0)` | ★☆☆ |
| Exercise_2 | Variadic Functions — `stdarg.h` Min/Max/Avg | ★★☆ |
| Exercise_3 | Capstone — The Industrial Logger Module | ★★★ |
| Exercise_4 | Conditional Compilation — Feature Flags | ★★☆ |
| Exercise_5 | Token Pasting & Stringification (`##` and `#`) | ★★☆ |

> **Prerequisite**: Review `L7_labs/demo_macro_pitfalls.c` and `L7_labs/demo_inline_vs_macro.c` before starting.

---

## Exercise_1 [build]

### Problem Statement

**Safe Macros — Parentheses, Side Effects & `do-while(0)`**

**Scenario:**
In embedded C, function-like macros are used frequently to avoid function call overhead for small math utilities like `MIN`, `ABS`, and `CLAMP`. However, poorly written macros introduce subtle, hard-to-debug bugs at the call site. Your task is to write these macros correctly from the start.

**Requirements:**

Create a new file named `main.c` from scratch. Implement the following:

1. Define `MIN(a, b)`, `ABS(x)`, and `CLAMP(val, lo, hi)` using the 4 Rules of Macros:
   - Parenthesize all parameter uses and the full expression body.
   - Wrap multi-statement macros with `do { ... } while(0)`.
2. Implement `static inline uint32_t safe_min_u32(uint32_t a, uint32_t b)` as the type-safe alternative to a macro.
3. In `main()`, call each macro and the inline function with the inputs from the **Expected Output** section below and print the results.

**Rules:**
- Follow BARR-C coding style (fixed-width integers, mandatory braces).
- All functions MUST be fully documented using Doxygen-style comments.
- Use `cppcheck` and `clang-tidy` to analyze, and make sure there are no warning or error messages.
- Compile with strict flags: `-Wall -Wextra -pedantic -Werror -std=c99`.

### Coding Standards Reference

This exercise is a good opportunity to study the following industry rules. Read the rule, understand *why* it exists, and apply the pattern in your code.

**MISRA-C 2012 (Safety):**
| Rule | Category | Relevance to This Exercise |
|---|---|---|
| Rule 20.7 | Required | Expressions resulting from the expansion of macro parameters shall be enclosed in parentheses. |
| Directive 4.9 | Advisory | A function should be used in preference to a function-like macro where they are interchangeable (use `static inline`). |
| Rule 13.1 | Required | Initializer lists shall not contain persistent side effects (Double Evaluation hazard). |

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs (under `C_Books/`)
> and read the full description of each rule above. After writing your code,
> verify your implementation follows these rules.

### Design Hints (optional)

```c
/* Hint: MIN — must parenthesize both parameters AND the full body */
#define MIN(a, b)           /* your implementation */

/* Hint: ABS — works for signed integers */
#define ABS(x)              /* your implementation */

/* Hint: CLAMP — multi-statement, must use do { } while(0) */
#define CLAMP(val, lo, hi)  /* your implementation */

/**
 * @brief Type-safe minimum using static inline.
 *
 * @param a First value.
 * @param b Second value.
 * @return The smaller of the two values.
 */
static inline uint32_t safe_min_u32(uint32_t a, uint32_t b);
```

### Acceptance Criteria (Scoring)

- **[20%]** Code builds successfully without warnings or errors.
- **[20%]** Code passes `cppcheck` and `clang-tidy` with no warnings.
- **[20%]** Code contains required Doxygen documentation for all functions.
- **[20%]** `MIN`, `ABS`, and `CLAMP` macros produce the correct output shown below.
- **[20%]** `safe_min_u32` produces the correct output shown below.

### Expected Output

```
=== Exercise 1: Safe Macros ===
MIN(3, 5)            = 3
ABS(-7)              = 7
ABS(5 - 10)          = 5
CLAMP(15, 0, 10)     = 10
CLAMP(-3, 0, 10)     = 0
safe_min_u32(3, 5)   = 3
```

Exit code: `0` on success.

### Submission

```
Exercise_1/
├── main.c        (required)
└── Makefile      (required — targets: all, clean)
```

---

## Exercise_2 [build]

### Problem Statement

**Variadic Functions — `stdarg.h`**

**Scenario:**
Embedded APIs often need to accept a flexible number of arguments. For example, computing statistics over a dynamic number of sensor readings.

**Requirements:**

Implement `void compute_stats(uint32_t count, ...)` in a new `main.c` from scratch, using the standard variadic argument macros.

Behavior:
1. Initialize a `va_list`.
2. Use `va_start()` using the `count` parameter.
3. Iterate `count` times using `va_arg()`.
   - **Crucial Note:** Default argument promotions apply! Small integer types passed to variadic functions are promoted to `int`. You MUST read them as `va_arg(ap, int)` before casting/storing them into `int32_t`.
4. Calculate min, max, and average (sum / count).
5. Call `va_end()`.
6. Return the populated `stats_t` struct.

**Rules:**
- Follow BARR-C coding style.
- **Code Documentation:** All functions MUST be fully documented using Doxygen-style comments.
- Use `cppcheck` and `clang-tidy` to analyze, and make sure there are no warning or error messages.
- Compile with strict flags: `-Wall -Wextra -pedantic -Werror -std=c99`.

### Coding Standards Reference

**CERT-C 2016 (Security):**
| Rule | Relevance to This Exercise |
|---|---|
| EXP58-C | Call `va_start()` before accessing arguments, and `va_end()` before the function returns. |
| INT31-C | Ensure that integer conversions (e.g., pulling `int` from `va_arg` and casting to `int32_t`) do not result in lost or misinterpreted data. |

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs (under `C_Books/`)
> and read the full description of each rule above. After writing your code,
> verify your implementation follows these rules.

### Design Hints (optional)

```c
typedef struct {
    int32_t min;
    int32_t max;
    int32_t average;
} stats_t;

stats_t compute_stats(uint32_t count, ...) {
    stats_t result = {0, 0, 0};
    if (count == 0) return result;
    
    va_list ap;
    va_start(ap, count);
    // Loop and use va_arg(ap, int)
    va_end(ap);
    return result;
}
```

### Acceptance Criteria (Scoring)

- **[15%]** Code builds successfully without warnings or errors.
- **[15%]** Code passes static analysis without warnings.
- **[30%]** Correct usage of `va_start`, `va_arg`, and `va_end`.
- **[25%]** Min, Max, and Average calculate correctly for positive and negative numbers.
- **[15%]** Edge cases (count = 0, count = 2 identical numbers) handled gracefully without division by zero.

### Expected Output

```
=== Exercise 2: Variadic Stats ===

Test 1 (5, 10, -5, 20, 0, 5):
Min: -5
Max: 20
Avg: 6

Test 2 (2, 42, 42):
Min: 42
Max: 42
Avg: 42

Test 3 (0 args):
Min: 0
Max: 0
Avg: 0
```

### Submission

```
Exercise_2/
├── main.c        (required)
└── Makefile      (required — targets: all, clean)
```

---

## Exercise_3 [build]

### Problem Statement

**Capstone — The Industrial Logger Module**

**Scenario:**
In bare-metal firmware, standard `printf()` is slow, blocks the CPU, and lacks crucial debugging context (like file and line numbers). You need a professional logging module that prints the log level, filename, line number, function name, and formatted message — but only if the log level is enabled at compile time!

**Requirements:**

Create a new `main.c` from scratch. Build the logging module:

**Step 1:** Implement `log_write()`. It receives the pre-evaluated log level, the call-site location (`file`, `line`, `func`), and variadic arguments.
- Format the output string. (For this lab, use `vprintf` to simulate UART output).
- Only print if `level <= LOG_LEVEL_MAX`.

**Step 2:** Define the public macros `LOG_ERROR`, `LOG_WARNING`, `LOG_INFO`, `LOG_DEBUG`.
- Must use `do { ... } while(0)`.
- Must automatically pass `__FILE__`, `__LINE__`, and `__func__` to `log_write()`.
- Must forward variadic arguments correctly using the GCC extension `##__VA_ARGS__`.

**Rules:**
- Follow BARR-C coding style.
- **Code Documentation:** All functions MUST be fully documented using Doxygen-style comments.
- Use `cppcheck` and `clang-tidy` to analyze, and make sure there are no warning or error messages.
- Compile the code using `-DLOG_LEVEL_MAX=LOG_LEVEL_DEBUG` to test all levels.
- Re-compile with `-DLOG_LEVEL_MAX=LOG_LEVEL_WARNING` and verify that INFO and DEBUG logs completely disappear from the output.

### Coding Standards Reference

**MISRA-C 2012 (Safety):**
| Rule | Category | Relevance to This Exercise |
|---|---|---|
| Rule 20.10 | Advisory | The `#` and `##` preprocessor operators should not be used. (Note: In logging modules, `##__VA_ARGS__` is a standard, acceptable violation of this rule in Zephyr/Linux). |
| Rule 21.6 | Required | The Standard Library input/output functions shall not be used (In real firmware, replace `printf` with a UART driver). |

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs (under `C_Books/`)
> and read the full description of each rule above. After writing your code,
> verify your implementation follows these rules.

### Design Hints (optional)

```c
__attribute__((format(printf, 5, 6)))
void log_write(log_level_t level, const char *file, uint32_t line, 
               const char *func, const char *fmt, ...) 
{
    if (level <= LOG_LEVEL_MAX) {
        // Print prefix using level, file, line, func
        va_list ap;
        va_start(ap, fmt);
        vprintf(fmt, ap); // Or vsnprintf into a buffer
        va_end(ap);
        printf("\n");
    }
}

#define LOG_ERROR(fmt, ...) \
    do { \
        if (LOG_LEVEL_ERROR <= LOG_LEVEL_MAX) { \
            log_write(LOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__); \
        } \
    } while(0)
```

### Acceptance Criteria (Scoring)

- **[10%]** Code builds successfully and passes static analysis.
- **[25%]** `log_write` uses `vprintf` or `vsnprintf` correctly.
- **[25%]** Macros capture `__FILE__`, `__LINE__`, and `__func__` automatically.
- **[15%]** Macros correctly forward variadic arguments using `##__VA_ARGS__`.
- **[25%]** Compile-time filtering works correctly depending on the passed `-DLOG_LEVEL_MAX`.

### Expected Output

```
=== Exercise 3: Industrial Logger ===
Compiled with LOG_LEVEL_MAX = 3

[INFO ] main.c:65 (main) | System boot. Build time: 12:00:00
[DEBUG] main.c:68 (main) | Discovered 4 sensors on I2C bus.
[WARN ] main.c:70 (main) | Sensor 2 reading is unstable.
[ERROR] main.c:72 (main) | Watchdog timeout! Rebooting in 500 ms.
[INFO ] main.c:75 (main) | Boot sequence complete.
```

### Submission

```
Exercise_3/
├── main.c        (required)
└── Makefile      (required — targets: all, clean)
```

---

## Exercise_4 [build]

### Problem Statement

**Conditional Compilation — Feature Flags**

**Scenario:**
Because memory is highly constrained in embedded systems, you cannot afford to link drivers that you aren't using. If a customer buys the Wi-Fi version of your IoT device, the Ethernet driver should not just be inactive—it should be entirely excluded from the compile process to save Flash memory. 

**Requirements:**

Create a new `main.c` from scratch. Implement a hardware abstraction layer (HAL) that uses Conditional Compilation to determine which driver to initialize.

1. Implement two simulated drivers: `wifi_driver_init()` and `ethernet_driver_init()`.
2. Use the **Best Practice** `#if defined()` syntax to check for compile-time macros: `CONFIG_WIFI_ENABLED` and `CONFIG_ETHERNET_ENABLED`.
3. If Wi-Fi is enabled, only compile the Wi-Fi init call.
4. If Ethernet is enabled, only compile the Ethernet init call.
5. **Bonus Constraint**: If the user tries to compile with BOTH enabled, throw a compile-time error using `#error "Cannot enable both WiFi and Ethernet at the same time!"`.
6. If neither is enabled, throw a compile-time error `#error "At least one network interface must be enabled!"`.

**Rules:**
- Follow BARR-C coding style.
- Compile multiple times passing different flags via `-D` to test the logic (e.g., `gcc -DCONFIG_WIFI_ENABLED -o main main.c`).
- Use `cppcheck` and `clang-tidy` to analyze, and make sure there are no warning or error messages.

### Coding Standards Reference

**MISRA-C 2012 (Safety):**
| Rule | Category | Relevance to This Exercise |
|---|---|---|
| Rule 20.9 | Required | All identifiers used in the controlling expression of `#if` or `#elif` shall be defined before evaluation. (Use `#if defined(X)` to safely check existence). |

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs (under `C_Books/`)
> and read the full description of each rule above. After writing your code,
> verify your implementation follows these rules.

### Design Hints (optional)

```c
#if defined(CONFIG_WIFI_ENABLED) && defined(CONFIG_ETHERNET_ENABLED)
    #error "Cannot enable both WiFi and Ethernet at the same time!"
#elif defined(CONFIG_WIFI_ENABLED)
    // Call wifi init
#elif defined(CONFIG_ETHERNET_ENABLED)
    // Call ethernet init
#else
    #error "At least one network interface must be enabled!"
#endif
```

### Acceptance Criteria (Scoring)

- **[25%]** Code successfully uses `#if defined()` and `#elif defined()`.
- **[25%]** Wi-Fi driver compiles and runs when `-DCONFIG_WIFI_ENABLED` is passed.
- **[25%]** Ethernet driver compiles and runs when `-DCONFIG_ETHERNET_ENABLED` is passed.
- **[25%]** `#error` directives correctly prevent compilation if both or neither are defined.

### Expected Output

When compiled with `gcc -DCONFIG_WIFI_ENABLED -o main main.c`:
```
=== Exercise 4: Feature Flags ===
[NET] Initializing Wi-Fi Driver...
```

When compiled with `gcc -DCONFIG_WIFI_ENABLED -DCONFIG_ETHERNET_ENABLED -o main main.c`:
```
main.c:15:6: error: #error "Cannot enable both WiFi and Ethernet at the same time!"
```

### Submission

```
Exercise_4/
├── main.c        (required)
└── Makefile      (required — targets: all, clean)
```

---

## Exercise_5 [build]

### Problem Statement

**Token Pasting & Stringification (`##` and `#`)**

**Scenario:**
While MISRA-C restricts the use of the `#` (stringify) and `##` (token pasting) operators (Rule 20.10) because they can make code difficult to read and analyze, they remain heavily used in major embedded operating systems like Zephyr RTOS and Linux. They are commonly used to automatically generate struct names, driver initialization boilerplate, and Kconfig version strings. Therefore, understanding how they work is an essential skill for any embedded engineer.

**Requirements:**

Create a new `main.c` from scratch. Implement the following:

1. **Token Pasting (`##`) — Device Name Generation**:
   Implement a `DEFINE_DEVICE(name, id)` macro that generates a uniquely named `struct device` variable at compile time.
   - `DEFINE_DEVICE(spi, 1)` must expand to: `struct device device_spi_1 = { .dev_id = 1 };`
   - `DEFINE_DEVICE(i2c, 2)` must expand to: `struct device device_i2c_2 = { .dev_id = 2 };`
   - In `main()`, print the `dev_id` of each generated variable.

2. **Stringification (`#`) — Firmware Version String**:
   Implement `STRINGIFY(x)` and `TO_STRING(x)` (two-level macro). The two levels are required so that a defined macro is expanded *before* being stringified.
   - Define three macros: `FW_VERSION_MAJOR 3`, `FW_VERSION_MINOR 0`, `FW_VERSION_PATCH 4`.
   - Build a version string by concatenating the individually stringified parts with `.` separators:
     ```
     TO_STRING(FW_VERSION_MAJOR) "." TO_STRING(FW_VERSION_MINOR) "." TO_STRING(FW_VERSION_PATCH)
     ```
     C automatically merges adjacent string literals, so the result is `"3.0.4"` at compile time.
   - Define a single macro `FW_VERSION_STRING` that uses the pattern above.
   - In `main()`, print: `Firmware version: 3.0.4`

3. **Zephyr Device Tree Mock — Address Lookup via Token Pasting**:
   In real Zephyr RTOS, each hardware peripheral's register address is stored in a `#define` constant with a long, generated name (e.g. `DT_N_NODELABEL_my_i2c_REG_ADDR`). Rather than typing this full name, developers call a short helper macro like `DT_REG_ADDR(DT_N_NODELABEL_my_i2c)`. Under the hood, this macro uses `##` to paste the argument with `_REG_ADDR`, constructing the full constant name at compile time.

   **Important**: `DT_REG_ADDR()` is NOT a function — it does not run at runtime and does not "return" anything. The preprocessor replaces the macro call with the value of the matching `#define` constant before the code is compiled. The result is a compile-time constant, identical to writing `0x40003000U` directly.

   Example of what the preprocessor does, step by step:
   ```
   DT_REG_ADDR(DT_N_NODELABEL_my_i2c)
           ↓  ## pastes the tokens
   DT_N_NODELABEL_my_i2c_REG_ADDR
           ↓  preprocessor substitutes the #define
   0x40003000U
   ```

   Your task:
   - Define the raw address constant: `#define DT_N_NODELABEL_my_i2c_REG_ADDR 0x40003000U`
   - Implement `DT_REG_ADDR(node_id)` using `##` so that `DT_REG_ADDR(DT_N_NODELABEL_my_i2c)` pastes `node_id` with `_REG_ADDR` and resolves to `0x40003000U` at compile time.
   - In `main()`, write: `uint32_t p_i2c_base = DT_REG_ADDR(DT_N_NODELABEL_my_i2c);` and print it with `printf("I2C base address: 0x%08X\n", p_i2c_base);`.
   - Verify by running `gcc -E main.c` and confirming the macro call is replaced by the raw constant in the preprocessed output.
**Rules:**
- Follow BARR-C coding style.
- Use `cppcheck` and `clang-tidy` to analyze, and make sure there are no warning or error messages.

### Coding Standards Reference

**MISRA-C 2012 (Safety):**
| Rule | Category | Relevance to This Exercise |
|---|---|---|
| Rule 20.10 | Advisory | The `#` and `##` preprocessor operators should not be used. (Note: This exercise intentionally explores them because they are standard practice in Linux/Zephyr for driver boilerplate). |

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs (under `C_Books/`)
> and read the full description of each rule above. After writing your code,
> verify your implementation follows these rules.

### Design Hints (optional)

```c
struct device {
    uint32_t dev_id;
};

/* Token pasting — generates a unique variable name at compile time */
#define DEFINE_DEVICE(name, id) \
    struct device device_ ## name ## _ ## id = { .dev_id = (id) }

/* Stringification — two levels ensure macros are expanded before stringifying */
#define STRINGIFY(x)  #x
#define TO_STRING(x)  STRINGIFY(x)

/* Version string — C merges adjacent string literals at compile time */
#define FW_VERSION_MAJOR  3
#define FW_VERSION_MINOR  0
#define FW_VERSION_PATCH  4
/* Hint: FW_VERSION_STRING should produce "3.0.4" */
#define FW_VERSION_STRING  /* your implementation using TO_STRING and "." */

/* DT mock — paste node_id with _REG_ADDR to form the full constant name */
#define DT_REG_ADDR(node_id)  /* your implementation */
```

### Acceptance Criteria (Scoring)

- **[20%]** Code successfully uses `##` to dynamically name the device structs (e.g., `device_spi_1`).
- **[20%]** `FW_VERSION_STRING` correctly produces `"3.0.4"` by concatenating three individually stringified macros.
- **[20%]** Code successfully uses `##` to implement `DT_REG_ADDR`, and `main()` stores the result in a `uint32_t` variable and prints it.
- **[20%]** The `main()` function output matches the Expected Output exactly.
- **[20%]** Code passes static analysis.

### Expected Output

```
=== Exercise 5: Token Pasting & Stringification ===
Firmware version: 3.0.4
Initialized SPI device with ID: 1
Initialized I2C device with ID: 2
I2C reg addr: 0x40003000
```

### Submission

```
Exercise_5/
├── main.c        (required)
├── Makefile      (required — targets: all, clean)
└── *.h           (if any)
```

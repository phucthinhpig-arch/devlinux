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

# Assignment — Session 06: Embedded Design Patterns
**Deadline: 2026-07-18 23:59:00**

---

## Overview

This homework covers the three major topics from Lecture 6:

| Exercise | Topic | Difficulty |
|:---|:---|:---:|
| Exercise_1 | Callback Pattern — Timer Registration System | ★☆☆ |
| Exercise_2 | Singleton Pattern — Single-Instance Driver with Accessor | ★★☆ |
| Exercise_3 | Command Dispatcher — String-Driven Jump Table | ★★☆ |
| Exercise_4 | Function Pointer FSM — Traffic Light Controller (Refactor) | ★★☆ |
| Exercise_5 | Scalable FSM — WiFi Connection State Machine (Full Design) | ★★★ |

> **Prerequisite**: Review `L6_labs/demo_clean_code.c`, `L6_labs/demo_singleton_accessor.c`, `L6_labs/demo_command_dispatcher.c`, and `L6_labs/demo_fsm_function_pointers.c` before starting.

---

## Exercise_1 [build]

### Problem Statement

**Callback Pattern — Timer Registration System**

**Scenario:**
In embedded firmware, hardware peripherals (timers, UARTs, ADCs) generate asynchronous events. Instead of continuously polling in the main loop (which wastes CPU cycles), the application registers a **callback function** — a function pointer that the driver will call when the event fires.

Your task is to implement a simple software timer system that supports this callback registration pattern.

**Requirements:**

Write a C program with the following interface:

```c
typedef void (*timer_callback_t)(void);

void Timer_Register(uint32_t expire_at_tick, timer_callback_t callback);
void Timer_Tick(void);
void Timer_Reset(void);
bool Timer_IsRunning(void);
```

The behavior must be:
1. `Timer_Register()` stores the expiry tick count and the callback function pointer in a `static` internal struct (Singleton pattern). Calling `Timer_Register()` while a timer is already running must print a warning and not overwrite the existing timer.
2. `Timer_Tick()` increments an internal tick counter. When `current_tick == expire_at_tick`, it invokes the registered callback (with a `NULL` guard), then stops the timer.
3. `Timer_Reset()` cancels any active timer.
4. In `main()`:
   - Register `My_Alarm_Function` (prints an alarm message) to fire at tick 5.
   - Simulate 10 ticks via a loop.
   - Demonstrate that registering a second timer while the first is active is rejected.
   - After reset, register and fire a second alarm at tick 3.

**Rules:**
- The internal timer state **must** be a `static` struct inside the `.c` file (not a global variable). No other module may access it directly.
- All function pointer calls must be guarded against `NULL`.
- Follow BARR-C coding style (fixed-width integers, mandatory braces, `p_` pointer prefix).
- **Code Documentation:** All functions and data structures MUST be fully documented using Doxygen-style comments.
- Use `cppcheck` and `clang-tidy` to analyze, and make sure there are no warning or error messages.
- Compile with strict flags: `-Wall -Wextra -pedantic -Werror -std=c99`.

### Coding Standards Reference

**MISRA-C 2012 (Safety):**
| Rule | Category | Relevance to This Exercise |
|---|---|---|
| Directive 4.11 | Required | Validate callback pointer before invocation — guard `on_expire != NULL`. |
| Directive 4.12 | Required | No dynamic memory allocation — use `static` struct for timer state. |
| Rule 9.1 | Mandatory | Initialize all local variables and the `static` struct fields before use. |
| Rule 15.5 | Advisory | Functions should have a single exit point — structure `Timer_Tick()` accordingly. |
| Rule 15.6 | Required | Mandatory braces on all `if`/`else`/`for`/`while` control structures. |

**CERT-C 2016 (Security):**
| Rule | Relevance to This Exercise |
|---|---|
| EXP34-C | Never dereference a NULL function pointer — guard `on_expire` before calling it. |
| EXP33-C | Do not read uninitialized memory — initialize the `static` struct at declaration. |

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs (under `C_Books/`)
> and read the full description of each rule above. After writing your code,
> verify your implementation follows these rules.

### Design Hints (optional)

```c
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

typedef void (*timer_callback_t)(void);

/* Internal timer state — static, invisible to other modules */
static struct {
    uint32_t         expire_at_tick; /**< Tick count at which the callback fires. */
    uint32_t         current_tick;   /**< Current elapsed tick count.             */
    timer_callback_t on_expire;      /**< Callback to invoke on expiry.           */
    bool             is_running;     /**< True if a timer is currently active.    */
} s_timer = { 0U, 0U, NULL, false };

/**
 * @brief Register a callback to fire after a given number of ticks.
 * @param[in] expire_at_tick  Tick number at which the callback fires.
 * @param[in] callback        Function pointer to invoke. Must not be NULL.
 */
void Timer_Register(uint32_t expire_at_tick, timer_callback_t callback);

/**
 * @brief Advance the timer by one tick. Fires callback if expired.
 */
void Timer_Tick(void);

/**
 * @brief Cancel any active timer.
 */
void Timer_Reset(void);

/**
 * @brief Query whether a timer is currently active.
 * @return true if a timer is running, false otherwise.
 */
bool Timer_IsRunning(void);
```

### Suggested Approach (optional)

```
1. Implement Timer_Register(): check is_running guard, store fields, set is_running = true.
2. Implement Timer_Tick(): increment current_tick, check expiry, NULL-guard callback call, stop timer.
3. Implement Timer_Reset(): reset all fields to zero/false.
4. In main(): register alarm at tick 5, loop 10 ticks, attempt second registration during run,
   call Timer_Reset(), register new alarm at tick 3, loop 5 more ticks.
```

### Acceptance Criteria (Scoring)

- **[15%]** Code builds successfully without warnings or errors using strict flags.
- **[15%]** Code passes `cppcheck` and `clang-tidy` with no warnings.
- **[10%]** All functions and the internal struct are fully documented with Doxygen comments.
- **[20%]** Timer fires the callback exactly once at the correct tick, then stops.
- **[20%]** Double-registration guard works: registering while active prints a warning and is rejected.
- **[20%]** Reset-then-register-again works: after `Timer_Reset()`, a new timer can be registered and fires correctly.

### Expected Output

```
--- Test 1: Alarm at tick 5, run for 10 ticks ---
Tick 1...
Tick 2...
Tick 3...
Tick 4...
Tick 5...
[ALARM] Timer fired at tick 5!
Tick 6...
[WARN] Timer already running! Ignoring new registration.
Tick 7...
Tick 8...
Tick 9...
Tick 10...

--- Test 2: Reset, then new alarm at tick 3 ---
[TIMER] Reset.
Tick 1...
Tick 2...
Tick 3...
[ALARM] Second alarm fired at tick 3!
```

Exit code: `0` on success.

### Submission

```
Exercise_1/
├── main.c        (required — timer implementation + test in main)
├── Makefile      (required — targets: all, clean)
└── *.h           (if any)
```

---

## Exercise_2 [build]

### Problem Statement

**Singleton Pattern — Single-Instance ADC Driver with Getter Functions**

**Scenario:**
In embedded firmware, hardware peripherals such as ADCs exist exactly once on the chip. If two modules accidentally initialize the same ADC with different configurations, the second `Init` call silently corrupts the first module's settings, leading to subtle, hard-to-debug runtime errors.

The Singleton pattern solves this: the internal driver state is a `static` struct hidden inside the `.c` file, completely invisible to all other modules. Access is controlled exclusively through a public API.

**Requirements:**

Implement a single-file ADC driver that demonstrates the Singleton pattern with getter functions:

```c
bool     ADC_Init(uint32_t sample_rate_hz);
void     ADC_DeInit(void);
void     ADC_SetChannel(uint32_t channel);
uint32_t ADC_Read(void);
bool     ADC_IsInitialized(void);
uint32_t ADC_GetChannel(void);
uint32_t ADC_GetSampleRate(void);
```

Behavior:
1. `ADC_Init()`: Initializes the ADC. If called a second time before `ADC_DeInit()`, print an error and return `false` (single-instance guard).
2. `ADC_SetChannel()` / `ADC_Read()`: Guard against use before initialization.
3. **Getter functions** (`ADC_IsInitialized`, `ADC_GetChannel`, `ADC_GetSampleRate`): Allow other modules to read specific fields without accessing the struct directly.
4. Demonstrate in `main()` that `s_adc_ctx.active_channel` cannot be accessed directly (include a commented-out line with the compile error explanation).

**Rules:**
- The internal ADC state **must** be a `static` struct inside the `.c` file (not exposed in any header).
- Do not use `malloc` or any dynamic memory allocation.
- All pointer parameters must be NULL-guarded.
- Follow BARR-C coding style (fixed-width integers, mandatory braces, `p_` pointer prefix for pointers in function parameters).
- **Code Documentation:** All functions and data structures MUST be fully documented using Doxygen-style comments.
- Use `cppcheck` and `clang-tidy` to analyze, and make sure there are no warning or error messages.
- Compile with strict flags: `-Wall -Wextra -pedantic -Werror -std=c99`.

### Coding Standards Reference

**MISRA-C 2012 (Safety):**
| Rule | Category | Relevance to This Exercise |
|---|---|---|
| Directive 4.12 | Required | No dynamic memory allocation — internal state uses a `static` struct only. |
| Rule 8.9 | Advisory | Objects that are only accessed from within a single translation unit should be `static` at file scope → applies to `s_adc_ctx`. |
| Rule 9.1 | Mandatory | Initialize `s_adc_ctx` to `{ false, 0U, 0U }` at declaration. |
| Rule 15.5 | Advisory | Single exit point in functions with multiple error guards (e.g., `ADC_Init`). |
| Rule 15.6 | Required | Mandatory braces on all control structures. |

**CERT-C 2016 (Security):**
| Rule | Relevance to This Exercise |
|---|---|
| EXP34-C | Guard all pointer parameters against NULL before dereferencing. |
| EXP33-C | Ensure the static struct is initialized before any getter is called. |

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs (under `C_Books/`)
> and read the full description of each rule above. After writing your code,
> verify your implementation follows these rules.

### Design Hints (optional)

```c
/* Private struct — hidden inside the .c file. NOT declared in any .h file. */
static struct {
    bool     is_initialized;   /**< Guards against double-initialization. */
    uint32_t active_channel;   /**< Currently selected ADC channel.       */
    uint32_t sample_rate_hz;   /**< Configured sampling rate in Hz.       */
} s_adc_ctx = { false, 0U, 0U };

/* Getter functions — the ONLY way for other modules to read specific fields */
bool     ADC_IsInitialized(void) { return s_adc_ctx.is_initialized; }
uint32_t ADC_GetChannel(void)    { return s_adc_ctx.active_channel;  }
uint32_t ADC_GetSampleRate(void) { return s_adc_ctx.sample_rate_hz;  }
```

### Suggested Approach (optional)

```
1. Implement the full ADC API with the static struct guard.
2. Implement getter functions for each readable field.
3. In main(): init, set channel 2, read, print via getters.
4. Attempt double-init: verify the guard fires.
5. DeInit, re-init, verify clean state.
6. Add a commented-out line: s_adc_ctx.active_channel = 99;  // COMPILE ERROR
```

### Acceptance Criteria (Scoring)

- **[15%]** Code builds successfully without warnings or errors using strict flags.
- **[15%]** Code passes `cppcheck` and `clang-tidy` with no warnings.
- **[10%]** All functions and the internal struct are documented with Doxygen comments.
- **[30%]** Double-init guard fires on the second `ADC_Init()` call and returns `false`.
- **[30%]** Getter functions return correct field values; a commented-out direct access line (`s_adc_ctx.active_channel`) is included with a compile error explanation.

### Expected Output

```
[ADC] Initialized at 44100 Hz on channel 0.
[ADC] Error: Already initialized! Call ADC_DeInit() first.
Channel:     0
Sample rate: 44100 Hz
Init status: YES
[ADC] Channel set to 2.
[ADC] Read ch2 -> 300 mV
Result: 300 mV
[ADC] De-initialized.
Is initialized? NO
```

Exit code: `0` on success.

### Submission

```
Exercise_2/
├── main.c        (required — ADC driver + getter functions + test in main)
├── Makefile      (required — targets: all, clean)
└── *.h           (if any)
```

---

## Exercise_3 [build]

### Problem Statement

**Command Dispatcher — String-Driven Jump Table**

**Scenario:**
Embedded systems often receive commands over UART, USB-CDC, or Ethernet as ASCII strings: `"LED_ON"`, `"MOTOR_START"`, `"RESET"`. The naive approach is a chain of `if-strcmp-else` blocks — fragile, slow to extend, and impossible to review.

The **Command Dispatcher** pattern replaces this with a `const` table of `{string, function_pointer}` pairs. Adding a new command means adding one struct entry to the table — not editing the dispatch logic.

**Requirements:**

Implement a command dispatcher system supporting at least 5 commands:

| Command String | Action |
|:---|:---|
| `"LED_ON"` | Print `"[CMD] LED turned ON."` |
| `"LED_OFF"` | Print `"[CMD] LED turned OFF."` |
| `"MOTOR_START"` | Print `"[CMD] Motor started at 1500 RPM."` |
| `"MOTOR_STOP"` | Print `"[CMD] Motor stopped."` |
| `"STATUS"` | Print `"[CMD] System status: OK."` |

The dispatcher interface:
```c
typedef void (*cmd_action_t)(void);

typedef struct {
    const char   *p_command_str; /**< Null-terminated command string. */
    cmd_action_t  action;        /**< Function pointer to invoke.     */
} command_entry_t;

void Dispatch_Command(const char *p_received_cmd);
```

Behavior:
1. `Dispatch_Command()` iterates the `CMD_TABLE` and invokes the matching `action`. Unknown commands print `"[CMD] Unknown command: <name>"`.
2. The `CMD_TABLE` must be `const` (lives in Flash, not RAM).
3. Array size must use `sizeof(CMD_TABLE) / sizeof(CMD_TABLE[0])` (the `ARRAY_SIZE` macro — identical to how it is used in the Linux kernel and Zephyr RTOS).
4. In `main()`, call `Dispatch_Command()` with all 5 valid commands plus one unknown command.

**Rules:**
- The `CMD_TABLE` must be `static const`.
- All string comparisons must use `strcmp()` with a `NULL` guard on the input pointer.
- No dynamic memory allocation.
- Follow BARR-C coding style (fixed-width integers, mandatory braces, `p_` prefix for pointer parameters).
- **Code Documentation:** All functions and data structures MUST be fully documented using Doxygen-style comments.
- Use `cppcheck` and `clang-tidy` to analyze, and make sure there are no warning or error messages.
- Compile with strict flags: `-Wall -Wextra -pedantic -Werror -std=c99`.

### Coding Standards Reference

**MISRA-C 2012 (Safety):**
| Rule | Category | Relevance to This Exercise |
|---|---|---|
| Directive 4.11 | Required | Validate `p_received_cmd` for NULL before passing to `strcmp()`. |
| Directive 4.12 | Required | No dynamic memory allocation — `CMD_TABLE` must be `static const`. |
| Rule 14.2 | Required | Well-formed `for` loop — iterate table using an index with a compile-time bound. |
| Rule 15.6 | Required | Mandatory braces on all control structures inside `Dispatch_Command()`. |
| Rule 18.1 | Required | Pointer arithmetic within array bounds — stay within `CMD_TABLE_SIZE`. |

**CERT-C 2016 (Security):**
| Rule | Relevance to This Exercise |
|---|---|
| EXP34-C | Guard `p_received_cmd` against NULL before calling `strcmp()`. |
| ARR30-C | Never access `CMD_TABLE[i]` with `i >= CMD_TABLE_SIZE`. |
| STR31-C | Command strings are string literals in `.rodata` — do not attempt to modify them. |

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs (under `C_Books/`)
> and read the full description of each rule above. After writing your code,
> verify your implementation follows these rules.

### Design Hints (optional)

```c
#include <stdint.h>
#include <string.h>
#include <stdio.h>

typedef void (*cmd_action_t)(void);

typedef struct {
    const char   *p_command_str; /**< Command keyword string. */
    cmd_action_t  action;        /**< Handler function.       */
} command_entry_t;

#define ARRAY_SIZE(arr)  (sizeof(arr) / sizeof((arr)[0]))

static const command_entry_t CMD_TABLE[] = {
    { "LED_ON",      Cmd_LED_On      },
    { "LED_OFF",     Cmd_LED_Off     },
    { "MOTOR_START", Cmd_Motor_Start },
    { "MOTOR_STOP",  Cmd_Motor_Stop  },
    { "STATUS",      Cmd_Status      },
};

/**
 * @brief Dispatch a received ASCII command string to its handler.
 * @param[in] p_received_cmd  Null-terminated command string. Must not be NULL.
 */
void Dispatch_Command(const char *p_received_cmd);
```

### Suggested Approach (optional)

```
1. Define the 5 command action functions (Cmd_LED_On, etc.).
2. Build CMD_TABLE with designated initializers or ordered struct literals.
3. Implement Dispatch_Command(): NULL-guard input, loop table, strcmp match, call action.
4. Use ARRAY_SIZE() macro for the loop bound — never hardcode the table size.
5. In main(): call Dispatch_Command() with all 5 valid commands + 1 unknown.
```

### Acceptance Criteria (Scoring)

- **[15%]** Code builds without warnings or errors using strict flags.
- **[15%]** Code passes `cppcheck` and `clang-tidy` with no warnings.
- **[10%]** All functions and the struct are documented with Doxygen comments.
- **[20%]** All 5 valid commands dispatch to the correct handler.
- **[20%]** Unknown commands print the correct error message.
- **[20%]** `CMD_TABLE` is `static const`; `ARRAY_SIZE` macro is used for the loop bound.

### Expected Output

```
[CMD] LED turned ON.
[CMD] LED turned OFF.
[CMD] Motor started at 1500 RPM.
[CMD] Motor stopped.
[CMD] System status: OK.
[CMD] Unknown command: REBOOT
```

Exit code: `0` on success.

### Submission

```
Exercise_3/
├── main.c        (required — command functions + dispatcher + test in main)
├── Makefile      (required — targets: all, clean)
└── *.h           (if any)
```

---

## Exercise_4 [build]

### Problem Statement

**Function Pointer FSM — Traffic Light Controller (Refactor)**

**Scenario:**
A junior engineer wrote a traffic light controller using a `switch-case` inside the main loop. As the project grew, the single `switch` function exceeded 150 lines and became impossible to review. Your task is to refactor it into a clean Function Pointer FSM using the 4-Rule methodology from Lecture 6.

**Provided switch-case code (starting point):**

```c
typedef enum { RED = 0, GREEN = 1, YELLOW = 2, NUM_STATES } traffic_state_t;

void run_traffic_light_bad(traffic_state_t *p_state, uint32_t tick) {
    switch (*p_state) {
        case RED:
            printf("[RED]    Tick %u — Stop! Holding for 3 ticks.\n", tick);
            if (tick % 3U == 0U) { *p_state = GREEN; }
            break;
        case GREEN:
            printf("[GREEN]  Tick %u — Go!  Holding for 3 ticks.\n", tick);
            if (tick % 3U == 0U) { *p_state = YELLOW; }
            break;
        case YELLOW:
            printf("[YELLOW] Tick %u — Slow down!\n", tick);
            *p_state = RED;
            break;
        default: break;
    }
}
```

**Requirements:**

Refactor the above into a Function Pointer FSM following all 4 rules:
1. Define `traffic_state_t` enum with `NUM_STATES` as the last entry (sentinel).
2. Create individual state functions `State_Red()`, `State_Green()`, `State_Yellow()` — each with the signature `void (*)(uint32_t tick, traffic_state_t *p_next_state)`.
3. Create a `static const` function pointer array `TrafficFSM[NUM_STATES]`.
4. Implement `RunTrafficFSM(uint32_t tick, traffic_state_t *p_state)` with bounds check and NULL guard.
5. In `main()`, replace the original `switch-case` call with `RunTrafficFSM()`. Simulate 10 ticks. The output must be **identical** to what the original `switch-case` produces.

**Rules:**
- `RunTrafficFSM()` must contain a bounds check (`*p_state >= NUM_STATES`) before dispatching.
- The function pointer array must be `static const` (to place it in Flash / `.rodata`).
- Do not use any global variables for state — pass state via a pointer parameter.
- Follow BARR-C coding style (fixed-width integers, mandatory braces, `p_` prefix for pointers).
- **Code Documentation:** All functions and data structures MUST be fully documented using Doxygen-style comments.
- Use `cppcheck` and `clang-tidy` to analyze, and make sure there are no warning or error messages.
- Compile with strict flags: `-Wall -Wextra -pedantic -Werror -std=c99`.

### Coding Standards Reference

**MISRA-C 2012 (Safety):**
| Rule | Category | Relevance to This Exercise |
|---|---|---|
| Directive 4.11 | Required | Validate function pointer is not NULL before calling via table. |
| Rule 9.1 | Mandatory | Initialize `traffic_state_t state = RED;` before the tick loop. |
| Rule 15.6 | Required | Mandatory braces on `if (*p_state >= NUM_STATES)` bounds check. |
| Rule 18.1 | Required | Never access `TrafficFSM[i]` with `i >= NUM_STATES`. |
| Rule 14.2 | Required | Well-formed `for` loop for the 10-tick simulation. |

**CERT-C 2016 (Security):**
| Rule | Relevance to This Exercise |
|---|---|
| EXP34-C | Guard the function pointer `TrafficFSM[*p_state]` against NULL before calling. |
| ARR30-C | Bounds-check `*p_state < NUM_STATES` before indexing `TrafficFSM[]`. |

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs (under `C_Books/`)
> and read the full description of each rule above. After writing your code,
> verify your implementation follows these rules.

### Design Hints (optional)

```c
typedef enum { RED = 0, GREEN = 1, YELLOW = 2, NUM_STATES } traffic_state_t;

/* Uniform state function signature */
typedef void (*traffic_handler_t)(uint32_t tick, traffic_state_t *p_next_state);

/* One function per state */
static void State_Red   (uint32_t tick, traffic_state_t *p_next_state);
static void State_Green (uint32_t tick, traffic_state_t *p_next_state);
static void State_Yellow(uint32_t tick, traffic_state_t *p_next_state);

/* The FSM table — const = lives in Flash */
static const traffic_handler_t TrafficFSM[NUM_STATES] = {
    [RED]    = State_Red,
    [GREEN]  = State_Green,
    [YELLOW] = State_Yellow,
};

/**
 * @brief Run one tick of the traffic light FSM.
 * @param[in]     tick      Current tick number.
 * @param[in,out] p_state   Pointer to current state; updated by the state function.
 */
void RunTrafficFSM(uint32_t tick, traffic_state_t *p_state);
```

### Suggested Approach (optional)

```
1. Implement State_Red(), State_Green(), State_Yellow() with identical logic to the original switch-case.
2. Build TrafficFSM[] with designated initializers.
3. Implement RunTrafficFSM(): bounds check → NULL check → dispatch via table.
4. In main(): initialize state = RED, loop tick 1..10, call RunTrafficFSM(tick, &state).
5. Compare output with the original switch-case output — they must be identical.
```

### Acceptance Criteria (Scoring)

- **[15%]** Code builds without warnings or errors using strict flags.
- **[15%]** Code passes `cppcheck` and `clang-tidy` with no warnings.
- **[10%]** All functions are documented with Doxygen comments.
- **[25%]** The output of the Function Pointer FSM is byte-for-byte identical to the original `switch-case` version.
- **[20%]** `RunTrafficFSM()` contains both bounds check (`>= NUM_STATES`) and NULL guard before dispatch.
- **[15%]** No global state variables — state is passed via pointer parameter only.

### Expected Output

```
[RED]    Tick 1 — Stop! Holding for 3 ticks.
[RED]    Tick 2 — Stop! Holding for 3 ticks.
[RED]    Tick 3 — Stop! Holding for 3 ticks.
[GREEN]  Tick 4 — Go!  Holding for 3 ticks.
[GREEN]  Tick 5 — Go!  Holding for 3 ticks.
[GREEN]  Tick 6 — Go!  Holding for 3 ticks.
[YELLOW] Tick 7 — Slow down!
[RED]    Tick 8 — Stop! Holding for 3 ticks.
[RED]    Tick 9 — Stop! Holding for 3 ticks.
[RED]    Tick 10 — Stop! Holding for 3 ticks.
```

Exit code: `0` on success.

### Submission

```
Exercise_4/
├── main.c        (required — both original switch-case AND refactored FSM, with comparison)
├── Makefile      (required — targets: all, clean)
└── *.h           (if any)
```

---

## Exercise_5 [build]

### Problem Statement

**Scalable FSM — WiFi Connection State Machine (Full Design from Scratch)**

**Scenario:**
This is the capstone exercise for Lecture 6. You will design and implement a complete, production-style WiFi connection state machine from scratch using the full `RunStateMachine()` methodology taught in class.

This exercise tests your ability to:
- Design a state diagram before writing any code
- Apply all 4 FSM rules correctly
- Handle edge cases (retry logic, state corruption guard)
- Write a self-documenting, standards-compliant implementation

**State Diagram (design this before coding):**

```
  WIFI_INIT ──(always)──▶ WIFI_CONNECTING
                               │
              input==1         │         input==0 (retry < 3)
           ┌──────────────────┘ └──────────────────────┐
           ▼                                             ▼
     WIFI_CONNECTED                            WIFI_CONNECTING (retry)
           │                                             │
     input==0                               retry >= 3  │
           ▼                                             ▼
    WIFI_CONNECTING                            WIFI_ERROR
                                                    │
                                           (any input)
                                                    ▼
                                             WIFI_INIT
```

**States and Transition Rules:**

| State | Input | Next State | Action |
|:---|:---|:---|:---|
| `WIFI_INIT` | any | `WIFI_CONNECTING` | Reset retry counter, print init message |
| `WIFI_CONNECTING` | `1` | `WIFI_CONNECTED` | Reset retry counter, print connected |
| `WIFI_CONNECTING` | `0` | `WIFI_CONNECTING` (retry++) | Print retry attempt number |
| `WIFI_CONNECTING` | `0` (retry >= 3) | `WIFI_ERROR` | Print error, reset retry |
| `WIFI_CONNECTED` | `1` | `WIFI_CONNECTED` | Stay connected, print online status |
| `WIFI_CONNECTED` | `0` | `WIFI_CONNECTING` | Link dropped, print reconnecting |
| `WIFI_ERROR` | any | `WIFI_INIT` | Print error recovery |

**Requirements:**

1. Define `WIFI_STATE` enum with `WIFI_MAX_STATES` as the last entry.
2. Define a uniform state function signature:
   `typedef uint32_t (*wifi_handler_t)(uint8_t input, WIFI_STATE *p_next_state);`
3. Implement all 4 state functions. The retry counter **must** be a `static` variable inside `wifi_state_connecting()` — reset it when entering the state fresh (from INIT or CONNECTED), increment on failed retry.
4. Build `const WifiFSM[WIFI_MAX_STATES]` function pointer array.
5. Implement `RunStateMachine(uint8_t input, WIFI_STATE *p_state)` with:
   - Bounds check: `*p_state >= WIFI_MAX_STATES` → error recovery
   - NULL guard on function pointer before call
6. In `main()`, test with the input sequence `{0, 0, 1, 0, 0, 0, 0, 1}` and print each state transition. Verify that:
   - After 3 consecutive `0` inputs in CONNECTING, the FSM enters ERROR.
   - ERROR automatically restarts to INIT on the next tick.
   - After a link-drop (`0` from CONNECTED), the FSM re-enters CONNECTING correctly.

**Rules:**
- No global variables for state or retry counter — state via pointer, retry as `static` local.
- `WifiFSM[]` must be `static const` (Flash placement).
- Bounds check in `RunStateMachine()` is mandatory.
- Follow BARR-C coding style (fixed-width integers, mandatory braces, `p_` pointer prefix).
- **Code Documentation:** All functions and data structures MUST be fully documented using Doxygen-style comments.
- Use `cppcheck` and `clang-tidy` to analyze, and make sure there are no warning or error messages.
- Compile with strict flags: `-Wall -Wextra -pedantic -Werror -std=c99`.

### Coding Standards Reference

**MISRA-C 2012 (Safety):**
| Rule | Category | Relevance to This Exercise |
|---|---|---|
| Directive 4.11 | Required | Validate function pointer `WifiFSM[*p_state]` is not NULL before calling. |
| Directive 4.12 | Required | No dynamic memory allocation — all state is on the stack or `static`. |
| Rule 9.1 | Mandatory | Initialize `WIFI_STATE state = WIFI_INIT;` before the input loop. |
| Rule 14.2 | Required | Well-formed `for` loop for iterating the input sequence. |
| Rule 15.5 | Advisory | Single exit point in `RunStateMachine()`. |
| Rule 15.6 | Required | Mandatory braces on bounds check and all control structures. |
| Rule 18.1 | Required | Never access `WifiFSM[i]` with `i >= WIFI_MAX_STATES`. |

**CERT-C 2016 (Security):**
| Rule | Relevance to This Exercise |
|---|---|
| EXP34-C | Guard `WifiFSM[*p_state]` against NULL before invocation. |
| EXP33-C | Initialize the `static` retry counter correctly on every fresh entry to CONNECTING. |
| ARR30-C | Bounds-check `*p_state < WIFI_MAX_STATES` before indexing the FSM table. |

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs (under `C_Books/`)
> and read the full description of each rule above. After writing your code,
> verify your implementation follows these rules.

### Design Hints (optional)

```c
#include <stdint.h>
#include <stdio.h>

typedef enum {
    WIFI_INIT       = 0,
    WIFI_CONNECTING = 1,
    WIFI_CONNECTED  = 2,
    WIFI_ERROR      = 3,
    WIFI_MAX_STATES       /**< Sentinel — do NOT assign explicitly. */
} WIFI_STATE;

typedef uint32_t (*wifi_handler_t)(uint8_t input, WIFI_STATE *p_next_state);

/* Declare all state functions */
static uint32_t wifi_state_init      (uint8_t input, WIFI_STATE *p_next_state);
static uint32_t wifi_state_connecting(uint8_t input, WIFI_STATE *p_next_state);
static uint32_t wifi_state_connected (uint8_t input, WIFI_STATE *p_next_state);
static uint32_t wifi_state_error     (uint8_t input, WIFI_STATE *p_next_state);

/* FSM table — const = in Flash, not RAM */
static const wifi_handler_t WifiFSM[WIFI_MAX_STATES] = {
    [WIFI_INIT]       = wifi_state_init,
    [WIFI_CONNECTING] = wifi_state_connecting,
    [WIFI_CONNECTED]  = wifi_state_connected,
    [WIFI_ERROR]      = wifi_state_error,
};

/**
 * @brief Run one step of the WiFi FSM.
 * @param[in]     input     Hardware/event input (1 = link up, 0 = link down/fail).
 * @param[in,out] p_state   Pointer to the current state; updated on transition.
 * @return 0 on success, 0xFF on invalid state.
 */
uint32_t RunStateMachine(uint8_t input, WIFI_STATE *p_state);
```

**Retry counter hint** (the tricky part):
```c
static uint32_t wifi_state_connecting(uint8_t input, WIFI_STATE *p_next_state) {
    static uint32_t s_retry_count = 0U;  /* static: persists between calls */

    if (input == 1U) {
        s_retry_count = 0U;              /* Reset on success */
        *p_next_state = WIFI_CONNECTED;
    } else {
        s_retry_count++;
        if (s_retry_count >= 3U) {
            s_retry_count = 0U;          /* Reset on giving up */
            *p_next_state = WIFI_ERROR;
        } else {
            *p_next_state = WIFI_CONNECTING;
        }
    }
    return 0U;
}
```

### Suggested Approach (optional)

```
1. Draw the state diagram on paper first. Label every arrow with input and next state.
2. Implement each state function following the transition rules table.
3. Build WifiFSM[] with designated initializers.
4. Implement RunStateMachine() with bounds check, NULL guard, and dispatch.
5. In main(): run input sequence {0, 0, 1, 0, 0, 0, 0, 1}, print state name + action per step.
6. Trace the retry counter carefully — it must reset when INIT transitions to CONNECTING.
```

### Acceptance Criteria (Scoring)

- **[10%]** Code builds without warnings or errors using strict flags.
- **[10%]** Code passes `cppcheck` and `clang-tidy` with no warnings.
- **[10%]** All functions documented with Doxygen comments.
- **[15%]** `RunStateMachine()` has correct bounds check and NULL guard before dispatch.
- **[25%]** All state transitions are correct for the given input sequence.
- **[20%]** Retry logic works correctly: 3 consecutive failures → ERROR; entering fresh from INIT resets the counter.
- **[10%]** `WifiFSM[]` is `static const`; state is passed via pointer (no global state variables).

### Expected Output

```
[Step 0] State: WIFI_INIT       | input=0
[INIT] Initializing... -> CONNECTING

[Step 1] State: WIFI_CONNECTING | input=0
[CONNECTING] Attempt 1 failed. Retrying...

[Step 2] State: WIFI_CONNECTING | input=0
[CONNECTING] Attempt 2 failed. Retrying...

[Step 3] State: WIFI_CONNECTING | input=1
[CONNECTING] Connected! Retry count reset.

[Step 4] State: WIFI_CONNECTED  | input=0
[CONNECTED] Link dropped. Reconnecting...

[Step 5] State: WIFI_CONNECTING | input=0
[CONNECTING] Attempt 1 failed. Retrying...

[Step 6] State: WIFI_CONNECTING | input=0
[CONNECTING] Attempt 2 failed. Retrying...

[Step 7] State: WIFI_CONNECTING | input=0
[CONNECTING] Attempt 3 failed. -> ERROR

[Step 8] (auto) State: WIFI_ERROR | input=0
[ERROR] Recovery. Restarting -> INIT
```

Exit code: `0` on success.

### Submission

```
Exercise_5/
├── main.c        (required — full FSM implementation + test in main)
├── Makefile      (required — targets: all, clean)
└── *.h           (if any)
```

---

## General Submission Structure

```
L6_homework/
├── Exercise_1/
│   ├── main.c
│   └── Makefile
├── Exercise_2/
│   ├── main.c
│   └── Makefile
├── Exercise_3/
│   ├── main.c
│   └── Makefile
├── Exercise_4/
│   ├── main.c
│   └── Makefile
└── Exercise_5/
    ├── main.c
    └── Makefile
```

> **Reminder:** Each Makefile must support `make all` and `make clean`. Build with:
> ```
> gcc -Wall -Wextra -pedantic -Werror -std=c99 -o output main.c
> ```

# Assignment — Session: 08
**Deadline: 2026-09-06 23:59:00**

---

## Exercise_1 — Two Timer APIs and a Watchdog That Bites [review-only]

### Problem Statement

`vTaskDelay()` has carried you through five sessions, and it is the wrong tool the moment you need something to happen *on time* rather than *eventually*. ESP-IDF gives you two timer APIs at different levels, and a watchdog whose job is to reboot the board when your code stops behaving. This exercise makes you use all three.

Build one program with three parts:

1. **`esp_timer` — a periodic software timer.** Create a periodic timer firing every 1000 ms whose callback increments a counter and logs seconds since boot.
2. **`gptimer` — a hardware general-purpose timer.** Configure a hardware timer at 1 MHz resolution with an alarm every 250 ms. Its callback runs in interrupt context, so it may only record something and return — the logging happens elsewhere, exactly as in Session 04.
3. **The Task Watchdog — trip it on purpose, then fix it.** Subscribe your own task to the Task Watchdog Timer, then call a function that deliberately busy-waits for far longer than the watchdog timeout. Observe the board panic and reboot. Then fix it two different ways.

Requirements:
- Use `esp_timer.h` for part 1 and `driver/gptimer.h` for part 2. Both must run at the same time in the same program.
- The `gptimer` callback runs in an ISR. It must only touch a counter or a flag and return — no `ESP_LOGI`, no delays, no allocation. Log its progress from a normal task.
- Any variable shared between the `gptimer` callback and a task must be `volatile`.
- For part 3, use `esp_task_wdt.h`: subscribe the current task with `esp_task_wdt_add()`, then starve it. Your stall function must be a genuine busy-wait — a loop the scheduler cannot interrupt — not `vTaskDelay()`.
- Ship the **fixed** version as your `main.c`, with the stalling version present but commented out and clearly labelled, so a reviewer can see both. Implement **both** fixes:
  - the fix where the long-running loop keeps the watchdog fed, and
  - the fix where the task stops hogging the CPU in the first place.
- Every period, timeout and resolution must be a named constant.
- Answer the questions in `timer_notes.md`.

### Hardware

ESP32-S3 DevKitC-1 only. No new parts. Leave the display module wired as it is from Sessions 06 and 07 — this exercise does not touch it.

### Design Hints

```c
#include "driver/gptimer.h"
#include "esp_task_wdt.h"
#include "esp_timer.h"

#define SOFT_TIMER_PERIOD_US (1000000ULL)     /* 1 s   */
#define GPTIMER_RESOLUTION   (1000000U)       /* 1 MHz -> 1 tick == 1 us */
#define GPTIMER_ALARM_TICKS  (250000ULL)      /* 250 ms at that resolution */
#define WDT_TIMEOUT_MS       (5000U)
#define STALL_DURATION_MS    (8000U)          /* deliberately longer than the timeout */

static volatile uint32_t hw_alarm_count = 0;

/* --- part 1: runs in the esp_timer task, so logging here is allowed --- */
static void soft_timer_cb(void* arg)
{
    /* increment, ESP_LOGI */
}

/* --- part 2: runs in ISR context. Count and leave. --- */
static bool IRAM_ATTR hw_timer_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t* edata, void* user_ctx)
{
    hw_alarm_count++;
    return false; /* what does this return value control? Look it up. */
}

const esp_timer_create_args_t soft_args = {
    .callback = soft_timer_cb,
    .name     = "uptime",
};

gptimer_config_t hw_cfg = {
    .clk_src       = GPTIMER_CLK_SRC_DEFAULT,
    .direction     = GPTIMER_COUNT_UP,
    .resolution_hz = GPTIMER_RESOLUTION,
};

gptimer_alarm_config_t alarm_cfg = {
    .alarm_count                = GPTIMER_ALARM_TICKS,
    .reload_count               = 0,
    .flags.auto_reload_on_alarm = true,
};
```

`gptimer` has a strict call order — create, set the alarm action, register callbacks, **enable**, then start. Skipping `gptimer_enable()` is the usual reason a hardware timer silently never fires. For the watchdog, note that `esp_task_wdt_init()` may already have been called by the framework; read the return value rather than assuming, and look at what `esp_task_wdt_reconfigure()` is for.

### Suggested Approach

```
app_main():
  1. esp_timer_create(&soft_args, &soft_handle)
     esp_timer_start_periodic(soft_handle, SOFT_TIMER_PERIOD_US)
  2. gptimer_new_timer -> set_alarm_action -> register_event_callbacks
     -> gptimer_enable -> gptimer_start
  3. subscribe this task to the TWDT
  4. loop:
       log hw_alarm_count so you can see the hardware timer advancing
       feed the watchdog
       yield
  5. somewhere reachable: stall_cpu(STALL_DURATION_MS) — a busy-wait on
     esp_timer_get_time() that never yields and never feeds the watchdog.
     Run it once with the watchdog subscribed to see what happens, then
     comment it out and keep the fixed paths.
```

### Common Pitfalls to Explain in Your Submission

Short paragraph each, in `timer_notes.md`.

1. You now have two timers that both "fire periodically". State the real difference: where each callback runs, what happens to each if a high-priority task hogs the CPU, and which one you would use to sample a sensor at exactly 1 kHz. Justify the choice.
2. When the watchdog tripped, the panic message named a specific task that failed to reset it in time. Which task was it, and why that one rather than the task you wrote? Explain what the idle task has to do with any of this.
3. You implemented two fixes. One feeds the watchdog inside the long loop; the other stops the loop from hogging the CPU. Both stop the reboot — but only one of them actually fixes the underlying problem. Say which, and what the other one is really doing.

### Expected Output

With the stall disabled, both timers run side by side indefinitely — the software timer once a second, the hardware timer four times a second:

```
I (1002) TIMERS: uptime = 1 s
I (1003) TIMERS: hw_alarm_count = 4
I (2002) TIMERS: uptime = 2 s
I (2003) TIMERS: hw_alarm_count = 8
I (3002) TIMERS: uptime = 3 s
I (3003) TIMERS: hw_alarm_count = 12
```

`hw_alarm_count` must climb by exactly 4 per second. If it drifts, your resolution or alarm count is wrong.

With the stall enabled, the board prints a watchdog panic and reboots a few seconds in. You do not need to submit that output — you need to be able to explain it in `timer_notes.md`.

### Submission

```
Exercise_1/
├── main/
│   ├── main.c              (required — fixed version; stalling version commented out and labelled)
│   └── CMakeLists.txt      (required)
├── CMakeLists.txt          (required — ESP-IDF project root)
└── timer_notes.md          (required — answers to the 3 questions above)
```

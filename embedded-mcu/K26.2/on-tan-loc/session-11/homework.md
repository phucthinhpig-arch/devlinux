# Assignment — Session: 11
**Deadline: 2026-09-20 23:59:00**

---

## Exercise_1 — Light Sleep, Deep Sleep, and What Survives a Reboot [review-only]

### Problem Statement

Everything you have built so far assumes the board is plugged in. The moment a product runs on a battery, the interesting question stops being "how fast is it" and becomes "how long is it asleep". The ESP32-S3 has two sleep modes with a crucial difference: one resumes where it left off, the other does not resume at all — it reboots.

Build one program that demonstrates both and the consequence of the difference:

1. **Light sleep.** Enter light sleep with two wake sources armed at once: a timer for 5 seconds, and the button. On wake, log which source woke you and confirm that execution continued from the line after the sleep call rather than restarting.
2. **Deep sleep.** Then enter deep sleep, also with a timer and the button armed. Because deep sleep restarts the chip, use `app_main()` itself to report what happened: log the wake cause on every boot.
3. **What survives.** Keep two counters of boots: one in RTC memory, one an ordinary global. Log both on every wake. One of them counts up across deep sleeps and the other does not, and demonstrating that difference is the point of the exercise.

Requirements:
- Use `esp_sleep.h`. Arm the timer with `esp_sleep_enable_timer_wakeup()`, and read the cause after waking with `esp_sleep_get_wakeup_cause()`. Print the cause as a readable name, not a bare integer.
- For the button, note that light sleep and deep sleep use **different** wake mechanisms on this chip. Find out which function each one needs — the GPIO wake source you enable for light sleep is not the one that works from deep sleep. Getting this wrong produces a board that sleeps and never wakes on the button, only on the timer.
- The button must be on an RTC-capable GPIO, or deep-sleep wake on that pin is impossible. The pin given below is a valid one; if you change it, check it against the datasheet first.
- The RTC-memory counter must be declared with `RTC_DATA_ATTR`. The other counter must be a plain global initialised to zero. Do not make either one `static const` or otherwise optimise the comparison away.
- Turn the display backlight off before sleeping and back on after waking from light sleep. Leaving a backlight on defeats the entire exercise, and noticing that is part of the lesson.
- Log a clear banner at the start of `app_main()` so a reviewer can tell a fresh power-on from a deep-sleep wake in the output.
- Every timeout and pin must be a named constant.
- Answer the questions in `sleep_notes.md`.

### Hardware

Keep the display module wired as it is — you only need its backlight pin here. Keep the button from Session 10:

| Part | ESP32-S3 | Notes |
| --- | --- | --- |
| push-button | GPIO16 | other side to GND, internal pull-up, RTC-capable |
| display backlight | GPIO2 | unchanged from Session 06 |

A multimeter in series with the board's supply lets you watch the current drop when it sleeps. That measurement is **optional and not graded** — question 3 below asks you to reason about it rather than report a number.

### Design Hints

```c
#include "esp_sleep.h"

#define BTN_PIN            GPIO_NUM_16
#define PIN_BK_LIGHT       GPIO_NUM_2
#define LIGHT_SLEEP_US     (5000000ULL)  /* 5 s  */
#define DEEP_SLEEP_US      (10000000ULL) /* 10 s */
#define BTN_WAKE_LEVEL     (0)           /* pull-up, so a press is a LOW */

RTC_DATA_ATTR static uint32_t rtc_boot_count = 0;
static uint32_t               ram_boot_count = 0;

static const char* wakeup_cause_name(esp_sleep_wakeup_cause_t cause)
{
    switch (cause)
    {
    case ESP_SLEEP_WAKEUP_TIMER:
        return "TIMER";
    /* ... EXT0, EXT1, GPIO, UNDEFINED ... */
    default:
        return "POWER_ON / RESET";
    }
}
```

Two things to work out from the API reference rather than by trial and error. First, `esp_light_sleep_start()` **returns** and your next line runs; `esp_deep_sleep_start()` never returns at all. That difference decides where in your program each piece of logging has to live. Second, arming a GPIO as a wake source is a two-step job for light sleep — enabling the sleep wake source is not the same call as enabling the wake capability on the pin itself.

`RTC_DATA_ATTR` places a variable in RTC slow memory, which keeps its power domain alive through deep sleep. That is why one of your two counters survives. Confirm the mechanism in the docs before you write the answer to question 1 — a plausible-sounding guess is not the answer.

### Suggested Approach

```
app_main():
  1. rtc_boot_count++;  ram_boot_count++;
  2. cause = esp_sleep_get_wakeup_cause()
     log a banner: cause name, rtc_boot_count, ram_boot_count
  3. configure BTN_PIN as an input with pull-up
  4. --- light sleep demo ---
     backlight off
     arm the timer for LIGHT_SLEEP_US, arm the button as a light-sleep wake source
     log "entering light sleep"
     esp_light_sleep_start()
     backlight on
     log "resumed from light sleep, cause = ..."     <- this line proves it resumed
  5. --- deep sleep demo ---
     backlight off
     arm the timer for DEEP_SLEEP_US, arm the button as a deep-sleep wake source
     log "entering deep sleep — see you in app_main()"
     esp_deep_sleep_start()
     /* nothing here ever executes; a comment saying so is worth writing */
```

### Common Pitfalls to Explain in Your Submission

Short answers in `sleep_notes.md`.

1. After several deep-sleep cycles, one counter reads 5 while the other still reads 1. Say which is which and explain the mechanism — what specifically happens to normal RAM across a deep sleep, and why RTC memory escapes it.
2. You had to use two different functions to arm the button, one per sleep mode. Name both, and explain what is still powered in light sleep that is switched off in deep sleep such that the light-sleep mechanism cannot work from deep sleep.
3. You did not have to measure the current draw, but predict it: rank power-on-idle, light sleep and deep sleep from highest to lowest consumption, and for each say what is still running that explains its position. Then say what the display module contributes to all three, and whether your program actually eliminates it.

### Expected Output

First boot from power-on, then repeated deep-sleep wakes. The RTC counter climbs while the RAM counter is stuck at 1:

```
I (0312) SLEEP: === boot: cause=POWER_ON / RESET  rtc_boots=1  ram_boots=1 ===
I (0320) SLEEP: entering light sleep (5 s or button)
I (5325) SLEEP: resumed from light sleep, cause=TIMER
I (5330) SLEEP: entering deep sleep — see you in app_main()

I (0311) SLEEP: === boot: cause=TIMER  rtc_boots=2  ram_boots=1 ===
I (0319) SLEEP: entering light sleep (5 s or button)
I (2140) SLEEP: resumed from light sleep, cause=EXT0
I (2145) SLEEP: entering deep sleep — see you in app_main()

I (0311) SLEEP: === boot: cause=EXT0  rtc_boots=3  ram_boots=1 ===
```

Two things to look for. The timestamp resets to near zero on every deep-sleep wake, because the chip really did reboot. And pressing the button during either sleep wakes it early, with the cause reported as the button rather than the timer.

> If you are monitoring over the **USB** port, be aware that deep sleep drops the USB Serial/JTAG connection and `idf.py monitor` may need to reconnect on each wake. Using the **UART** port for this exercise gives a cleaner log — the same two-port arrangement you set up in Session 05.

### Submission

```
Exercise_1/
├── main/
│   ├── main.c              (required — light sleep + deep sleep, both wake sources)
│   └── CMakeLists.txt      (required)
├── CMakeLists.txt          (required — ESP-IDF project root)
└── sleep_notes.md          (required — answers to the 3 questions above)
```

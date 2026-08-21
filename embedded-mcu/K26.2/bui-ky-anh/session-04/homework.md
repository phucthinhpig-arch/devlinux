# Assignment — Session: 04
**Deadline: 2026-08-23 23:59:00**

---

## Exercise_1 — The Same Counter, Interrupt-Driven [review-only]

### Problem Statement

In Session 03 the CPU asked "has the button changed yet?" hundreds of times a second and threw the answer away almost every time. That is polling. An interrupt inverts the arrangement: the hardware notices the edge and calls you, so the CPU spends no cycles waiting.

Rebuild Session 03 Exercise_1 — the same single-digit 7-segment counter, the same wiring, the same three gestures — with the button detected by a **GPIO interrupt** instead of a polling loop. The user-visible behaviour must be identical.

| Gesture | Effect on the counter |
| --- | --- |
| Single click | `+1` |
| Double click | `-1` |
| Long press | `+1` every 500 ms, repeating for as long as the button is held |

Requirements:
- Configure the button through the **Driver API** (`driver/gpio.h`, `gpio_config()`) with `.intr_type = GPIO_INTR_ANYEDGE`. You need both edges: the falling edge starts a press, the rising edge ends it, and the gap between them is what separates a click from a long press.
- Install the ISR service with `gpio_install_isr_service()` and register your handler with `gpio_isr_handler_add()`.
- The ISR must be marked `IRAM_ATTR` and must do the **absolute minimum**: capture what happened and when, hand it off, return. No logging, no delays, no digit arithmetic, no display writes inside the ISR.
- Nowhere in your program may a loop read the button's pin level. If the interrupt were removed, the program must go completely deaf — that is the proof it is genuinely interrupt-driven.
- Keep driving the 7-segment display with your **register-level** code from Session 03, copied into this project. Only the button's detection path changes. You are deliberately running both layers in one program.
- Any variable shared between the ISR and a task must be `volatile`, or must be passed through a FreeRTOS primitive that is safe to use from an ISR.
- Debounce still applies. Interrupts do not fix bounce — they make it worse, because now every bounce edge costs you an interrupt instead of being quietly missed between two polls.
- Answer the three questions below in `isr_notes.md`.

### Hardware

Identical to Session 03 — same display, same resistors, same button on **GPIO14**, same segment pins (`a`–`g` on GPIO4, 5, 6, 7, 15, 16, 17). Do not rewire anything.

### Design Hints

```c
#include "driver/gpio.h"
#include "esp_attr.h"   /* IRAM_ATTR */
#include "esp_timer.h"  /* esp_timer_get_time() — microseconds, safe from an ISR */

#define BTN_PIN GPIO_NUM_14

typedef struct
{
    int64_t timestamp_us;
    bool    is_press; /* falling edge = press, rising edge = release */
} btn_event_t;

static QueueHandle_t btn_queue; /* created with xQueueCreate() before the ISR is installed */

static void IRAM_ATTR button_isr(void* arg)
{
    /* read the level, stamp the time, xQueueSendFromISR() — and nothing else */
}
```

`gpio_config()` takes a `.pin_bit_mask` of the pins to configure, plus the mode, pull-up and interrupt type. Look up `xQueueSendFromISR()` before you use it: it has a different signature from `xQueueSend()` for a reason, and that reason is worth understanding.

### Suggested Approach

```
app_main():
  init the display (register level, copied from Session 03)
  create the event queue
  configure BTN_PIN as an input with pull-up and ANYEDGE interrupt
  install the ISR service, add the handler
  start the gesture task

gesture task:
  block on the queue and decode press/release events into gestures.

  The long-press repeat is the interesting part. While the button is held, no
  further edges arrive — the ISR has nothing to report until release. So the
  repeat cannot be driven by the queue alone. Blocking on the queue with a
  TIMEOUT rather than forever gives you both the next edge and a periodic tick
  from the same call. Work out what the timeout should be in each state.
```

You solved the same decoding problem in Session 03 with a polling loop. Notice which parts of that logic survive the move to interrupts unchanged, and which parts had to be rethought — question 3 asks about exactly that.

### Common Pitfalls to Explain in Your Submission

Short paragraph each, in `isr_notes.md`. These are graded.

1. Why must a flag shared between the ISR and a task be `volatile`? Say specifically what the compiler is allowed to do to a `while (!flag)` or `if (flag)` check without it, and why the ISR writing the variable does not prevent that.
2. Why is calling `ESP_LOGI()` or `vTaskDelay()` from inside `button_isr()` dangerous? Name the mechanism, not just "it is slow".
3. Compare your two implementations. Which is more responsive, and by how much in the worst case? Which uses less CPU while the button is untouched? And which one was easier to get right — answer honestly, and say why.

### Expected Output

Behaviour indistinguishable from Session 03: `0` after reset, click for `+1`, double-click for `-1`, hold for `+1` every 500 ms with correct wrapping, and no stray extra count when a long press ends.

While developing, logging each decoded gesture from the task (never from the ISR) makes the difference easy to see:

```
I (3128) BTN: CLICK      -> 1
I (4902) BTN: CLICK      -> 2
I (6015) BTN: DOUBLE     -> 1
I (8440) BTN: LONG start -> 2
I (8940) BTN: LONG rep   -> 3
I (9440) BTN: LONG rep   -> 4
I (9782) BTN: LONG end
```

If a single press produces two or three events a few milliseconds apart, your debounce is not working — with interrupts you will see this far more readily than you did in Session 03.

### Submission

```
Exercise_1/
├── main/
│   ├── main.c              (required — ISR-driven button, register-level display)
│   └── CMakeLists.txt      (required)
├── CMakeLists.txt          (required — ESP-IDF project root)
└── isr_notes.md            (required — answers to the 3 questions above)
```

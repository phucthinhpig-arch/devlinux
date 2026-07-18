# Assignment — Session 03: Register-Level vs Driver API Programming
**Deadline: 2026-06-07 23:59:00**

---

> ⚠️ **YÊU CẦU QUAN TRỌNG (áp dụng cho TẤT CẢ exercise trong file này):**
> **BẮT BUỘC phải sử dụng ESP-IDF** (register-level access qua `esp_attr.h`/thanh ghi trực tiếp như mô tả, hoặc Driver API của ESP-IDF khi được yêu cầu).
> **CẤM tuyệt đối sử dụng code kiểu Arduino** — không `#include <Arduino.h>`, không dùng các hàm/API của Arduino framework (`pinMode()`, `digitalWrite()`, `digitalRead()`, `Serial.print()`, `delay()`, `analogRead()`, v.v.), không dùng Arduino-ESP32 core, không build bằng `arduino-cli`/Arduino IDE. Bài nộp dùng Arduino API sẽ bị coi là **không đạt yêu cầu**, dù chức năng có đúng.

---

## Exercise_1 [review-only]

### Problem Statement

Blink an external LED wired to **GPIO45** using **direct register access only** — no `driver/gpio.h`, no `gpio_config()`, no `gpio_set_level()`. You must configure the pin's `IO_MUX` function, enable it as an output, and toggle it using the `GPIO_OUT_W1TS`/`GPIO_OUT_W1TC` "write-1-to-set/clear" registers, per the ESP32-S3 Technical Reference Manual.

**Rules:**
- No calls into `driver/gpio.h` anywhere in this exercise.
- All register addresses/bit positions must be named constants (`#define` or `enum`), never magic numbers inline.
- Every register pointer must be declared `volatile`.
- GPIO45 is pin ≥ 32, so it lives in the **second bank** of GPIO registers (`GPIO_OUT1_*`, `GPIO_ENABLE1_*`) — make sure you use the correct bank and correct bit offset (`pin - 32`).

### Design Hints

```c
#define DR_REG_GPIO_BASE      (0x60004000UL)
#define DR_REG_IO_MUX_BASE    (0x60009000UL)
#define GPIO_ENABLE1_W1TS_REG (*(volatile uint32_t*) (DR_REG_GPIO_BASE + 0x30U))
#define GPIO_OUT1_W1TS_REG    (*(volatile uint32_t*) (DR_REG_GPIO_BASE + 0x14U))
#define GPIO_OUT1_W1TC_REG    (*(volatile uint32_t*) (DR_REG_GPIO_BASE + 0x18U))
#define IO_MUX_GPIO_REG(n)    (*(volatile uint32_t*) (DR_REG_IO_MUX_BASE + 0x4U + (n) * 4U))

#define REGISTER_WIDTH_BITS (32U)
#define LED_PIN             (45U)
```

### Suggested Approach

```
1. gpio_init_led(): configure IO_MUX drive strength + disable input + enable output bit for LED_PIN
2. gpio_register_set_high(pin) / gpio_register_set_low(pin): pick GPIO_OUT_* vs GPIO_OUT1_*
   based on whether pin < 32 or pin >= 32
3. app_main(): call gpio_init_led() once, then loop set_high -> delay 1000ms -> set_low -> delay 1000ms
```

Check the **ESP32-S3 Technical Reference Manual** (chapter: IO MUX and GPIO Matrix) to confirm every bit offset you use — do not guess.

### Submission

```
Exercise_1/
└── main/
    ├── main.c              (required — register-level only)
    └── CMakeLists.txt      (required)
```

---

## Exercise_2 [review-only]

### Problem Statement

Combine input and output at the **register level**: read a push-button on **GPIO4** (configured as input with internal pull-up) and mirror its state onto the LED on **GPIO45** (button pressed = LED on), using polling — check the button state in a loop, no interrupts yet (interrupts are Session 04's topic).

**Rules:**
- GPIO4 must be configured as input with pull-up enabled and output disabled, entirely via `IO_MUX`/`GPIO_ENABLE` registers.
- Read the button state via the `GPIO_IN_REG` register (bit `BTN_PIN`).
- Reuse your `gpio_register_set_high()` / `gpio_register_set_low()` helpers from Exercise_1 to drive the LED.
- Poll on a `vTaskDelay(pdMS_TO_TICKS(50))` cycle (do not busy-loop with no delay — that starves the FreeRTOS idle task).

### Expected Output

Pressing the button turns the LED on; releasing it turns the LED off, with at most ~50 ms of visible lag.

### Submission

```
Exercise_2/
└── main/
    ├── main.c              (required — register-level input + output, polling)
    └── CMakeLists.txt      (required)
```

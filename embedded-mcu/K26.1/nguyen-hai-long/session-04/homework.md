# Assignment — Session 04: Polling and Interrupt
**Deadline: 2026-06-07 23:59:00**

---

> ⚠️ **YÊU CẦU QUAN TRỌNG (áp dụng cho TẤT CẢ exercise trong file này):**
> **BẮT BUỘC phải sử dụng ESP-IDF** (Driver API — `driver/gpio.h`, `gpio_config()`, `gpio_install_isr_service()`, `gpio_isr_handler_add()` — kết hợp với register-level cho LED như đã làm ở Session 03).
> **CẤM tuyệt đối sử dụng code kiểu Arduino** — không `#include <Arduino.h>`, không dùng `attachInterrupt()`, `pinMode()`, `digitalWrite()`, `digitalRead()`, `Serial.print()`, `delay()`, không dùng Arduino-ESP32 core, không build bằng `arduino-cli`/Arduino IDE. Bài nộp dùng Arduino API sẽ bị coi là **không đạt yêu cầu**, dù chức năng có đúng.

---

## Exercise_1 [review-only]

### Problem Statement

Using the same button (**GPIO4**) and LED (**GPIO45**) wiring from Session 03, implement **interrupt-driven** button-toggle-LED behavior: each button press must **toggle** the LED state (on→off, off→on), detected via a falling-edge GPIO interrupt — **not polling**.

**Rules:**
- Configure GPIO4 via the **Driver API** (`gpio_config()`) with `.intr_type = GPIO_INTR_NEGEDGE`.
- Install the ISR service (`gpio_install_isr_service`) and register your handler (`gpio_isr_handler_add`).
- Your ISR must be marked `IRAM_ATTR` and must do the **absolute minimum work** — set a flag only. All actual logic (toggling `led_state`, updating the LED) must happen in `app_main()`'s main loop, not inside the ISR.
- Drive the LED via register-level access (`gpio_register_set_high`/`_low`, from Session 03 Exercise_1) — only the button detection mechanism uses the Driver API.

### Design Hints

```c
static volatile bool btn_pressed = false;
static bool led_state = false;

static void IRAM_ATTR button_isr(void* arg)
{
    btn_pressed = true;
}

gpio_config_t btn_cfg = {
    .pin_bit_mask = (1ULL << BTN_PIN),
    .mode = GPIO_MODE_INPUT,
    .pull_up_en = GPIO_PULLUP_ENABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type = GPIO_INTR_NEGEDGE,
};
gpio_config(&btn_cfg);
gpio_install_isr_service(0);
gpio_isr_handler_add(BTN_PIN, button_isr, NULL);
```

### Suggested Approach

```
1. Configure LED output (register-level, from Session 03)
2. Configure button as interrupt input (driver API, negative edge)
3. Install ISR service + register handler
4. Main loop: if btn_pressed flag is set -> clear it, flip led_state, drive LED accordingly
5. vTaskDelay(pdMS_TO_TICKS(50)) in the main loop is just idle housekeeping, not the detection mechanism
```

### Common Pitfalls to Explain in Your Submission

Answer in a short write-up:
1. Why must `btn_pressed` be declared `volatile`? What could go wrong if it isn't (think: compiler optimizing away the flag check in the main loop)?
2. Why is it dangerous to call `ESP_LOGI()` or `vTaskDelay()` directly inside `button_isr()`?
3. A real push-button "bounces" — it can fire the interrupt several times within a few milliseconds of a single press. Your current code doesn't handle this. Describe (in words, code optional) one simple debounce strategy you could add.

### Submission

```
Exercise_1/
├── main/
│   ├── main.c              (required — ISR-driven button toggle, register-level LED)
│   └── CMakeLists.txt      (required)
└── isr_notes.md            (required — answers to the 3 pitfall questions)
```

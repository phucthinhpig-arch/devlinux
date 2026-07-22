# Assignment — Session 05: UART
**Deadline: 2026-06-14 23:59:00**

---

> ⚠️ **YÊU CẦU QUAN TRỌNG (áp dụng cho TẤT CẢ exercise trong file này):**
> **BẮT BUỘC phải sử dụng ESP-IDF** (`driver/uart.h`, `uart_driver_install()`, `uart_param_config()`, `uart_set_pin()`, FreeRTOS task/queue của ESP-IDF).
> **CẤM tuyệt đối sử dụng code kiểu Arduino** — không `#include <Arduino.h>`, không dùng `Serial.begin()`, `Serial.print()`, `Serial.available()`, `Serial.read()`, không dùng thư viện Arduino cho `led_strip`/NeoPixel kiểu Arduino, không dùng Arduino-ESP32 core, không build bằng `arduino-cli`/Arduino IDE. Bài nộp dùng Arduino API sẽ bị coi là **không đạt yêu cầu**, dù chức năng có đúng.

---

## Exercise_1 [review-only]

### Problem Statement

Build an **interrupt-driven UART command console** on `UART_NUM_1` (TX = GPIO17, RX = GPIO18, 115200-8-N-1) that controls the onboard RGB LED (`led_strip`, same LED as Session 01). The console must:

- Echo every typed character back to the terminal.
- Support Backspace/DEL to edit the current line before pressing Enter.
- On Enter (`\r` or `\n`), parse the accumulated line as a command and act on it:
  - `LED_ON` → white
  - `LED_OFF` → off
  - `RED` / `GREEN` / `BLUE` → set that color
  - anything else → log a warning, do **not** crash or hang

**Rules:**
- Use `uart_driver_install()` with an event queue (`QueueHandle_t`) — this is interrupt-driven RX, not polling `uart_read_bytes()` in a tight loop.
- Run the UART event handling in its own FreeRTOS task (`xTaskCreatePinnedToCore`), separate from any other logic.
- Your command buffer must be bounds-checked — a line longer than your buffer must never overflow it.
- Handle `UART_FIFO_OVF` and `UART_BUFFER_FULL` events by flushing and resetting state, not by ignoring them.

### Design Hints

```c
#define UART_PORT_NUM   UART_NUM_1
#define UART_TX_PIN     GPIO_NUM_17
#define UART_RX_PIN     GPIO_NUM_18
#define UART_BAUD_RATE  115200UL
#define UART_BUF_SIZE   1024U
#define UART_QUEUE_SIZE 10U

uart_config_t uart_config = {
    .baud_rate = UART_BAUD_RATE,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .source_clk = UART_SCLK_DEFAULT,
};

QueueHandle_t uart_queue;
ESP_ERROR_CHECK(uart_driver_install(UART_PORT_NUM, UART_BUF_SIZE, UART_BUF_SIZE, UART_QUEUE_SIZE, &uart_queue, 0));
ESP_ERROR_CHECK(uart_param_config(UART_PORT_NUM, &uart_config));
ESP_ERROR_CHECK(uart_set_pin(UART_PORT_NUM, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
```

### Expected Output

Typing `RED<Enter>` in the serial terminal turns the onboard LED red and logs:
```
I ([UART][ControlLED]): Received command: "RED"
I ([UART][ControlLED]): LED -> RED
```
Typing an unrecognized command logs a `W` (warning) line and leaves the LED unchanged.

### Submission

```
Exercise_1/
└── main/
    ├── main.c              (required)
    └── CMakeLists.txt      (required)
```

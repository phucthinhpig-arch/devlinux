# Assignment — Session: 05
**Deadline: 2026-08-30 23:59:00**

---

**Hardware needed:** ESP32-S3 DevKitC-1 and **two** USB cables. Nothing else — the RGB LED this exercise controls is already on the board.

---

### Hardware and Setup — read this before writing any code

This exercise is the first one where **your log output and your console are two different physical channels**. Getting this wrong costs more time than the code does, so set it up first.

The ESP32-S3 DevKitC-1 has **two USB ports**. Connect a cable to **both**:

| Port on the board | Path | Role in this exercise |
|---|---|---|
| **USB** (Native USB, wired to the S3's own D+/D− pins) | USB Serial/JTAG peripheral | `idf.py flash monitor` runs here. **This is where `ESP_LOGI` output appears.** |
| **UART** (through the CP2102N bridge chip, hard-wired to GPIO43 = TXD, GPIO44 = RXD) | `UART_NUM_0` | **This is the console you are building.** You type commands here and see characters echoed back. |

Before you start, you must route the ESP-IDF console away from UART0, otherwise the framework's own log output is injected into the same UART you are driving and the two streams interleave into garbage:

```
idf.py menuconfig
  -> Component config -> ESP System Settings -> Channel for console output
  -> USB Serial/JTAG Controller
```

(This is section 5.3 of `embedded-mcu/references/setup-dev-environment.md`.)

You will therefore have **two terminal windows open at once**: `idf.py monitor` on the USB port for logs, and any serial terminal (PuTTY, TeraTerm, or a second `idf.py monitor -p COMx`) on the UART port's COM number at 115200-8-N-1 for typing. No USB-to-TTL adapter needs to be purchased — the UART port on the DevKitC-1 already is one.

---

## Exercise_1 — Interrupt-Driven UART Command Console [review-only]

### Problem Statement

Almost every shipped embedded product has a serial command console behind a header on the PCB — it is how you commission a unit on the factory line and how you debug a returned one. This exercise builds a small one.

Write an **interrupt-driven UART command console** on `UART_NUM_0` (TX = GPIO43, RX = GPIO44, 115200-8-N-1) that controls the board's onboard addressable RGB LED. The console must:

- Echo every character back as it is typed, so the operator can see what they are entering.
- Support Backspace and DEL to correct the current line before it is submitted.
- On Enter (`\r` or `\n`), parse the accumulated line as a command and act on it:
  - `LED_ON` → white
  - `LED_OFF` → off
  - `RED` / `GREEN` / `BLUE` → that colour
  - anything else → log a warning at `ESP_LOGW` level, leave the LED alone, and carry on. An unrecognised command must never crash or hang the console.

The onboard RGB LED is a single WS2812 driven over RMT by the `led_strip` component:

```
idf.py add-dependency "espressif/led_strip^2.4.1"
```

It is on **GPIO48** on DevKitC-1 v1.0 and **GPIO38** on v1.1. Check the silkscreen on your own board.

Requirements:
- Use `uart_driver_install()` with an event queue (`QueueHandle_t`). The RX path must be interrupt-driven through UART events — polling `uart_read_bytes()` in a tight loop does not satisfy this exercise.
- Handle the UART events in a dedicated FreeRTOS task created with `xTaskCreatePinnedToCore()`, separate from `app_main()`.
- The command buffer must be bounds-checked. A line longer than the buffer must be rejected or truncated safely; it must never write past the end.
- Handle the `UART_FIFO_OVF` and `UART_BUFFER_FULL` events by flushing the input and resetting the line state. Silently ignoring them is not acceptable.
- State your board revision and the RGB LED GPIO you used in a comment at the top of `main.c`, e.g. `/* DevKitC-1 v1.1, RGB LED on GPIO38 */`. A reviewer must not have to guess which pin your code assumes.
- Every command string and buffer size must be a named constant.

### Design Hints

```c
#include "driver/uart.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "led_strip.h"

#define UART_PORT_NUM   UART_NUM_0
#define UART_TX_PIN     GPIO_NUM_43
#define UART_RX_PIN     GPIO_NUM_44
#define UART_BAUD_RATE  115200UL
#define UART_BUF_SIZE   1024U
#define UART_QUEUE_SIZE 10U
#define CMD_BUF_SIZE    64U

#define RGB_LED_PIN     GPIO_NUM_38 /* v1.0 boards: GPIO_NUM_48 */

uart_config_t uart_config = {
    .baud_rate  = UART_BAUD_RATE,
    .data_bits  = UART_DATA_8_BITS,
    .parity     = UART_PARITY_DISABLE,
    .stop_bits  = UART_STOP_BITS_1,
    .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,
    .source_clk = UART_SCLK_DEFAULT,
};

QueueHandle_t uart_queue;
ESP_ERROR_CHECK(uart_driver_install(UART_PORT_NUM, UART_BUF_SIZE, UART_BUF_SIZE, UART_QUEUE_SIZE, &uart_queue, 0));
ESP_ERROR_CHECK(uart_param_config(UART_PORT_NUM, &uart_config));
ESP_ERROR_CHECK(uart_set_pin(UART_PORT_NUM, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
```

### Suggested Approach

```
app_main():
  1. Initialize the led_strip handle for RGB_LED_PIN, clear it to off
  2. uart_param_config / uart_set_pin / uart_driver_install with &uart_queue
  3. xTaskCreatePinnedToCore(uart_event_task, ...)

uart_event_task():
  forever:
    xQueueReceive(uart_queue, &event, portMAX_DELAY)
    switch (event.type):
      case UART_DATA:
        uart_read_bytes() the event.size bytes
        for each byte:
          - printable        -> append to cmd_buf if there is room, echo it back
          - Backspace / DEL  -> drop last char if any, echo "\b \b" to erase on screen
          - '\r' or '\n'     -> null-terminate, echo newline, handle_command(cmd_buf),
                                reset the buffer length to 0
      case UART_FIFO_OVF:
      case UART_BUFFER_FULL:
        uart_flush_input(); xQueueReset(uart_queue); reset cmd_buf length
        ESP_LOGW(TAG, "UART overflow, input flushed")
      default:
        log the event type and ignore

handle_command(cmd):
  compare against the known commands, set the LED colour, ESP_LOGI what happened
  no match -> ESP_LOGW and return without touching the LED
```

### Expected Output

```
# Terminal 1 – UART port (where you type):
RED
LED_OFF
BOGUS

# Terminal 2 – idf.py monitor on the USB port (where the logs appear):
I (1120) UART_CONSOLE: Console ready on UART0, 115200-8-N-1
I (8431) UART_CONSOLE: Received command: "RED"
I (8431) UART_CONSOLE: LED -> RED
I (12094) UART_CONSOLE: Received command: "LED_OFF"
I (12094) UART_CONSOLE: LED -> OFF
W (15660) UART_CONSOLE: Unknown command: "BOGUS"
```

Two things about this are deliberate and correct, not bugs:

- The characters you type are echoed in **Terminal 1**, while the log lines appear in **Terminal 2**. They travel over two different physical links.
- Typing `BOGUS` leaves the LED in whatever state it was already in. The warning is the only effect.

### Submission

```
Exercise_1/
├── main/
│   ├── main.c              (required — board revision and LED GPIO in a header comment)
│   ├── CMakeLists.txt      (required)
│   └── idf_component.yml   (required — created by `idf.py add-dependency "espressif/led_strip^2.4.1"`)
└── CMakeLists.txt          (required — ESP-IDF project root)
```

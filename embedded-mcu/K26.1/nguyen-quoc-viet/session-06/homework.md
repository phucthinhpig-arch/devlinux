# Assignment — Session 06: I2C
**Deadline: 2026-06-14 23:59:00**

---

> ⚠️ **YÊU CẦU QUAN TRỌNG (áp dụng cho TẤT CẢ exercise trong file này):**
> **BẮT BUỘC phải sử dụng ESP-IDF** (`driver/i2c_master.h` cho I2C, ESP-IDF component registry qua `idf.py add-dependency` nếu dùng driver SSD1306 có sẵn).
> **CẤM tuyệt đối sử dụng code kiểu Arduino** — không `#include <Arduino.h>`, không dùng `Wire.h`/`Wire.begin()`/`Wire.write()`/`Wire.read()`, không dùng các thư viện Arduino cho DHT11/DHT20 (ví dụ `DHT.h`, `Adafruit_DHT`) hoặc SSD1306 kiểu Arduino (ví dụ `Adafruit_SSD1306`, `U8g2` bản Arduino), không dùng Arduino-ESP32 core, không build bằng `arduino-cli`/Arduino IDE. Bài nộp dùng Arduino API/thư viện Arduino sẽ bị coi là **không đạt yêu cầu**, dù chức năng có đúng.

---

## Exercise_1 [review-only]

### Problem Statement

Using GPIO, I2C, and the ESP-IDF SDK on the ESP32-S3, read the temperature from a **DHT20** sensor (I2C) — or a **DHT11** (1-wire) if that's the hardware you have — and display it on an **SSD1306** OLED (I2C).

**Hardware:** ESP32-S3 DevKitC v1, the ESP32-S3 Technical Reference Manual, a DHT11 or DHT20 module, an SSD1306 OLED module.

**Rules:**
- Pick **one** sensor — DHT20 (I2C, address `0x38`) or DHT11 (1-wire, any free GPIO) — and document which one you used plus your exact wiring (GPIO numbers for SDA/SCL or the 1-wire data pin) in a comment at the top of `main.c`.
- **If DHT20:** implement the AHT20/DHT20 protocol yourself directly on top of ESP-IDF's I2C master driver (`driver/i2c_master.h`) — do not pull in a ready-made DHT20 driver component. The point of this exercise is that *you* handle the I2C transactions and data decoding.
- **If DHT11:** implement the 1-wire bit-bang timing protocol yourself on a GPIO you configure. You may implement the edge-timing capture either by busy-wait polling (simplest) or by a GPIO edge interrupt that only timestamps edges (advanced, ties back to Session 04) — either approach is acceptable if correctly implemented.
- The SSD1306 talks I2C too (address `0x3C` typically) — you **may** use a maintained SSD1306 driver component from the ESP Component Registry (`idf.py add-dependency ...`) for the display, since the learning goal here is the *sensor's* I2C protocol, not OLED font rendering. Writing your own minimal SSD1306 command sequence is also fine if you prefer.
- **Cycle time: read the sensor and refresh the display every 5 seconds**, forever (`vTaskDelay(pdMS_TO_TICKS(5000))`).
- Log the parsed temperature at `ESP_LOGI` every cycle, in addition to showing it on the LCD.
- All I2C addresses/command bytes must be named constants, never magic numbers inline.
- Every I2C transaction must check its return value (`ESP_ERROR_CHECK` or explicit handling) — a failed read must not crash the app or leave garbage on the display.

### Design Hints — DHT20 over I2C (ESP-IDF v5.2 `i2c_master` driver)

```c
#define DHT20_I2C_ADDR   (0x38U)

i2c_master_bus_config_t bus_config = {
    .i2c_port = I2C_NUM_0,
    .sda_io_num = GPIO_NUM_8,
    .scl_io_num = GPIO_NUM_9,
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .glitch_ignore_cnt = 7,
    .flags.enable_internal_pullup = true,
};
i2c_master_bus_handle_t bus_handle;
ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &bus_handle));

i2c_device_config_t dht20_cfg = {
    .dev_addr_length = I2C_ADDR_BIT_LEN_7,
    .device_address = DHT20_I2C_ADDR,
    .scl_speed_hz = 100000,
};
i2c_master_dev_handle_t dht20_handle;
ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dht20_cfg, &dht20_handle));

/* Trigger measurement */
uint8_t trigger_cmd[3] = {0xACU, 0x33U, 0x00U};
ESP_ERROR_CHECK(i2c_master_transmit(dht20_handle, trigger_cmd, sizeof(trigger_cmd), -1));
vTaskDelay(pdMS_TO_TICKS(80));

/* Read 7 bytes: [status][hum 20-bit][temp 20-bit][crc] */
uint8_t data[7];
ESP_ERROR_CHECK(i2c_master_receive(dht20_handle, data, sizeof(data), -1));

uint32_t raw_temp = ((uint32_t)(data[3] & 0x0FU) << 16) | ((uint32_t)data[4] << 8) | data[5];
float temperature_c = ((float)raw_temp / 1048576.0f) * 200.0f - 50.0f;   /* 1048576 = 2^20 */
```

Check the sensor's datasheet for the full sequence, including the initial status check (`0x71`) and calibration init (`0xBE 0x08 0x00`) if the calibration bit isn't already set — don't skip error/status handling to save time.

### Suggested Approach

```
1. Configure the I2C bus (SDA/SCL pins, pull-ups, clock speed)
2. Add both I2C devices on the bus: DHT20 (0x38) and SSD1306 (0x3C) — this is your first multi-slave I2C bus
3. Initialize the SSD1306 (via component or hand-written init sequence)
4. Loop forever:
   a. Trigger + read + decode the sensor (DHT20 over I2C, or DHT11 over 1-wire)
   b. ESP_LOGI the temperature
   c. Render the temperature as text on the OLED
   d. vTaskDelay(pdMS_TO_TICKS(5000))
```

### Expected Output

Serial monitor logging once every ~5 seconds:
```
I (...) [DHT]: Temperature = 26.4 C
```
and the OLED showing the same value as readable text, updating every cycle.

### Submission

```
Exercise_1/
└── main/
    ├── main.c              (required)
    ├── CMakeLists.txt      (required)
    └── idf_component.yml   (required only if you added an SSD1306 component dependency)
```

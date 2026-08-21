# Assignment — Session: 07
**Deadline: 2026-09-06 23:59:00**

---

## Exercise_1 — Reading the Capacitive Touch Panel Over I2C [review-only]

### Problem Statement

The module you brought up in Session 06 carries two completely separate buses. The display is on SPI. The capacitive touch panel bonded to the same piece of glass is on **I2C**, talking to an FT6336U controller. Same hardware, second bus — so this exercise is purely about becoming fluent with ESP-IDF's `i2c_master` driver.

Read the touch panel and make the display respond to it:

1. Initialise the I2C bus and add the touch controller as a device.
2. Read the controller's ID registers once at startup and log what you found. This is your proof the bus works before any touch logic exists.
3. Poll the panel about 20 times a second. When a finger is present, read the first touch point's X and Y coordinates.
4. Draw a small filled square, roughly 20×20 pixels, centred on the touch point — so dragging a finger leaves a trail on screen. Log each new touch coordinate with `ESP_LOGI`.

Requirements:
- Use `driver/i2c_master.h` — `i2c_new_master_bus()`, `i2c_master_bus_add_device()`, `i2c_master_transmit_receive()`. Do not use the deprecated `driver/i2c.h`; most tutorials online still do, so check the include before copying anything.
- Reading a register on this controller is one combined write-then-read transaction: write the register address, then read N bytes back without releasing the bus. `i2c_master_transmit_receive()` does exactly that in one call — use it rather than a transmit followed by a separate receive.
- Drive `CTP_RST` low then high at startup to reset the touch controller before you talk to it.
- Reuse your display code from Session 06, copied into this project, at the same 480×320 landscape orientation. `lcd_fill_rect()` is all you need from it. The SPI side must keep working unchanged while I2C runs alongside it — two drivers, two buses, one program.
- Clamp the square's coordinates so it is never drawn partly outside the panel. A touch at the very edge must not produce a window extending past `LCD_H_RES` or `LCD_V_RES`.
- Inside the polling loop, do **not** use `ESP_ERROR_CHECK()` on the I2C reads. That macro calls `abort()`, so one noisy read would reboot the board. Capture the `esp_err_t`, log it at `ESP_LOGW`, and carry on to the next cycle. `ESP_ERROR_CHECK()` is still correct for the one-time setup calls.
- Every register address, I2C address and bit mask must be a named constant.

### Hardware

The **MSP3526** variant of the module — the one **with** the capacitive touch panel. MSP3525 has no touch controller and cannot do this exercise; if that is what you have, tell your instructor before the deadline rather than after it.

Keep every wire from Session 06 exactly as it is, and add four:

| Module pin | ESP32-S3 | Role |
| --- | --- | --- |
| `CTP_SDA` | GPIO4 | I2C data |
| `CTP_SCL` | GPIO5 | I2C clock |
| `CTP_RST` | GPIO6 | touch controller reset, active low |
| `CTP_INT` | GPIO7 | touch interrupt — wire it, but polling is what this exercise asks for |

Once these four are added, the module is fully wired and stays that way for the rest of the course. Do not rearrange it.

### Design Hints

```c
#include "driver/i2c_master.h"

#define TOUCH_I2C_ADDR   (0x38U)
#define PIN_TOUCH_SDA    GPIO_NUM_4
#define PIN_TOUCH_SCL    GPIO_NUM_5
#define PIN_TOUCH_RST    GPIO_NUM_6
#define PIN_TOUCH_INT    GPIO_NUM_7
#define I2C_CLK_HZ       (400000U)
#define I2C_TIMEOUT_MS   (100)   /* finite — never -1 inside a polling loop */
#define POLL_PERIOD_MS   (50U)

/* FT6336U registers */
#define REG_TD_STATUS    (0x02U) /* low nibble = number of touch points */
#define REG_P1_XH        (0x03U) /* then XL, YH, YL in the next three registers */
#define REG_CHIP_ID      (0xA3U)
#define REG_VENDOR_ID    (0xA8U)

#define TOUCH_COORD_MASK (0x0FU) /* only the low 4 bits of the high byte are coordinate */

i2c_master_bus_config_t bus_cfg = {
    .i2c_port                     = I2C_NUM_0,
    .sda_io_num                   = PIN_TOUCH_SDA,
    .scl_io_num                   = PIN_TOUCH_SCL,
    .clk_source                   = I2C_CLK_SRC_DEFAULT,
    .glitch_ignore_cnt            = 7,
    .flags.enable_internal_pullup = true,
};

i2c_device_config_t touch_cfg = {
    .dev_addr_length = I2C_ADDR_BIT_LEN_7,
    .device_address  = TOUCH_I2C_ADDR,
    .scl_speed_hz    = I2C_CLK_HZ,
};

/* Read `len` bytes starting at `reg`, in one transaction */
static esp_err_t touch_read(i2c_master_dev_handle_t dev, uint8_t reg, uint8_t* buf, size_t len)
{
    return i2c_master_transmit_receive(dev, &reg, 1U, buf, len, I2C_TIMEOUT_MS);
}
```

A touch coordinate does not fit in one byte — the panel is up to 480 pixels across. Each axis is spread over two consecutive registers: the low byte holds the bottom 8 bits, and only the **low 4 bits** of the high byte belong to the coordinate. The upper bits of that high byte mean something else entirely, so mask before you combine. Read all four coordinate registers in a single transaction rather than four separate ones, then reassemble them in code.

For the ID registers, do not assume what they contain — read `REG_CHIP_ID` and `REG_VENDOR_ID`, log the raw values in hex, and compare against the FT6336U datasheet yourself. If those two reads fail, or return `0x00` or `0xFF`, your wiring or your pull-ups are wrong and no amount of touch logic will help.

### Suggested Approach

```
1. Init the display exactly as in Session 06, clear it to black
2. Reset the touch controller: CTP_RST low, delay, high, delay
3. i2c_new_master_bus() -> i2c_master_bus_add_device()
4. Read REG_CHIP_ID and REG_VENDOR_ID, ESP_LOGI them in hex.
   Stop and fix your wiring here if this step does not work.
5. forever:
     a. read REG_TD_STATUS -> how many fingers are down?
     b. if at least one:
          read the four coordinate registers in one transaction
          mask and combine into x and y
          clamp so the square stays fully on screen
          ESP_LOGI the coordinates
          lcd_fill_rect() a small square there
     c. vTaskDelay(pdMS_TO_TICKS(POLL_PERIOD_MS))
```

The interesting part is the last mile, and it is worth budgeting real time for. The touch controller reports coordinates in **its own** orientation, which is the panel's native portrait, while your display is running in the landscape orientation you set with `MADCTL` in Session 06. The two do not agree, so a raw coordinate pair will not land where your finger is.

Work out the mapping empirically rather than by reasoning about it in the abstract: log the raw values, drag your finger deliberately along each edge of the screen, and write down what the numbers do. From that you can see whether you need to swap the two axes, invert one of them, or both. Note in a comment what you concluded and the range you actually observed on each axis — the observed range is the useful part, because it tells the next person whether an axis really spans the full 0–479 or something narrower.

### Expected Output

At startup, before any touch — the exact ID values are whatever your controller reports, so treat the shape of the line as the target, not the numbers:

```
I (0318) TOUCH: chip_id=0x?? vendor_id=0x??
I (0322) TOUCH: display ready, waiting for touch
```

Then dragging a finger across the glass leaves a trail of small squares following it, and the log fills with coordinates:

```
I (4820) TOUCH: touch @ x=142 y=201
I (4871) TOUCH: touch @ x=150 y=207
I (4922) TOUCH: touch @ x=163 y=219
```

Lifting the finger stops both the squares and the logging. The trail stays on screen — you are not required to clear it.

The square must appear **under your fingertip**, not mirrored to the opposite side and not with the axes transposed. If it does not, that is the coordinate mapping described above, and it is part of the exercise rather than a bug in the driver.

### Submission

```
Exercise_1/
├── main/
│   ├── main.c              (required — i2c_master driver + SPI display from Session 06)
│   └── CMakeLists.txt      (required)
└── CMakeLists.txt          (required — ESP-IDF project root)
```

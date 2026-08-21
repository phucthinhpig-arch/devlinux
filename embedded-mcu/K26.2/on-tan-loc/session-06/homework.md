# Assignment — Session: 06
**Deadline: 2026-08-30 23:59:00**

---

## Exercise_1 — Driving the ST7796 Display Over SPI [review-only]

### Problem Statement

A display is the most satisfying peripheral to get working and one of the best ways to learn a serial bus, because nothing appears on screen until every part of the transfer is right. This exercise is about becoming fluent with ESP-IDF's `spi_master` driver — you are not expected to write a graphics library.

Bring up the 3.5" IPS module (ST7796U controller, 480×320 landscape, RGB565) on `SPI2_HOST` and draw with it:

1. Reset and initialise the controller, set it to **landscape** orientation, and turn the display on.
2. Fill the **entire screen** with a solid colour, cycling red → green → blue → white → black, one second per colour, forever.
3. Before entering that loop, draw three horizontal bars stacked top to bottom — red, green, blue — each one third of the screen height, and hold them for three seconds so you can photograph the result.

Requirements:
- Use `driver/spi_master.h`: `spi_bus_initialize()`, `spi_bus_add_device()`, and `spi_device_polling_transmit()`. Do **not** use the `esp_lcd` panel API for this exercise. `esp_lcd` wraps the whole SPI layer away in one call, and the SPI layer is what you are here to learn. You will be given `esp_lcd` later, once you know what it is doing for you.
- The `LCD_RS` pin selects command versus data. Write two clearly separated helpers — one that sends a command byte, one that sends a data buffer — and drive `LCD_RS` inside them. Every other function calls those two and never touches the pin directly.
- Implement a window-setting helper using `CASET` / `RASET` / `RAMWR` before writing pixels, and one fill helper `lcd_fill_rect(x, y, w, h, colour)`. The full-screen fill is that helper called with the full screen — do not write two separate fill paths.
- Set orientation through the `MADCTL` register so the panel is **landscape, 480 wide by 320 tall**. The later LVGL work uses this same orientation, so getting it right now saves you redoing it.
- Colour depth is 16-bit RGB565 (`COLMOD` = `0x55`). Each pixel is two bytes, high byte first.
- Do not send one pixel per SPI transaction. Fill a buffer with many pixels and send it in chunks — a full screen is 480 × 320 × 2 = 307 200 bytes, and one transaction per pixel will be visibly, painfully slow. Pick a chunk size, make it a named constant, and be ready to justify it.
- `max_transfer_sz` in the bus config must be at least your chunk size. A chunk larger than that fails at runtime, not at compile time.
- Check the return value of every `spi_*` and `gpio_*` call. `ESP_ERROR_CHECK()` is fine throughout this exercise — a display that will not initialise leaves nothing to run.
- All command bytes, pin numbers and dimensions must be named constants.

### Hardware

LCD Wiki 3.5" IPS SPI module, **MSP3525** (no touch) or **MSP3526** (with touch) — either works here. Keep the module wired for Session 07.

> **Power:** the module's operating voltage is **5 V** — connect `VCC` to the DevKitC-1's `5V` pin, not `3V3`. Signal lines go directly to the ESP32-S3's 3.3 V GPIOs. Check the labels silk-screened on your own board against the table below before powering up; if anything differs, trust your board and the [module wiki page](https://www.lcdwiki.com/3.5inch_IPS_SPI_Module_ST7796).

| Module pin | ESP32-S3 | Role |
| --- | --- | --- |
| `VCC` | 5V | power |
| `GND` | GND | ground |
| `SCK` | GPIO12 | SPI clock |
| `SDI(MOSI)` | GPIO11 | SPI data, host → display |
| `SDO(MISO)` | GPIO13 | SPI data, display → host (unused here, wire it anyway) |
| `LCD_CS` | GPIO10 | chip select, active low |
| `LCD_RS` | GPIO9 | command (low) / data (high) |
| `LCD_RST` | GPIO14 | hardware reset, active low |
| `LED` | GPIO2 | backlight — drive high to switch on |
| `SD_CS` | — | leave unconnected |

**Wire this pinout exactly as given.** It is the one used for the rest of the course — Session 07 adds the touch pins on top of it, and the LVGL work later reuses both. Choosing your own pins now means rewiring twice.

The touch pins (`CTP_SCL`, `CTP_SDA`, `CTP_RST`, `CTP_INT`) stay unconnected in this exercise.

### Design Hints

```c
#include "driver/gpio.h"
#include "driver/spi_master.h"

#define LCD_HOST      SPI2_HOST
#define PIN_SCK       GPIO_NUM_12
#define PIN_MOSI      GPIO_NUM_11
#define PIN_MISO      GPIO_NUM_13
#define PIN_CS        GPIO_NUM_10
#define PIN_RS        GPIO_NUM_9
#define PIN_RST       GPIO_NUM_14
#define PIN_BK_LIGHT  GPIO_NUM_2

#define LCD_H_RES     (480U) /* landscape: width  */
#define LCD_V_RES     (320U) /* landscape: height */
#define LCD_CLK_HZ    (20 * 1000 * 1000) /* start here; raise it once it works */

#define CMD_SWRESET   (0x01U)
#define CMD_SLPOUT    (0x11U)
#define CMD_INVON     (0x21U)
#define CMD_DISPON    (0x29U)
#define CMD_CASET     (0x2AU)
#define CMD_RASET     (0x2BU)
#define CMD_RAMWR     (0x2CU)
#define CMD_MADCTL    (0x36U)
#define CMD_COLMOD    (0x3AU)

/* MADCTL bits — combine these to get the orientation you want */
#define MADCTL_MY     (0x80U) /* row address order    */
#define MADCTL_MX     (0x40U) /* column address order */
#define MADCTL_MV     (0x20U) /* row/column exchange -> this is what makes it landscape */
#define MADCTL_BGR    (0x08U) /* colour order: set = BGR, clear = RGB */

#define COLOUR_RED    (0xF800U)
#define COLOUR_GREEN  (0x07E0U)
#define COLOUR_BLUE   (0x001FU)

spi_bus_config_t buscfg = {
    .sclk_io_num     = PIN_SCK,
    .mosi_io_num     = PIN_MOSI,
    .miso_io_num     = PIN_MISO,
    .quadwp_io_num   = -1,
    .quadhd_io_num   = -1,
    .max_transfer_sz = CHUNK_BYTES, /* must be >= your chunk size */
};

spi_device_interface_config_t devcfg = {
    .clock_speed_hz = LCD_CLK_HZ,
    .mode           = 0,
    .spics_io_num   = PIN_CS, /* the driver toggles CS around each transaction for you */
    .queue_size     = 7,
};
```

The minimum bring-up sequence is: pull `LCD_RST` low then high with a delay, `SWRESET`, wait, `SLPOUT`, wait, write one `MADCTL` byte, write `COLMOD` = `0x55`, then `DISPON`. Several of those waits are mandatory and the panel stays blank if you skip them — the required durations are in the ST7796U datasheet.

For the full vendor initialisation block and the register values it writes, work from the ST7796U datasheet and the demo code linked on the module's wiki page. Copying the vendor's magic register sequence is acceptable — but put a comment above it saying where it came from, and do not present it as something you derived.

### Suggested Approach

```
1. Configure PIN_RS, PIN_RST, PIN_BK_LIGHT as plain GPIO outputs. Backlight on.
2. spi_bus_initialize() -> spi_bus_add_device()
3. lcd_write_cmd(uint8_t cmd)                 : RS low,  transmit 1 byte
   lcd_write_data(const uint8_t* buf, size_t) : RS high, transmit buf
4. lcd_init(): hardware reset, then the command sequence above,
   including one MADCTL byte built from the bit constants
5. lcd_set_window(x0, y0, x1, y1): CASET with the X range, RASET with the Y
   range, then CMD_RAMWR. Each coordinate goes out as two bytes, high byte first.
6. lcd_fill_rect(x, y, w, h, colour):
     set the window, fill a buffer with the colour, send w*h pixels in chunks
7. app_main(): init -> three bars -> 3 s delay -> forever: cycle five colours
```

### Expected Output

The backlight comes on, three crisp colour bars appear — red on top, green in the middle, blue at the bottom, each spanning the full 480-pixel width — and after three seconds the screen begins cycling five solid colours, one per second, indefinitely.

Three failures are common enough on this exact panel to name in advance, because all three look like a broken program when each is really a one-line fix:

- **Backlight on, screen stays black.** The command sequence is not landing. Check that `LCD_RS` is low for command bytes and high for data bytes, and that you did not skip the mandatory delays after `SWRESET` and `SLPOUT`.
- **Colours are inverted** — black where you wrote white, and vice versa. This IPS panel needs display inversion switched on. One of the constants above is that command; work out where in your sequence it belongs.
- **Red and blue are swapped, green is fine.** That is the colour-order bit in `MADCTL`. Decide whether to clear the bit or to swap your colour constants, and say in a comment which you chose and why.

### Submission

```
Exercise_1/
├── main/
│   ├── main.c              (required — spi_master driver, no esp_lcd panel API)
│   └── CMakeLists.txt      (required)
└── CMakeLists.txt          (required — ESP-IDF project root)
```

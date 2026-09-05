# Assignment — Session: 18
**Deadline: 2026-08-16 23:59:00**

---

## Exercise_1 — I2C OLED Display Driver (SSD1306) with App-Driven Bitmap Upload [build]

### Problem Statement

The **SSD1306** is a popular I2C-based OLED display controller (128×64 pixels) widely used in embedded Linux projects like Raspberry Pi, Arduino, and IoT devices. Unlike simple GPIO-controlled displays, the SSD1306 requires **I2C protocol communication** and manages a **page-based framebuffer** in the kernel.

Write a **complete I2C character device driver** (`ssd1306_i2c_driver.c`) and **user-space application** (`ssd1306_app.c`) that together:

### Kernel Driver Requirements

1. **Registers a character device** at `/dev/ssd1306_i2c` (auto-assign major number).

2. **Initializes the SSD1306 display** via I2C (address `0x3C`):
   - Configure display mode, contrast, clock divider (standard SSD1306 init sequence)
   - Clear the display (all pixels off)
   - Log initialization via `pr_info()` (visible in dmesg)

3. **Implements file operations**:
   - **`open()`**: Verify I2C device exists, allow multiple readers, log PID
   - **`write()`**: Accept text commands:
     - `TEXT <string>` - render text on display using 5×7 font (max 21 chars per line)
     - `CLEAR` - clear display buffer
     - `ON` - turn display on
     - `OFF` - turn display off
     - Example: `echo "TEXT Hello World" > /dev/ssd1306_i2c`
   - **`read()`**: Return display status (e.g., `"Status: ON, Last text: Hello World\n"`)
   - **`release()`**: Cleanup on close

4. **Manages display memory**:
   - Page-based framebuffer (8 pages × 128 columns = 1024 bytes)
   - Each page = 8 vertical pixels
   - Maintain kernel-space buffer, sync to hardware via I2C

5. **Implements text rendering**:
   - Built-in 5×7 pixel font (provide in header file)
   - Support A-Z, a-z, 0-9, and common symbols (-, ., !, ?, space)
   - Multi-line text wrapping (21 chars per line, 8 lines max)
   - Render onto existing bitmap (text overlay)

6. **I2C Communication**:
   - Use I2C user-space API (`/dev/i2c-1` on Raspberry Pi)
   - Command format: send 0x00 prefix + command byte
   - Data format: send 0x40 prefix + data bytes (up to 32 bytes per write)
   - Standard 100kHz I2C speed

7. **Error Handling**:
   - Return `-ENODEV` if I2C device not found
   - Return `-EIO` on I2C communication errors
   - Return `-EINVAL` for invalid bitmap size or commands
   - Log all errors via `pr_err()`

8. **Kernel Logging** (pr_info/pr_err to dmesg):
   ```
   [ssd1306] I2C device initialized at address 0x3C
   [ssd1306] Device opened by PID XXXX
   [ssd1306] Bitmap uploaded: 1024 bytes
   [ssd1306] Text rendered: "Hello"
   [ssd1306] Display command: ON
   [ssd1306] Device closed by PID XXXX
   ```

### User-Space Application Requirements

Write a **test and demo application** (`ssd1306_app.c`) that:

1. Opens `/dev/ssd1306_i2c`
2. Performs sequence of operations:
   - Write command to clear display: `"CLEAR"`
   - Write text commands to render text:
     - `"TEXT Hello World"`
     - `"TEXT Raspberry Pi"`
     - `"TEXT SSD1306 Test"`
   - Write command to turn display on: `"ON"`
   - Sleep 3 seconds (to see on hardware)
   - Write command to turn display off: `"OFF"`
   - Read status from device
   - Close device
3. Print status messages to stdout:
   ```
   [+] Opened /dev/ssd1306_i2c
   [+] Cleared display
   [+] Rendered text: "Hello World"
   [+] Rendered text: "Raspberry Pi"
   [+] Rendered text: "SSD1306 Test"
   [+] Display turned ON (watch your OLED!)
   [+] Waiting 3 seconds...
   [+] Display turned OFF
   [+] Status: ON, Last text: SSD1306 Test
   [+] Application completed successfully
   ```

### Design Hints

```c
/* ssd1306.h */
#define SSD1306_I2C_ADDR    0x3C
#define SSD1306_WIDTH       128
#define SSD1306_HEIGHT      64
#define SSD1306_PAGES       8

/* Display dimensions */
#define FONT_WIDTH          5
#define FONT_HEIGHT         7
#define CHARS_PER_LINE      (SSD1306_WIDTH / FONT_WIDTH)  /* ~25 chars */
#define LINES_PER_DISPLAY   (SSD1306_HEIGHT / FONT_HEIGHT) /* ~9 lines */

/* Command strings for write() */
#define CMD_CLEAR   "CLEAR"
#define CMD_TEXT    "TEXT"
#define CMD_ON      "ON"
#define CMD_OFF     "OFF"

/* Font: 5×7 bitmap (66 characters) */
static const unsigned char font5x7[66][7] = {
    /* ' ' (space) */   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    /* '0' */           {0x0e, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0e},
    /* '1' */           {0x04, 0x0c, 0x04, 0x04, 0x04, 0x04, 0x0e},
    /* ... more characters ... */
};
```

### Suggested Approach

```
[Kernel Driver Implementation]
1. Define I2C bus and device address constants
2. Allocate framebuffer (1024 bytes for 8 pages)
3. Implement I2C write helper: send command/data to SSD1306
4. In module_init():
   a. Register character device (alloc_chrdev_region, cdev_add, class_create, device_create)
   b. Find and open I2C adapter (/dev/i2c-1)
   c. Initialize SSD1306 (standard init sequence: display off, set clock, contrast, etc.)
   d. Clear framebuffer
5. In file_operations:
   a. open(): increment reference count, log PID
   b. write(): parse command (TEXT, CLEAR, ON, OFF)
      - For TEXT: copy_from_user, parse text string, render using font5x7 onto framebuffer
      - For CLEAR: clear all framebuffer bytes to 0x00
      - For ON: send display ON command to hardware via I2C
      - For OFF: send display OFF command to hardware via I2C
   c. read(): format and return status string (e.g., "Status: ON, Pages: 8")
   d. release(): decrement reference count, log close
6. Helper functions:
   - ssd1306_i2c_write(): send command/data to I2C device
   - ssd1306_render_char(): draw single character (5×7) on framebuffer
   - ssd1306_render_text(): draw multi-character text with wrapping
   - ssd1306_display_update(): sync framebuffer to hardware
7. In module_exit(): close I2C device, cleanup cdev/class/device

[User-Space Application]
1. open("/dev/ssd1306_i2c", O_RDWR)
2. write(fd, "CLEAR", 5) to clear display
3. write(fd, "TEXT Hello World", ...) to render first text
4. write(fd, "TEXT Raspberry Pi", ...) to render second text
5. write(fd, "TEXT SSD1306 Test", ...) to render third text
6. write(fd, "ON", 2) to turn display on
7. sleep(3) to let user see the display
8. write(fd, "OFF", 3) to turn display off
9. read(fd, status_buf, 100) to get status
10. close(fd)
11. Print status and results to stdout
```

### Expected Output

```
# Build driver (Yocto environment):
$ cd driver && make
[*] Compiling ssd1306_i2c_driver.c...
[+] Module built: ssd1306_i2c_driver.ko

# Build app:
$ cd ../app && make
[*] Cross-compiling ssd1306_app.c...
[+] Binary ready: ssd1306_app

# On Raspberry Pi (hardware):
$ sudo insmod driver/ssd1306_i2c_driver.ko
$ dmesg | tail -5
[ssd1306] I2C device initialized at address 0x3C
[ssd1306] Display cleared
[ssd1306] Character device created: /dev/ssd1306_i2c

$ ./app/ssd1306_app
[+] Device opened: /dev/ssd1306_i2c
[+] Display info: 128×64, 8 pages
[+] Bitmap uploaded (1024 bytes)
[+] Text rendered: "Hello Raspberry Pi"
[+] Display turned ON (watch your OLED!)
[+] Waiting 3 seconds...
[+] Display turned OFF
[+] Application completed

$ dmesg | tail -10
[ssd1306] Device opened by PID 1234
[ssd1306] Bitmap uploaded: 1024 bytes
[ssd1306] Text rendered at (0,0): "Hello"
[ssd1306] Display turned ON
[ssd1306] Display turned OFF
[ssd1306] Device closed by PID 1234

# Cleanup:
$ sudo rmmod ssd1306_i2c_driver
[ssd1306] I2C device closed, module unloaded
```

### Submission

```
Exercise_1/
├── driver/
│   ├── ssd1306_i2c_driver.c    (kernel module)
│   ├── ssd1306.h               (header with ioctl defs, font)
│   └── Makefile                (kernel module build)
├── app/
│   ├── ssd1306_app.c           (user-space test app)
│   └── Makefile                (cross-compile build)
├── README.md                   (build/run instructions)
└── BITMAP_UPLOAD_GUIDE.md      (how to load bitmaps)
```

---

## Summary

**Session 18** covers **I2C device drivers**, one of the most common peripheral interfaces in embedded Linux:

- **I2C protocol basics**: Master/slave communication, addressing, speed
- **Character device drivers**: Extended to hardware interface layer
- **Framebuffer management**: Page-based display memory model
- **Font rendering**: Bitmap-based character drawing
- **Hardware integration**: Real-world Raspberry Pi + SSD1306 OLED

Master this exercise and you can write drivers for **any I2C device**: temperature sensors, pressure sensors, accelerometers, GPIO expanders, real-time clocks (RTC), EEPROM, multiplexers, etc.

**Hardware Context:**
- Target: Raspberry Pi Zero W or Pi 3B+
- Display: SSD1306 128×64 OLED (≈$5)
- I2C pins: GPIO2 (SDA), GPIO3 (SCL)
- Connector: Standard 4-pin header
- Yocto/BitBake cross-compilation environment

This is a **production-grade exercise** based on real projects deployed in IoT/robotics systems.

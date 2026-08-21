# Assignment — Session: 03
**Deadline: 2026-08-23 23:59:00**

---

## Exercise_1 — Button-Driven 7-Segment Counter, Register Level [review-only]

### Problem Statement

Every driver call you will ever make — `gpio_set_level()`, `uart_write_bytes()`, `i2c_master_transmit()` — ends as a write into a memory-mapped register. Before trusting the abstraction, write the registers yourself once.

Build a single-digit counter on a 7-segment display, driven by one push-button. The digit shows `0`–`9` and wraps in both directions (`9` → `0`, `0` → `9`). One button carries three different gestures:

| Gesture | Effect on the counter |
| --- | --- |
| Single click | `+1` |
| Double click | `-1` |
| Long press | `+1` every 500 ms, repeating for as long as the button is held |

Requirements:
- **Register access only.** No `driver/gpio.h`, no `gpio_config()`, `gpio_set_level()` or `gpio_get_level()` anywhere in this exercise. Configure the pads through `IO_MUX`, route them through the GPIO matrix, and drive them through the `GPIO_OUT_*` / `GPIO_ENABLE_*` / `GPIO_IN_REG` registers.
- **Polling only.** Read the button state in a loop. GPIO interrupts are Session 04's topic and are not accepted here.
- **Choose your own display type.** Common cathode or common anode — wire whichever part you have. State which one you used in a comment at the top of `main.c`, and make the code correct for it. Between the two, the difference in your code should be a single, clearly-named inversion, not two copies of the digit table.
- The button uses the pad's **internal pull-up**, wired between the pin and GND, so pressed reads as `0`. Do not add an external pull-up resistor.
- Debounce the button. A mechanical switch bounces for a few milliseconds on every press and release; without debouncing, one physical click will register as several.
- Every register address, bit position, GPIO number and timing threshold must be a named constant.
- Every register access goes through a `volatile` pointer.

### Hardware

ESP32-S3 DevKitC-1, one single-digit 7-segment display, eight 220 Ω–330 Ω resistors (one per segment — never a single shared resistor on the common pin), one push-button, breadboard and jumper wires.

| Signal | GPIO | | Signal | GPIO |
| --- | --- | --- | --- | --- |
| segment `a` | 4 | | segment `f` | 16 |
| segment `b` | 5 | | segment `g` | 17 |
| segment `c` | 6 | | segment `dp` | 18 (optional) |
| segment `d` | 7 | | button | 14 (to GND) |
| segment `e` | 15 | | common pin | GND if common cathode, 3V3 if common anode |

All of these pins are below 32, so this exercise only ever touches the first bank of GPIO registers.

### Design Hints

```c
#define SEG_A_PIN (4U)
/* ... one per segment ... */
#define BTN_PIN   (14U)

#define DEBOUNCE_MS       (25U)
#define DOUBLE_CLICK_MS   (350U)  /* max gap between two clicks to count as a double */
#define LONG_PRESS_MS     (800U)  /* hold time before auto-repeat starts */
#define REPEAT_PERIOD_MS  (500U)  /* required by the spec — do not change this one */

/* Bit b0..b6 = segments a..g. This table is for a COMMON CATHODE display. */
static const uint8_t SEGMENT_MAP[10] = {
    0x3FU, 0x06U, 0x5BU, 0x4FU, 0x66U, /* 0 1 2 3 4 */
    0x6DU, 0x7DU, 0x07U, 0x7FU, 0x6FU, /* 5 6 7 8 9 */
};
```

The registers you need are all in the TRM chapter **"IO MUX and GPIO Matrix"**. Look up, for each pad you use: how to select the plain-GPIO function in `IO_MUX_GPIOn_REG`, how to enable the input buffer and the internal pull-up for the button, how the GPIO matrix routes an output signal to a pad, and which register enables the output driver. All four steps matter — a pad that is enabled as an output but still routed to its default peripheral function will simply stay dark.

Since all seven segments live in one 32-bit register, you can update the whole digit with a single write of a pre-built mask rather than seven separate ones. Work out how, and what `GPIO_OUT_W1TS_REG` / `GPIO_OUT_W1TC_REG` give you that a plain `GPIO_OUT_REG` read-modify-write does not.

### Suggested Approach

The hardware side is mostly mechanical. The gesture decoding is the part worth thinking about, so here is the shape of the problem rather than the solution:

```
Poll the debounced button state on a short, fixed period. From that you get two
events: press (1 -> 0) and release (0 -> 1). Track when each happened.

- A press that is still held after LONG_PRESS_MS starts auto-repeat: increment,
  then increment again every REPEAT_PERIOD_MS until release. A long press must
  NOT also register as a click when it finally ends.
- A release before LONG_PRESS_MS is a click. But you cannot act on it yet: a
  second click may still arrive within DOUBLE_CLICK_MS and turn it into a
  decrement. Work out where that pending click lives and what makes you finally
  commit to it.
```

That last point is the real design question in this exercise. A single click is necessarily reported late — think about what "late" costs here, and be able to explain your choice if asked.

### Expected Output

The display shows `0` after reset. Click once: `1`. Click again: `2`. Double-click: back to `1`. Hold the button: the digit climbs one step every half second — `2`, `3`, `4`, … — wrapping past `9` to `0`, and stops the moment you let go. Releasing after a long press must not add one extra count.

No serial output is required. The display is the output. Logging your decoded gestures with `ESP_LOGI` while developing is a good idea, and harmless to leave in.

### Submission

```
Exercise_1/
├── main/
│   ├── main.c              (required — register access only; display type in a header comment)
│   └── CMakeLists.txt      (required)
└── CMakeLists.txt          (required — ESP-IDF project root)
```

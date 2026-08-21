# Assignment — Session: 10
**Deadline: 2026-09-13 23:59:00**

---

## Exercise_1 — LED Dimming With LEDC, Knob-Controlled and Self-Fading [review-only]

### Problem Statement

A GPIO can only be on or off, yet an LED can clearly be *half* bright. The trick is switching fast enough that the eye integrates the result, and Session 09 already told you why you cannot just use a DAC on this chip — the S3 does not have one. PWM through the `ledc` peripheral is how you get an analog-looking output here.

Two parts, one program:

1. **Knob-controlled brightness.** Drive an LED with `ledc` and set its duty cycle from the potentiometer you read in Session 09. Turning the knob dims the LED smoothly across its full range.
2. **Hardware fade.** On a button press, hand the LED over to `ledc`'s built-in fade engine and let it "breathe" — fade up to full over 2 seconds, fade back down over 2 seconds, repeatedly — without your code computing a single intermediate duty value. Press again to return to knob control.

Requirements:
- Use `driver/ledc.h`: `ledc_timer_config()`, `ledc_channel_config()`, `ledc_set_duty()`, `ledc_update_duty()`.
- On the ESP32-S3 there is only `LEDC_LOW_SPEED_MODE`. There is no high-speed mode on this chip, so do not copy a configuration that asks for one.
- `ledc_set_duty()` alone does nothing visible. Find out what `ledc_update_duty()` is for and why the API is split in two.
- Pick a PWM frequency high enough that the LED does not visibly flicker, and a duty resolution that gives you fine control. State both as named constants, and be ready to explain the relationship between them — they are not independent, and asking for an impossible combination makes `ledc_timer_config()` fail.
- For part 2, use the fade API: `ledc_fade_func_install()` once at startup, then `ledc_set_fade_with_time()` and `ledc_fade_start()`. Your code must not implement fading with a loop of `ledc_set_duty()` calls and delays — the point is that the peripheral does it for you while the CPU sleeps.
- The button switches between the two modes. Reuse the interrupt-driven, debounced button approach from Session 04 rather than polling.
- Reuse your ADC code from Session 09, copied into this project.
- Map the raw ADC value to the duty range correctly. The ADC's full-scale count and the duty resolution's maximum are different numbers — do not assume they match.

### Hardware

Keep the display module and the potentiometer wired as they are. This exercise does not draw to the display, but leave it connected. Add an LED and a button:

| Part | ESP32-S3 | Notes |
| --- | --- | --- |
| LED anode, via 220 Ω–330 Ω resistor | GPIO15 | cathode to GND |
| push-button | GPIO16 | other side to GND, internal pull-up |
| potentiometer wiper | GPIO1 | unchanged from Session 09 |

Use a plain single-colour LED, not the onboard RGB LED — that one is a WS2812 driven over RMT and cannot be dimmed with `ledc`.

### Design Hints

```c
#include "driver/ledc.h"

#define LED_PIN          GPIO_NUM_15
#define BTN_PIN          GPIO_NUM_16

#define LEDC_MODE        LEDC_LOW_SPEED_MODE   /* the only mode on ESP32-S3 */
#define LEDC_TIMER       LEDC_TIMER_0
#define LEDC_CHANNEL     LEDC_CHANNEL_0
#define LEDC_DUTY_RES    LEDC_TIMER_13_BIT     /* -> duty range 0 .. (2^13 - 1) */
#define LEDC_FREQ_HZ     (5000U)
#define LEDC_DUTY_MAX    ((1U << 13) - 1U)

#define FADE_TIME_MS     (2000)

ledc_timer_config_t timer_cfg = {
    .speed_mode      = LEDC_MODE,
    .timer_num       = LEDC_TIMER,
    .duty_resolution = LEDC_DUTY_RES,
    .freq_hz         = LEDC_FREQ_HZ,
    .clk_cfg         = LEDC_AUTO_CLK,
};
ESP_ERROR_CHECK(ledc_timer_config(&timer_cfg));

ledc_channel_config_t channel_cfg = {
    .gpio_num   = LED_PIN,
    .speed_mode = LEDC_MODE,
    .channel    = LEDC_CHANNEL,
    .timer_sel  = LEDC_TIMER,
    .duty       = 0,
    .hpoint     = 0,
};
ESP_ERROR_CHECK(ledc_channel_config(&channel_cfg));

/* part 2, once at startup: */
ESP_ERROR_CHECK(ledc_fade_func_install(0));
```

`ledc_set_fade_with_time()` followed by `ledc_fade_start()` takes a target duty and a duration. Look at the `ledc_fade_mode_t` argument to `ledc_fade_start()`: one value returns immediately and lets the fade run in the background, the other blocks until the fade completes. Which one you want depends on how you structure the breathing loop, so read both before choosing.

### Suggested Approach

```
1. ledc_timer_config -> ledc_channel_config -> ledc_fade_func_install
2. Init ADC1 on the potentiometer (Session 09 code)
3. Configure BTN_PIN as a debounced, interrupt-driven input (Session 04 approach).
   The ISR only records the press; the mode switch happens in the task.
4. forever:
     if mode == KNOB:
         read + average the ADC, map to 0..LEDC_DUTY_MAX
         ledc_set_duty() + ledc_update_duty()
         short delay
     if mode == BREATHE:
         set_fade_with_time(LEDC_DUTY_MAX, FADE_TIME_MS) + fade_start
         wait for it to finish
         set_fade_with_time(0, FADE_TIME_MS) + fade_start
         wait for it to finish
     a button press flips the mode; make sure a press during a fade is not lost
```

### Common Pitfalls to Explain in Your Submission

Two short answers in `pwm_notes.md`.

1. Turn the knob slowly from one end to the other and watch the LED. The duty cycle changes linearly, but the brightness almost certainly does not *look* linear — most of the visible change is crowded into one part of the knob's travel. Say which part, and explain why. You are not required to correct it, only to describe what you see and why it happens.
2. You picked a PWM frequency and a duty resolution. Explain what limits the combination — what does the peripheral have to do at 13-bit resolution and 5 kHz, and what happens if you ask for 13 bits at 5 MHz? Try it and report what `ledc_timer_config()` says.

### Expected Output

In knob mode, the LED tracks the potentiometer smoothly from fully off to fully on, with no visible flicker or stepping at either end. A press of the button switches it into a smooth 4-second breathing cycle that continues on its own; a second press hands control back to the knob, picking up at whatever position the knob is currently in.

```
I (1150) PWM: mode=KNOB  raw=2043  duty=4086
I (1350) PWM: mode=KNOB  raw=3901  duty=7802
I (2402) PWM: mode=BREATHE  fade up   -> 8191 over 2000 ms
I (4405) PWM: mode=BREATHE  fade down -> 0 over 2000 ms
I (7810) PWM: mode=KNOB  raw=3899  duty=7798
```

During the breathing fades the CPU should have nothing to do — if you find yourself writing a loop that computes intermediate brightness values, re-read the requirement about the fade API.

### Submission

```
Exercise_1/
├── main/
│   ├── main.c              (required — ledc PWM + fade API, ADC from Session 09)
│   └── CMakeLists.txt      (required)
├── CMakeLists.txt          (required — ESP-IDF project root)
└── pwm_notes.md            (required — answers to the 2 questions above)
```

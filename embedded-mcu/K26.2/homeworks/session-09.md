# Assignment — Session: 09
**Deadline: 2026-09-13 23:59:00**

---

## Exercise_1 — Reading a Potentiometer With the ADC and Plotting It [review-only]

### Problem Statement

Every peripheral so far has been digital — a pin is high or it is low, a byte is `0x2C` or it is not. The ADC is where the outside world stops being tidy: the same knob position gives you a slightly different number every time you read it, and the number you get is not the voltage until you have calibrated it.

Read a potentiometer and display the result:

1. Configure ADC1 in one-shot mode on the potentiometer's channel and read the raw value.
2. Convert that raw value to **millivolts** using the ADC calibration API. Do not invent your own scaling formula.
3. Average several consecutive samples per reading so the displayed value does not jitter.
4. Every 200 ms, log the raw value and the millivolts, and draw a **horizontal bar** on the ST7796 whose length is proportional to the measured voltage — a bar graph that follows the knob.

Requirements:
- Use the current `esp_adc/adc_oneshot.h` API: `adc_oneshot_new_unit()`, `adc_oneshot_config_channel()`, `adc_oneshot_read()`. Do **not** use the deprecated `driver/adc.h` — almost every tutorial online still does, so check the include before copying anything.
- Use `esp_adc/adc_cali.h` and `esp_adc/adc_cali_scheme.h` to convert raw counts to millivolts. If calibration is unavailable on your chip, log a warning and fall back, but do not silently substitute a hand-rolled formula.
- Use **ADC1**. ADC2 shares hardware with the Wi-Fi radio and reads will fail unpredictably once Wi-Fi is in use later in the course.
- Pick an attenuation that lets you read the full 0 V to 3.3 V swing of the potentiometer, and state in a comment what input range your chosen attenuation actually covers.
- Averaging: take at least 16 samples per displayed reading. Make the count a named constant.
- Redraw only what changed. Do not clear and repaint the whole 480×320 screen every 200 ms — work out how to erase just the part of the bar that shrank.
- Reuse your display code from Session 06, copied into this project, at the same 480×320 landscape orientation.
- Every pin, channel, attenuation, period and sample count must be a named constant.

### Hardware

Keep the display module wired exactly as in Sessions 06 and 07, and add a potentiometer:

| Part | ESP32-S3 | Notes |
| --- | --- | --- |
| potentiometer, outer pin 1 | 3V3 | |
| potentiometer, outer pin 2 | GND | |
| potentiometer, wiper (middle) | GPIO1 | `ADC1_CHANNEL_0` |

Any linear potentiometer from 1 kΩ to 100 kΩ works. Wire the wiper to GPIO1 — **not** to any pin the display is using, and not to GPIO3, which is a strapping pin.

> **A note on the "DAC" half of this session's title:** the ESP32-S3 has **no DAC peripheral**. The original ESP32 and the S2 have one; it was removed on the S3. So there is no DAC exercise to give you, and any tutorial that calls `dac_output_voltage()` on an S3 will not compile. The way you produce an analog-looking output on this chip is PWM through a low-pass filter, which is Session 10.

### Design Hints

```c
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"

#define ADC_UNIT_USED    ADC_UNIT_1
#define ADC_CHANNEL_POT  ADC_CHANNEL_0     /* GPIO1 on ESP32-S3 */
#define ADC_ATTEN_USED   ADC_ATTEN_DB_12   /* widest input range on this chip */
#define ADC_BITWIDTH     ADC_BITWIDTH_DEFAULT

#define SAMPLE_COUNT     (16U)
#define UPDATE_PERIOD_MS (200U)
#define VOLTAGE_MAX_MV   (3300)            /* full-scale for the bar length */

#define BAR_X            (40U)
#define BAR_Y            (140U)
#define BAR_MAX_W        (400U)
#define BAR_H            (40U)

adc_oneshot_unit_init_cfg_t unit_cfg = {
    .unit_id = ADC_UNIT_USED,
};
adc_oneshot_unit_handle_t adc_handle;
ESP_ERROR_CHECK(adc_oneshot_new_unit(&unit_cfg, &adc_handle));

adc_oneshot_chan_cfg_t chan_cfg = {
    .atten    = ADC_ATTEN_USED,
    .bitwidth = ADC_BITWIDTH,
};
ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_POT, &chan_cfg));

/* Calibration: on ESP32-S3 the scheme is curve fitting. Create the handle once,
   at startup, then call adc_cali_raw_to_voltage() for every reading. */
adc_cali_handle_t cali_handle = NULL;
adc_cali_curve_fitting_config_t cali_cfg = {
    .unit_id  = ADC_UNIT_USED,
    .atten    = ADC_ATTEN_USED,
    .bitwidth = ADC_BITWIDTH,
};
```

`adc_oneshot_read()` returns an `esp_err_t` and writes the raw value through a pointer — it does not return the reading. Check the error before you use the value.

Two details worth finding out for yourself rather than guessing. First: what raw value does a full-scale input actually produce at your chosen attenuation and bit width? It is not necessarily 4095, and your bar-scaling arithmetic depends on it. Second: turn the knob slowly to one extreme and watch the raw value. It probably does not reach either theoretical end of the range. Note the real minimum and maximum you observe in a comment — that gap is normal ADC behaviour on this chip, not a broken potentiometer.

### Suggested Approach

```
1. Init the display (Session 06 code), clear to black, draw the bar's outline once
2. Init ADC1 unit + channel, then create the calibration handle
3. forever:
     a. read SAMPLE_COUNT raw samples, average them
     b. adc_cali_raw_to_voltage(avg) -> millivolts
     c. ESP_LOGI raw and mV
     d. new_w = millivolts * BAR_MAX_W / VOLTAGE_MAX_MV
        if new_w > old_w: fill the newly exposed strip with the bar colour
        if new_w < old_w: fill the vacated strip with the background colour
        remember old_w
     e. vTaskDelay(pdMS_TO_TICKS(UPDATE_PERIOD_MS))
```

Step (d) is the whole point of "redraw only what changed" — two small fills instead of one full-screen repaint. Get the arithmetic right and the bar will look smooth; get it wrong and you will see stripes left behind.

### Expected Output

A bar on screen that grows and shrinks as you turn the knob, tracking it without visible flicker or leftover fragments, and a log stream at 5 Hz:

```
I (1204) ADC: raw=   12  ->    9 mV
I (1404) ADC: raw= 1043  ->  856 mV
I (1604) ADC: raw= 2087  -> 1690 mV
I (1804) ADC: raw= 4051  -> 3241 mV
```

At one end of the knob the reading sits near zero; at the other it sits near 3300 mV. Neither end reaches its theoretical limit exactly, and that is the correct result — see the second question in the hints above.

### Submission

```
Exercise_1/
├── main/
│   ├── main.c              (required — adc_oneshot + adc_cali, no driver/adc.h)
│   └── CMakeLists.txt      (required)
└── CMakeLists.txt          (required — ESP-IDF project root)
```

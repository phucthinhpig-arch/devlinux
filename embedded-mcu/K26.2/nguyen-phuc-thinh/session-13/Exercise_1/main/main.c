#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "esp_sleep.h"
#include "esp_log.h"

#define BTN_PIN            GPIO_NUM_0
#define BTN_WAKE_LEVEL     (0)              /* pressed = LOW[cite: 57] */
#define LED_PIN            GPIO_NUM_15
#define BLINK_COUNT        (3U)
#define BLINK_HALF_MS      (150U)
#define HEARTBEAT_US       (60ULL * 1000000ULL) /* 60 seconds[cite: 57] */

static const char *TAG = "BELL";

/* RTC_DATA_ATTR keeps the variable in RTC memory during deep sleep. 
 * It initializes to 0 on a fresh power-on automatically, requiring no magic number[cite: 57]. */
RTC_DATA_ATTR static uint32_t press_count = 0;

void app_main(void) {
    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause(); /*[cite: 57] */

    /* 1. Branch on wakeup cause[cite: 57] */
    switch (cause) {
        case ESP_SLEEP_WAKEUP_EXT0: {
            gpio_config_t led_cfg = {
                .pin_bit_mask = (1ULL << LED_PIN),
                .mode = GPIO_MODE_OUTPUT,
            };
            gpio_config(&led_cfg);

            /* Blink the LED 3 times[cite: 57] */
            for (uint32_t i = 0; i < BLINK_COUNT; i++) {
                gpio_set_level(LED_PIN, 1);
                vTaskDelay(pdMS_TO_TICKS(BLINK_HALF_MS));
                gpio_set_level(LED_PIN, 0);
                vTaskDelay(pdMS_TO_TICKS(BLINK_HALF_MS));
            }
            press_count++; /* Only a button wake increments the counter[cite: 57] */
            ESP_LOGI(TAG, "ding-dong #%lu", press_count); /*[cite: 57] */
            break;
        }
        case ESP_SLEEP_WAKEUP_TIMER: {
            /* A heartbeat only reports the count[cite: 57] */
            ESP_LOGI(TAG, "heartbeat, total presses=%lu", press_count); /*[cite: 57] */
            break;
        }
        default: {
            /* Fresh power-on[cite: 57] */
            ESP_LOGI(TAG, "power-on, doorbell armed. presses=%lu", press_count); /*[cite: 57] */
            break;
        }
    }

    /* 2. Hand BTN_PIN back to the digital side before reading it[cite: 57] */
    rtc_gpio_deinit(BTN_PIN); /*[cite: 57] */
    gpio_config_t btn_cfg = {
        .pin_bit_mask = (1ULL << BTN_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
    };
    gpio_config(&btn_cfg);

    /* 3. Wait until the button has been released to prevent immediate re-waking[cite: 57] */
    while (gpio_get_level(BTN_PIN) == BTN_WAKE_LEVEL) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    /* 4. Arm the RTC pull-up so the pin cannot float during deep sleep[cite: 57] */
    rtc_gpio_pullup_en(BTN_PIN); /*[cite: 57] */

    /* 5. Configure wake sources and sleep[cite: 57] */
    esp_sleep_enable_ext0_wakeup(BTN_PIN, BTN_WAKE_LEVEL); /*[cite: 57] */
    esp_sleep_enable_timer_wakeup(HEARTBEAT_US); /*[cite: 57] */

    ESP_LOGI(TAG, "sleeping"); /*[cite: 57] */
    esp_deep_sleep_start(); /*[cite: 57] */
    
    /* Nothing below this line ever runs[cite: 57] */
}
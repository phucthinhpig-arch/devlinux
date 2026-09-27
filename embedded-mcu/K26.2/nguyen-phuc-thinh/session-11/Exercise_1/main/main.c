#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_sleep.h"
#include "esp_log.h"

static const char *TAG = "SLEEP";

/* --- CONSTANTS --- */
#define BTN_PIN             GPIO_NUM_16
#define PIN_BK_LIGHT        GPIO_NUM_2
#define LIGHT_SLEEP_US      (5000000ULL)
#define DEEP_SLEEP_US       (10000000ULL)
#define BTN_WAKE_LEVEL      0

/* --- COUNTERS --- */
RTC_DATA_ATTR static uint32_t rtc_boot_count = 0; /* Survives deep sleep[cite: 47, 49] */
static uint32_t ram_boot_count = 0;               /* Resets on reboot[cite: 47, 49] */

/* --- HELPER --- */
static const char* wakeup_cause_name(esp_sleep_wakeup_cause_t cause) {
    switch (cause) {
        case ESP_SLEEP_WAKEUP_TIMER: return "TIMER";
        case ESP_SLEEP_WAKEUP_GPIO:  return "EXT0 / GPIO";
        case ESP_SLEEP_WAKEUP_EXT1:  return "EXT1";
        default: return "POWER_ON / RESET";
    }
}

void app_main(void) {
    rtc_boot_count++;
    ram_boot_count++;

    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
    ESP_LOGI(TAG, "=== boot: cause=%s rtc_boots=%lu ram_boots=%lu ===", 
             wakeup_cause_name(cause), rtc_boot_count, ram_boot_count); /*[cite: 49] */

    /* GPIO Config */
    gpio_config_t btn_conf = {
        .pin_bit_mask = (1ULL << BTN_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&btn_conf));

    gpio_config_t bk_conf = {
        .pin_bit_mask = (1ULL << PIN_BK_LIGHT),
        .mode = GPIO_MODE_OUTPUT,
    };
    ESP_ERROR_CHECK(gpio_config(&bk_conf));

    /* --- LIGHT SLEEP DEMO --- */
    gpio_set_level(PIN_BK_LIGHT, 0); /* Backlight off before sleep[cite: 52] */
    
    ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(LIGHT_SLEEP_US));
    ESP_ERROR_CHECK(gpio_wakeup_enable(BTN_PIN, GPIO_INTR_LOW_LEVEL));
    ESP_ERROR_CHECK(esp_sleep_enable_gpio_wakeup());
    
    ESP_LOGI(TAG, "entering light sleep (5 s or button)");
    esp_light_sleep_start(); /* Resumes execution from the next line[cite: 49] */

    gpio_set_level(PIN_BK_LIGHT, 1); /* Backlight on after waking[cite: 52] */
    ESP_LOGI(TAG, "resumed from light sleep, cause=%s", wakeup_cause_name(esp_sleep_get_wakeup_cause())); /*[cite: 49] */

    /* Delay to observe the light sleep resume state before deep sleeping */
    vTaskDelay(pdMS_TO_TICKS(2000));

    /* --- DEEP SLEEP DEMO --- */
    gpio_set_level(PIN_BK_LIGHT, 0); /* Turn off display to save power[cite: 52] */
    
    ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(DEEP_SLEEP_US));
    ESP_ERROR_CHECK(esp_sleep_enable_ext1_wakeup(1ULL << BTN_PIN, ESP_EXT1_WAKEUP_ALL_LOW)); /*[cite: 49] */
    
    ESP_LOGI(TAG, "entering deep sleep - see you in app_main()");
    esp_deep_sleep_start(); /* Reboots the chip upon waking[cite: 49] */
    
    /* Code below this point will never execute */
}
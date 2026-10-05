#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_task_wdt.h"
#include "esp_log.h"

/* --- CÁC HẰNG SỐ CỐ ĐỊNH --- */
#define BTN_PIN            GPIO_NUM_0    /* Nút BOOT trên phần cứng */
#define SENSOR_PERIOD_MS   (200U)        /* */
#define REPORT_PERIOD_MS   (1000U)       /* */
#define TWDT_TIMEOUT_MS    (3000U)       /* Giới hạn timeout 3 giây */
#define RESET_MAGIC        (0xC0FFEE42U) /* Magic number bảo vệ vùng nhớ */

static const char *TAG = "WDT";

/* Biến lưu trữ sống sót qua các lần watchdog reset[cite: 53] */
RTC_NOINIT_ATTR static uint32_t wdt_magic;
RTC_NOINIT_ATTR static uint32_t wdt_reset_count;

/* Các biến điều khiển luồng */
static volatile bool hang_requested = false; /* Cờ báo hiệu yêu cầu treo task[cite: 53] */
static SemaphoreHandle_t hang_sem = NULL;

/* Hàm chuyển đổi mã reset thành tên dễ đọc[cite: 53] */
static const char* reset_reason_name(esp_reset_reason_t reason) {
    switch (reason) {
        case ESP_RST_POWERON:   return "POWERON";
        case ESP_RST_EXT:       return "EXT";
        case ESP_RST_SW:        return "SW";
        case ESP_RST_PANIC:     return "PANIC";
        case ESP_RST_INT_WDT:   return "INT_WDT";
        case ESP_RST_TASK_WDT:  return "TASK_WDT";
        case ESP_RST_WDT:       return "WDT";
        case ESP_RST_DEEPSLEEP: return "DEEPSLEEP";
        case ESP_RST_BROWNOUT:  return "BROWNOUT";
        case ESP_RST_SDIO:      return "SDIO";
        default:                return "OTHER";
    }
}

/* --- TASK 1: SENSOR --- */
static void sensor_task(void *pvParameters) {
    esp_task_wdt_add(NULL); /* Tự đăng ký vào TWDT[cite: 53] */
    
    while (1) {
        ESP_LOGI(TAG, "sensor tick");
        esp_task_wdt_reset(); /* Tự feed watchdog trong vòng lặp[cite: 53] */
        vTaskDelay(pdMS_TO_TICKS(SENSOR_PERIOD_MS));
    }
}

/* --- TASK 2: REPORT --- */
static void report_task(void *pvParameters) {
    esp_task_wdt_add(NULL); /* Tự đăng ký vào TWDT[cite: 53] */
    
    while (1) {
        if (hang_requested) {
            /* Block vĩnh viễn trên một semaphore không bao giờ được cấp quyền (give)[cite: 53] */
            xSemaphoreTake(hang_sem, portMAX_DELAY);
        }
        ESP_LOGI(TAG, "report: all good");
        esp_task_wdt_reset(); /* Tự feed watchdog trong vòng lặp[cite: 53] */
        vTaskDelay(pdMS_TO_TICKS(REPORT_PERIOD_MS));
    }
}

/* --- MAIN APPLICATION --- */
void app_main(void) {
    /* 1. Đọc và xử lý nguyên nhân reset[cite: 53] */
    esp_reset_reason_t reason = esp_reset_reason();
    
    /* Khởi tạo lại counter từ 0 sau mỗi lần ngắt nguồn vật lý hoặc sai magic number[cite: 53] */
    if (wdt_magic != RESET_MAGIC || reason == ESP_RST_POWERON) {
        wdt_reset_count = 0;
        wdt_magic = RESET_MAGIC;
    }
    
    /* Chỉ tăng bộ đếm khi nguyên nhân chính xác là do WDT reset[cite: 53] */
    if (reason == ESP_RST_TASK_WDT) {
        wdt_reset_count++;
    }
    
    ESP_LOGI(TAG, "boot: reason=%s  watchdog_resets=%lu", reset_reason_name(reason), wdt_reset_count);

    /* 2. Cấu hình Task Watchdog Timer */
    esp_task_wdt_config_t twdt_cfg = {
        .timeout_ms = TWDT_TIMEOUT_MS,
        .idle_core_mask = (1 << 0) | (1 << 1),
        .trigger_panic = true, /* Yêu cầu panic để khởi động lại mạch[cite: 53] */
    };
    
    /* Kiểm tra trạng thái TWDT từ framework và cấu hình[cite: 53] */
    esp_err_t err = esp_task_wdt_reconfigure(&twdt_cfg);
    if (err == ESP_ERR_INVALID_STATE) {
        esp_task_wdt_init(&twdt_cfg);
    }

    /* 3. Khởi tạo Semaphore và GPIO Nút nhấn */
    hang_sem = xSemaphoreCreateBinary();
    
    gpio_config_t btn_cfg = {
        .pin_bit_mask = (1ULL << BTN_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&btn_cfg);

    /* 4. Khởi tạo các task với tên định danh chuẩn xác[cite: 53] */
    xTaskCreate(sensor_task, "sensor", 2048, NULL, 5, NULL);
    xTaskCreate(report_task, "report", 2048, NULL, 5, NULL);

    /* 5. Vòng lặp chính xử lý nút nhấn */
    while (1) {
        if (gpio_get_level(BTN_PIN) == 0 && !hang_requested) {
            ESP_LOGI(TAG, "BOOT pressed -> report will hang");
            hang_requested = true; /* Đổi trạng thái cờ khi nhấn nút[cite: 53] */
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
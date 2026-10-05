#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_task_wdt.h"
#include "esp_log.h"

/* --- CÁC HẰNG SỐ CỐ ĐỊNH --- */
#define BTN_PIN            GPIO_NUM_0    /* Nút BOOT trên ESP32-S3 DevKitC-1[cite: 53] */
#define SENSOR_PERIOD_MS   (200U)        /* Chu kỳ chạy của task sensor[cite: 53] */
#define REPORT_PERIOD_MS   (1000U)       /* Chu kỳ chạy của task report[cite: 53] */
#define BUTTON_PERIOD_MS   (100U)        /* Chu kỳ lấy mẫu nút nhấn */
#define TWDT_TIMEOUT_MS    (3000U)       /* Timeout của watchdog timer là 3 giây[cite: 53] */
#define RESET_MAGIC        (0xC0FFEE42U) /* Magic number để phân biệt khởi động có chủ đích với giá trị ngẫu nhiên rác sau khi cấp nguồn (power-on)[cite: 53, 54] */

static const char *TAG = "WDT";

/* Biến lưu trữ số lần watchdog reset, sống sót qua quá trình reset bằng RTC_NOINIT_ATTR[cite: 53] */
RTC_NOINIT_ATTR static uint32_t wdt_magic;
RTC_NOINIT_ATTR static uint32_t wdt_reset_count;

static volatile bool hang_requested = false; /* Cờ báo hiệu yêu cầu treo task, bắt buộc dùng volatile[cite: 53] */
static SemaphoreHandle_t hang_sem = NULL;

/* Hàm chuyển đổi mã reset thành tên chuỗi ký tự dễ đọc[cite: 53] */
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
    ESP_ERROR_CHECK(esp_task_wdt_add(NULL)); /* Task tự đăng ký giám sát với TWDT[cite: 53] */
    
    while (1) {
        ESP_LOGI(TAG, "sensor tick");
        ESP_ERROR_CHECK(esp_task_wdt_reset()); /* Feed watchdog mỗi chu kỳ[cite: 53] */
        vTaskDelay(pdMS_TO_TICKS(SENSOR_PERIOD_MS));
    }
}

/* --- TASK 2: REPORT --- */
static void report_task(void *pvParameters) {
    ESP_ERROR_CHECK(esp_task_wdt_add(NULL)); /* Task tự đăng ký giám sát với TWDT[cite: 53] */
    
    while (1) {
        if (hang_requested) {
            if (hang_sem != NULL) { /* Null-check trước khi take semaphore để tránh crash hệ thống nếu cấp phát lỗi[cite: 54, 55] */
                xSemaphoreTake(hang_sem, portMAX_DELAY); /* Treo (hang) thực sự bằng cách block vĩnh viễn trên một semaphore không bao giờ được give[cite: 53] */
            } else {
                ESP_LOGE(TAG, "Semaphore creation failed"); /*[cite: 55] */
            }
        }
        ESP_LOGI(TAG, "report: all good");
        ESP_ERROR_CHECK(esp_task_wdt_reset()); /* Feed watchdog mỗi chu kỳ[cite: 53] */
        vTaskDelay(pdMS_TO_TICKS(REPORT_PERIOD_MS));
    }
}

/* --- TASK 3: BUTTON --- */
/* Tách riêng logic xử lý nút nhấn ra khỏi app_main để code rõ ràng và chia thành 3 task độc lập[cite: 54, 56] */
static void button_task(void *pvParameters) {
    while (1) {
        if (gpio_get_level(BTN_PIN) == 0 && !hang_requested) {
            ESP_LOGI(TAG, "BOOT pressed -> report will hang"); /*[cite: 53] */
            hang_requested = true; /* Chỉ set cờ, việc treo diễn ra bên trong report_task[cite: 53] */
        }
        vTaskDelay(pdMS_TO_TICKS(BUTTON_PERIOD_MS));
    }
}

/* --- MAIN APPLICATION --- */
void app_main(void) {
    /* 1. Xử lý nguyên nhân khởi động và đếm số lần watchdog reset[cite: 53] */
    esp_reset_reason_t reason = esp_reset_reason(); /* Lấy nguyên nhân reset[cite: 53] */
    
    /* Khởi tạo lại counter từ 0 sau một chu kỳ cắt nguồn vật lý hoặc nếu magic number bị sai[cite: 53] */
    if (wdt_magic != RESET_MAGIC || reason == ESP_RST_POWERON) {
        wdt_reset_count = 0;
        wdt_magic = RESET_MAGIC;
    }
    
    /* Chỉ tăng bộ đếm khi nguyên nhân chính xác là do Task WDT reset[cite: 53] */
    if (reason == ESP_RST_TASK_WDT) {
        wdt_reset_count++;
    }
    
    ESP_LOGI(TAG, "boot: reason=%s  watchdog_resets=%lu", reset_reason_name(reason), wdt_reset_count); /*[cite: 53] */

    /* 2. Cấu hình Task Watchdog Timer */
    esp_task_wdt_config_t twdt_cfg = {
        .timeout_ms = TWDT_TIMEOUT_MS,
        .idle_core_mask = (1 << 0) | (1 << 1),
        .trigger_panic = true, /* Bật chế độ panic để board khởi động lại khi có task quá hạn[cite: 53] */
    };
    
    esp_err_t err = esp_task_wdt_reconfigure(&twdt_cfg); /* Ưu tiên dùng hàm reconfigure[cite: 53] */
    if (err == ESP_ERR_INVALID_STATE) {
        ESP_ERROR_CHECK(esp_task_wdt_init(&twdt_cfg));
    } else {
        ESP_ERROR_CHECK(err);
    }

    /* 3. Khởi tạo Semaphore và kiểm tra kết quả trả về[cite: 55] */
    hang_sem = xSemaphoreCreateBinary();
    if (hang_sem == NULL) {
        ESP_LOGE(TAG, "Failed to create semaphore"); /*[cite: 55] */
        return;
    }
    
    /* 4. Khởi tạo GPIO Nút nhấn và kiểm tra lỗi[cite: 54, 55] */
    gpio_config_t btn_cfg = {
        .pin_bit_mask = (1ULL << BTN_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    
    esp_err_t gpio_err = gpio_config(&btn_cfg); 
    if (gpio_err != ESP_OK) {
        ESP_LOGE(TAG, "GPIO config failed: %s", esp_err_to_name(gpio_err)); /*[cite: 54, 55] */
        return;
    }

    /* 5. Khởi tạo các Task với kiểm tra kết quả phân bổ bộ nhớ (pdPASS)[cite: 54, 55] */
    BaseType_t ret;
    
    ret = xTaskCreate(sensor_task, "sensor", 2048, NULL, 5, NULL); /*[cite: 53] */
    if (ret != pdPASS) {
        ESP_LOGE(TAG, "Failed to create sensor task"); /*[cite: 55] */
    }
    
    ret = xTaskCreate(report_task, "report", 2048, NULL, 5, NULL); /*[cite: 53] */
    if (ret != pdPASS) {
        ESP_LOGE(TAG, "Failed to create report task"); /*[cite: 55] */
    }
    
    ret = xTaskCreate(button_task, "button", 2048, NULL, 5, NULL); /* Đẩy logic nút bấm vào task riêng[cite: 54, 56] */
    if (ret != pdPASS) {
        ESP_LOGE(TAG, "Failed to create button task"); /*[cite: 55] */
    }
}
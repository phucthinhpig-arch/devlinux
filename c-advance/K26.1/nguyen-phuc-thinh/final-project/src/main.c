#include "app_core.h"
#include "hal_timer.h"
#include "event_log.h"
#include <stdio.h>
#include <unistd.h> /* Cho hàm sleep() mô phỏng */

#define TX_BUFFER_SIZE (32U)

int main(void) {
    printf("Starting IoT Monitoring System...\n");
    
    hal_timer_init();
    
    app_config_t cfg = {
        .temp_threshold_high = 400, /* 40.0 độ C */
        .humi_threshold_high = 800  /* 80.0 % */
    };
    
    if (app_core_init(&cfg) != STATUS_OK) {
        printf("System Init Failed!\n");
        return -1;
    }
    
    uint8_t tx_buf[TX_BUFFER_SIZE];
    uint32_t tx_len = 0;
    
    /* Super Loop mô phỏng 5 chu kỳ hoạt động */
    for (int i = 0; i < 5; ++i) {
        printf("\n--- Cycle %d ---\n", i + 1);
        
        if (app_core_process_cycle(tx_buf, TX_BUFFER_SIZE, &tx_len) == STATUS_OK) {
            printf("Telemetry Packet Ready (%u bytes): ", tx_len);
            for (uint32_t j = 0; j < tx_len; ++j) {
                printf("%02X ", tx_buf[j]);
            }
            printf("\n");
        }
        
        event_log_print_all();
        sleep(1); /* Nghỉ 1 giây giữa các chu kỳ */
    }
    
    printf("\nSystem Shutdown.\n");
    return 0;
}
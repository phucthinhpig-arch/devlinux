/**
 * @file main.c
 * @brief Exercise 4: Feature Flags for Network Interfaces
 */

#include <stdio.h>

/* 
 * Hardware Abstraction Layer (HAL) Configuration Check 
 * Enforces that exactly ONE network interface is active.
 */
#if defined(CONFIG_WIFI_ENABLED) && defined(CONFIG_ETHERNET_ENABLED)
    /* Throw a compile-time error if the user tries to compile with BOTH enabled */
    #error "Cannot enable both WiFi and Ethernet at the same time!"
#elif defined(CONFIG_WIFI_ENABLED)
    /**
     * @brief Simulates Wi-Fi driver initialization.
     */
    static void wifi_driver_init(void)
    {
        printf("[NET] Initializing Wi-Fi Driver...\n");
    }
#elif defined(CONFIG_ETHERNET_ENABLED)
    /**
     * @brief Simulates Ethernet driver initialization.
     */
    static void ethernet_driver_init(void)
    {
        printf("[NET] Initializing Ethernet Driver...\n");
    }
#else
    /* Throw a compile-time error if neither network interface is enabled */
    #error "At least one network interface must be enabled!"
#endif

/**
 * @brief Main entry point of the program.
 * 
 * @return int Exit code (0 on success).
 */
int main(void)
{
    printf("=== Exercise 4: Feature Flags ===\n");

#if defined(CONFIG_WIFI_ENABLED)
    /* Only compile the Wi-Fi init call if Wi-Fi is enabled */
    wifi_driver_init();
#elif defined(CONFIG_ETHERNET_ENABLED)
    /* Only compile the Ethernet init call if Ethernet is enabled */
    ethernet_driver_init();
#endif

    return 0;
}
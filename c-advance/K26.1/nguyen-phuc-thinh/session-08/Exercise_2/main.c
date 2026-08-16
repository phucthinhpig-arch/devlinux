/**
 * @file main.c
 * @brief Exercise 2: Format String Patch
 */

#include <stdio.h>
#include <string.h>

/**
 * @brief Logs the status of a device safely, preventing format string attacks.
 * 
 * @param device_name The untrusted user-provided device name.
 */
void log_device_status(const char* device_name) 
{
    char status_message[128];
    
    /* Format the message into a local buffer */
    snprintf(status_message, sizeof(status_message), "Device online: %s", device_name);

    /* 
     * PATCH: Use "%s" to treat status_message strictly as a string.
     * This strictly adheres to CERT-C FIO30-C by separating the format string 
     * from the untrusted data, preventing memory leak or corruption.
     */
    printf("%s\n", status_message);
}

/**
 * @brief Main entry point of the program.
 * 
 * @return int Exit status (0 on success).
 */
int main(void) 
{
    printf("=== Exercise 2: Format String Patch ===\n");
    
    /* Simulated malicious input from network containing format specifiers */
    const char* attacker_payload = "Sensor_%x_%x_%x_%x";
    
    log_device_status(attacker_payload);
    
    return 0;
}
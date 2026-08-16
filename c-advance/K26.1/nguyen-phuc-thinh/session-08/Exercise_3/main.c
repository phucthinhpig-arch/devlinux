/**
 * @file main.c
 * @brief Exercise 3: Secure Serial Parser - Patched Version
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define MAX_PAYLOAD_SIZE 64
#define START_BYTE 0xAA

/**
 * @brief Structure to hold the parsed packet data.
 */
typedef struct {
    uint8_t length;
    uint8_t payload[MAX_PAYLOAD_SIZE];
} packet_t;

/**
 * @brief Parses incoming UART data safely, preventing buffer overflows.
 * 
 * @param raw_data The raw byte array received from the UART interface.
 */
void parse_packet(const uint8_t* raw_data) 
{
    packet_t pkt;

    if (raw_data[0] != START_BYTE) 
    {
        return; /* Invalid start byte, discard packet */
    }

    pkt.length = raw_data[1];

    /* 
     * PATCH: Strict bounds checking before memory operations.
     * This prevents memcpy from causing a Segmentation Fault if the 
     * sender maliciously claims a length larger than our buffer size.
     */
    if (pkt.length > MAX_PAYLOAD_SIZE) 
    {
        printf("ERROR: Packet length (%d) exceeds buffer size!\n", pkt.length);
        return; /* Safely discard the malicious packet */
    }

    /* Safe to copy since length is verified */
    memcpy(pkt.payload, &raw_data[2], pkt.length);

    printf("Successfully parsed packet of length: %d\n", pkt.length);
}

/**
 * @brief Main entry point simulating a malicious UART packet.
 * 
 * @return int Exit status (0 on success).
 */
int main(void) 
{
    printf("=== Exercise 3: Secure Serial Parser ===\n");

    /* Simulated malicious packet: Claims length is 200, but buffer is only 64! */
    uint8_t malicious_uart_data[] = {0xAA, 200, 0x01, 0x02, 0x03};
    
    parse_packet(malicious_uart_data);
    
    return 0;
}
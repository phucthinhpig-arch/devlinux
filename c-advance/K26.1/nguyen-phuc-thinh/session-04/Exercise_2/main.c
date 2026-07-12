#include "pool.h"
#include <stdio.h>

static network_packet_t packet_pool[POOL_SIZE];
static bool packet_in_use[POOL_SIZE];

void pool_init(void)
{
    for (uint32_t i = 0U; i < POOL_SIZE; i++) {
        packet_in_use[i] = false;
    }
}


network_packet_t* packet_alloc(void)
{
    for (uint32_t i = 0U; i < POOL_SIZE; i++) {
        if (!packet_in_use[i]) {
            packet_in_use[i] = true;
            packet_pool[i].id = i + 1U; 
            return &packet_pool[i];
        }
    }
    return NULL; 
}

void packet_free(network_packet_t* p_packet)
{
    if (p_packet == NULL) {
        return; 
    }

    for (uint32_t i = 0U; i < POOL_SIZE; i++) {
        if (p_packet == &packet_pool[i]) {
            packet_in_use[i] = false;
            return;
        }
    }
   
}


int main(void)
{
    pool_init();

    network_packet_t* packets[POOL_SIZE];
    for (uint32_t i = 0U; i < POOL_SIZE; i++) {
        packets[i] = packet_alloc();
        printf("Allocating packet %u: %s\n", i + 1U,
               (packets[i] != NULL) ? "Success" : "Failed");
    }

    network_packet_t* p_extra = packet_alloc();
    printf("Allocating packet 6: %s (Pool Full)\n",
           (p_extra != NULL) ? "Success" : "Failed");

    printf("Freeing packet 2...\n");
    packet_free(packets[1]);

    p_extra = packet_alloc();
    printf("Allocating packet 6 again: %s\n",
           (p_extra != NULL) ? "Success" : "Failed");

    return 0;
}

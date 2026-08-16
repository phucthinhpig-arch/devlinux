/**
 * @file main.c
 * @brief Exercise 5: Token Pasting & Stringification
 */

#include <stdio.h>
#include <stdint.h>

/* Define the device struct structure first so our macro has a type to use */
struct device {
    int dev_id;
};

/* Token pasting - generates a unique variable name at compile time */
#define DEFINE_DEVICE(name, id) \
    struct device device_ ## name ## _ ## id = { .dev_id = (id) }

/* Stringification - two levels ensure macros are expanded before stringifying */
#define STRINGIFY(x) #x
#define TO_STRING(x) STRINGIFY(x)

/* Version string - C merges adjacent string literals at compile time */
#define FW_VERSION_MAJOR 3
#define FW_VERSION_MINOR 0
#define FW_VERSION_PATCH 4

/* Concatenating three individually stringified macros */
#define FW_VERSION_STRING TO_STRING(FW_VERSION_MAJOR) "." TO_STRING(FW_VERSION_MINOR) "." TO_STRING(FW_VERSION_PATCH)

/* Zephyr Device Tree Mock */
#define DT_N_NODELABEL_my_i2c_REG_ADDR 0x40003000U

/* Paste node_id with _REG_ADDR to form the full constant name */
#define DT_REG_ADDR(node_id) node_id ## _REG_ADDR


/**
 * @brief Main entry point of the program.
 * 
 * @return int Exit code (0 on success).
 */
int main(void)
{
    printf("=== Exercise 5: Token Pasting & Stringification ===\n");
    
    /* 2. Stringification: Print Firmware Version */
    printf("Firmware version: %s\n", FW_VERSION_STRING);

    /* 1. Token Pasting: Generate struct variables at compile time */
    DEFINE_DEVICE(spi, 1);
    DEFINE_DEVICE(i2c, 2);

    /* Print the dev_id of each generated variable */
    printf("Initialized SPI device with ID: %d\n", device_spi_1.dev_id);
    printf("Initialized I2C device with ID: %d\n", device_i2c_2.dev_id);

    /* 3. Zephyr Device Tree Mock Lookup */
    uint32_t p_i2c_base = DT_REG_ADDR(DT_N_NODELABEL_my_i2c);
    
    /* Print the resolved compile-time constant */
    printf("I2C reg addr: 0x%08X\n", p_i2c_base);

    return 0;
}
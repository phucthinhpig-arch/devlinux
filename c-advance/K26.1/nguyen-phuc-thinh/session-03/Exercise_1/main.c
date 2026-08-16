#include <stdint.h>
#include <stdio.h>

typedef union
{
    uint32_t full_word;
    uint8_t bytes[4];
} endian_checker_u;

int main(void)
{
    endian_checker_u checker = {0U};

    checker.full_word = 0x11223344U;

    printf("=====================================\n");
    printf("        Endianness Checker\n");
    printf("=====================================\n\n");

    printf("Stored Value         : 0x%08X\n", checker.full_word);
    printf("Byte[0] in Memory    : 0x%02X\n\n", checker.bytes[0]);

    if (checker.bytes[0] == 0x44U)
    {
        printf("Result : Little-Endian System\n");
    }
    else if (checker.bytes[0] == 0x11U)
    {
        printf("Result : Big-Endian System\n");
    }
    else
    {
        printf("Result : Unknown Endianness\n");
    }

    return 0;
}
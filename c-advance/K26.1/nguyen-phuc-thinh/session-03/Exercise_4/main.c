#include <stdio.h>
#include <stdint.h>

typedef union
{
    uint32_t val;
    uint8_t  arr[5];
} unpacked_union_t;

typedef struct __attribute__((packed))
{
    unpacked_union_t u;
} struct_with_unpacked_union_t;

typedef union
{
    uint32_t val;
    uint8_t  arr[5];
} packed_union_t __attribute__((packed));

typedef struct __attribute__((packed))
{
    packed_union_t u;
} struct_with_packed_union_t;

typedef struct
{
    uint32_t EN   : 1;
    uint32_t MODE : 3;
    uint32_t FLAG : 1;
    uint32_t res  : 27;
} hw_reg_bits_t;

typedef union
{
    uint32_t      ALL;
    hw_reg_bits_t BIT;
} hw_reg_t;

static void print_size_info(const char * const p_name, const size_t size)
{
    printf("  %-35s: %zu bytes\n", p_name, size);
}

int main(void)
{
    printf("=== Advanced Nested Packing ===\n\n");
    
    printf("[Task 1: Packed Unions Comparison]\n");
    
    print_size_info("unpacked_union_t (standalone)", 
                    sizeof(unpacked_union_t));
    print_size_info("packed_union_t (standalone)", 
                    sizeof(packed_union_t));
    
    printf("\n");
    
    print_size_info("struct with UNPACKED union (tail padding exists)", 
                    sizeof(struct_with_unpacked_union_t));
    print_size_info("struct with PACKED union (tail padding eliminated)", 
                    sizeof(struct_with_packed_union_t));
    
    printf("\n=== Struct Bit-Fields & Hardware Mapping ===\n\n");
    
    printf("[Task 2: Bit-Field Register]\n");
    print_size_info("hw_reg_bits_t (bit-field struct)", 
                    sizeof(hw_reg_bits_t));
    
    printf("\n[Task 3: Peripheral Union Pattern]\n");
    
    hw_reg_t reg = { .ALL = 0U };
    
    printf("Register ALL before: 0x%08X\n", reg.ALL);
    
    printf("Setting EN bit via bit-field...\n");
    reg.BIT.EN = 1U;
    printf("Register ALL after: 0x%08X\n", reg.ALL);
    
    printf("Clearing register via ALL...\n");
    reg.ALL = 0U;
    printf("Register ALL final: 0x%08X\n", reg.ALL);
    
    printf("\n=== CMSIS Bit-Field Portability Discussion ===\n\n");
    
    printf("CMSIS strongly PROHIBITS using bit-fields for hardware registers!\n");
    printf("Reasons:\n");
    printf("  1. Bit ordering is compiler-dependent (not portable)\n");
    printf("  2. Padding and alignment are not standardized\n");
    printf("  3. Non-atomic access in interrupt context\n");
    printf("  4. Different compilers produce different memory layouts\n\n");
    printf("Recommended solution: Use bitwise operations with macros\n");
    printf("Example: #define REG_SET_ENABLE() (*(reg) |= (1U << 0))\n");
    
    return 0;
}
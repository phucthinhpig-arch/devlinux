#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

typedef struct
{
    char     c;
    int32_t  i;
    double   d;
    int16_t  s;
} unoptimized_t;

typedef struct
{
    double   d;
    int32_t  i;
    int16_t  s;
    char     c;
} optimized_t;

typedef struct __attribute__((packed))
{
    char     c;
    int32_t  i;
    double   d;
    int16_t  s;
} packed_t;

static void print_struct_info(const char * const p_name, 
                              const size_t size,
                              const size_t * const p_offsets,
                              const uint8_t num_members)
{
    printf("[%s]\n", p_name);
    printf("Size: %zu bytes\n", size);
    printf("Offsets: ");
    
    for (uint8_t u8_idx = 0; u8_idx < num_members; u8_idx++)
    {
        printf("%s(%zu)", 
               (u8_idx == 0) ? "c" : 
               (u8_idx == 1) ? "i" : 
               (u8_idx == 2) ? "d" : "s",
               p_offsets[u8_idx]);
               
        if (u8_idx < (num_members - 1U))
        {
            printf(", ");
        }
    }
    printf("\n\n");
}

int main(void)
{
    printf("=== Struct Padding Analyzer ===\n\n");
    
    unoptimized_t unopt;
    size_t unopt_offsets[4] = {
        offsetof(unoptimized_t, c),
        offsetof(unoptimized_t, i),
        offsetof(unoptimized_t, d),
        offsetof(unoptimized_t, s)
    };
    
    print_struct_info("Unoptimized Struct", 
                     sizeof(unoptimized_t), 
                     unopt_offsets, 
                     4U);
    
    optimized_t opt;
    size_t opt_offsets[4] = {
        offsetof(optimized_t, d),
        offsetof(optimized_t, i),
        offsetof(optimized_t, s),
        offsetof(optimized_t, c)
    };
    
    print_struct_info("Optimized Struct", 
                     sizeof(optimized_t), 
                     opt_offsets, 
                     4U);
    
    packed_t pck;
    size_t pck_offsets[4] = {
        offsetof(packed_t, c),
        offsetof(packed_t, i),
        offsetof(packed_t, d),
        offsetof(packed_t, s)
    };
    
    print_struct_info("Packed Struct", 
                     sizeof(packed_t), 
                     pck_offsets, 
                     4U);
    
    printf("Attempting direct access to packed member... ");
    
    pck.i = 10;
    printf("Success! (pck.i = %d)\n", pck.i);
    
    printf("Attempting pointer access to packed member... ");
    
    int32_t *p_int = &pck.i;
    *p_int = 20;
    printf("Success! (pck.i = %d via pointer)\n", *p_int);
    
    printf("\n=== Discussion ===\n");
    printf("1. Unoptimized: %zu bytes (with padding)\n", sizeof(unoptimized_t));
    printf("2. Optimized:   %zu bytes (minimal padding)\n", sizeof(optimized_t));
    printf("3. Packed:      %zu bytes (no padding)\n", sizeof(packed_t));
    printf("\n=== WARNING ===\n");
    printf("Pointer access to packed struct member is DANGEROUS!\n");
    printf("- On ARM Cortex-M, misaligned access triggers HardFault\n");
    printf("- On x86, it may work but with performance penalty\n");
    printf("- Compiler may generate multiple memory accesses\n");
    printf("- Use packed only when necessary (e.g., network protocols)\n");
    
    return 0;
}
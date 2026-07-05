/**
 * @file main.c
 * @brief Memory segment analyzer for Session 02 Exercise 1.
 */

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

const uint32_t global_const = 100U;
uint32_t global_init = 42U;
uint32_t global_uninit;

typedef void (*text_func_ptr_t)(void);

typedef union
{
    text_func_ptr_t p_text_func;
    void *p_object;
} function_address_t;

static uintptr_t address_distance(uintptr_t first_addr, uintptr_t second_addr);
static void print_distance(const char *p_label, uintptr_t first_addr, uintptr_t second_addr);
void print_memory_map(void);

/**
 * @brief Calculate absolute distance between two addresses.
 *
 * @param[in] first_addr  First address as uintptr_t.
 * @param[in] second_addr Second address as uintptr_t.
 * @return Distance in bytes.
 */
static uintptr_t address_distance(uintptr_t first_addr, uintptr_t second_addr)
{
    uintptr_t distance = 0U;

    if (first_addr >= second_addr)
    {
        distance = first_addr - second_addr;
    }
    else
    {
        distance = second_addr - first_addr;
    }

    return distance;
}

/**
 * @brief Print distance between two memory addresses.
 *
 * @param[in] p_label     Distance label.
 * @param[in] first_addr  First address as uintptr_t.
 * @param[in] second_addr Second address as uintptr_t.
 */
static void print_distance(const char *p_label, uintptr_t first_addr, uintptr_t second_addr)
{
    printf("%-17s %" PRIuPTR " bytes\n", p_label, address_distance(first_addr, second_addr));
}

/**
 * @brief Print addresses of variables placed in major memory segments.
 */
void print_memory_map(void)
{
    uint32_t stack_var = 123U;
    uint32_t *p_heap_var = NULL;
    function_address_t text_address;
    uintptr_t text_addr = 0U;
    uintptr_t rodata_addr = 0U;
    uintptr_t data_addr = 0U;
    uintptr_t bss_addr = 0U;
    uintptr_t heap_addr = 0U;
    uintptr_t stack_addr = 0U;

    text_address.p_text_func = &print_memory_map;
    p_heap_var = malloc(sizeof(*p_heap_var));

    if (p_heap_var == NULL)
    {
        fprintf(stderr, "Error: malloc failed.\n");
        exit(EXIT_FAILURE);
    }

    *p_heap_var = 77U;

    text_addr = (uintptr_t)text_address.p_object;
    rodata_addr = (uintptr_t)&global_const;
    data_addr = (uintptr_t)&global_init;
    bss_addr = (uintptr_t)&global_uninit;
    heap_addr = (uintptr_t)p_heap_var;
    stack_addr = (uintptr_t)&stack_var;

    printf("=== Memory Segment Map ===\n");
    printf("[TEXT]   Address of print_memory_map():  %p\n", text_address.p_object);
    printf("[RODATA] Address of global_const:        %p\n", (const void *)&global_const);
    printf("[DATA]   Address of global_init:         %p\n", (void *)&global_init);
    printf("[BSS]    Address of global_uninit:       %p\n", (void *)&global_uninit);
    printf("[HEAP]   Address of heap_var:            %p\n", (void *)p_heap_var);
    printf("[STACK]  Address of stack_var:           %p\n", (void *)&stack_var);

    printf("\n=== Segment Distances ===\n");
    print_distance("RODATA - TEXT:", rodata_addr, text_addr);
    print_distance("DATA   - RODATA:", data_addr, rodata_addr);
    print_distance("BSS    - DATA:", bss_addr, data_addr);
    print_distance("HEAP   - BSS:", heap_addr, bss_addr);
    print_distance("STACK  - HEAP:", stack_addr, heap_addr);

    free(p_heap_var);
    p_heap_var = NULL;
}

/**
 * @brief Program entry point.
 *
 * @return EXIT_SUCCESS on success.
 */
int main(void)
{
    print_memory_map();

    return EXIT_SUCCESS;
}

/*
 * Verification output generated on one Linux/GCC environment.
 * Re-run these commands on your machine and update this block if required.
 *
 * $ size memory_map
 *    text    data     bss     dec     hex filename
 *    3001     628      16    3645     e3d memory_map
 *
 * $ nm memory_map | grep -i global_init
 * 0000000000004040 D global_init
 *
 * $ nm memory_map | grep -i global_uninit
 * 000000000000406c B global_uninit
 */

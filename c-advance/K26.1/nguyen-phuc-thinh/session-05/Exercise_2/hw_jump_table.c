#include <stdint.h>
#include <stdio.h>

/**
 * @brief Typedef for a function pointer that handles drawing a UI menu page.
 * @param page_id The requested menu index.
 */
typedef void (*menu_handler_t)(uint8_t page_id);

/**
 * @brief Mock handler for drawing the Main Menu.
 * @param page_id Passed parameter (unused).
 */
static void draw_menu(uint8_t page_id)
{
    (void)page_id; /* Cast to void to strictly prevent unused variable warning */
    printf("Drawing Main Menu...\n");
}

/**
 * @brief Mock handler for drawing the Settings Menu.
 * @param page_id Passed parameter (unused).
 */
static void draw_settings(uint8_t page_id)
{
    (void)page_id;
    printf("Drawing Settings Menu...\n");
}

/**
 * @brief Mock handler for drawing the About Menu.
 * @param page_id Passed parameter (unused).
 */
static void draw_about(uint8_t page_id)
{
    (void)page_id;
    printf("Drawing About Menu...\n");
}

/**
 * @brief Const array of function pointers (Jump Table).
 * The __attribute__ explicitly places this in a custom Flash section to save RAM.
 */
static const menu_handler_t p_jump_table[] __attribute__((section(".my_dispatch_table"))) = {
    draw_menu,
    draw_settings,
    draw_about
};

/* Macro to automatically determine the number of entries in the jump table */
#define MENU_COUNT (sizeof(p_jump_table) / sizeof(p_jump_table[0]))

/**
 * @brief Safely dispatches the UI rendering based on the requested index.
 * 
 * @param menu_index The index of the menu to draw.
 */
void dispatch_ui(uint8_t menu_index)
{
    /* MISRA-C Rule 18.1: Always bounds-check index before looking up */
    if (menu_index < (uint8_t)MENU_COUNT)
    {
        /* CERT-C EXP34-C: Defensively ensure function pointer is not NULL */
        if (p_jump_table[menu_index] != NULL)
        {
            p_jump_table[menu_index](menu_index);
        }
    }
    else
    {
        printf("Error: Invalid menu index!\n");
    }
}

int main(void)
{
    /* Test dispatching all valid menus */
    dispatch_ui(0); /* Expected: Main Menu */
    dispatch_ui(1); /* Expected: Settings Menu */
    dispatch_ui(2); /* Expected: About Menu */
    
    /* Test dispatching an invalid menu */
    dispatch_ui(99); /* Expected: Error */
    
    return 0;
}
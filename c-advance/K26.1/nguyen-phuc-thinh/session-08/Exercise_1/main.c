/**
 * @file main.c
 * @brief Exercise 1: Secure I/O (Buffer Overflows) - Patched Version
 * 
 * @note Attack Payload used to hack the vulnerable version: "AAAAAAAAAAAAA"
 * By entering a string longer than the 8-byte buffer, the null terminator 
 * and excess characters overflow into the adjacent memory space on the stack, 
 * overwriting the 'is_admin' boolean flag from 0 (false) to a non-zero value (true).
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/**
 * @brief Main function handling authentication.
 * 
 * @return int Exit status (0 on success).
 */
int main(void) 
{
    bool is_admin = false;
    char password[8];

    printf("=== Exercise 1: Secure I/O ===\n");
    printf("Enter admin password: ");
    
    /* Safely read input using fgets to prevent buffer overflow */
    if (fgets(password, sizeof(password), stdin) != NULL) 
    {
        /* Strip the trailing newline character left by fgets */
        size_t len = strlen(password);
        if ((len > 0) && (password[len - 1] == '\n')) 
        {
            password[len - 1] = '\0';
        }
    }

    /* Authentication check */
    if (strcmp(password, "secret") == 0) 
    {
        is_admin = true;
    }

    /* Privilege verification */
    if (is_admin) 
    {
        printf("Access Granted: Admin privileges unlocked.\n");
    } 
    else 
    {
        printf("Access Denied.\n");
    }

    return 0;
}
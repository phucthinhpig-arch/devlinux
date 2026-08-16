#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * @brief Macro to calculate the number of elements in an array.
 */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/**
 * @brief Type definition for a command action function pointer.
 */
typedef void (*cmd_action_t)(void);

/**
 * @brief Structure associating a string command with its handler function.
 */
typedef struct {
    const char   *p_command_str; /**< Null-terminated command string (keyword). */
    cmd_action_t action;         /**< Function pointer to invoke when matched. */
} command_entry_t;

/* ========================================================================== */
/* Command Handler Functions                                                  */
/* ========================================================================== */

static void Cmd_LED_On(void)
{
    printf("[CMD] LED turned ON.\n");
}

static void Cmd_LED_Off(void)
{
    printf("[CMD] LED turned OFF.\n");
}

static void Cmd_Motor_Start(void)
{
    printf("[CMD] Motor started at 1500 RPM.\n");
}

static void Cmd_Motor_Stop(void)
{
    printf("[CMD] Motor stopped.\n");
}

static void Cmd_Status(void)
{
    printf("[CMD] System status: OK.\n");
}

/* ========================================================================== */
/* Command Table (Stored in Flash / .rodata)                                  */
/* ========================================================================== */

/**
 * @brief Static constant array mapping command strings to their handlers.
 * 
 * MISRA Directive 4.12: No dynamic allocation.
 * CERT STR31-C: Pointers reference string literals (read-only).
 */
static const command_entry_t CMD_TABLE[] = {
    { "LED_ON",      Cmd_LED_On      },
    { "LED_OFF",     Cmd_LED_Off     },
    { "MOTOR_START", Cmd_Motor_Start },
    { "MOTOR_STOP",  Cmd_Motor_Stop  },
    { "STATUS",      Cmd_Status      }
};

/* ========================================================================== */
/* Dispatcher Implementation                                                  */
/* ========================================================================== */

/**
 * @brief Dispatches a received ASCII command string to its corresponding handler.
 * 
 * @param[in] p_received_cmd Null-terminated command string. Must not be NULL.
 */
void Dispatch_Command(const char *p_received_cmd)
{
    bool b_command_found = false;
    size_t i;

    /* MISRA Directive 4.11 / CERT EXP34-C: Guard against NULL pointer */
    if (p_received_cmd != NULL)
    {
        /* MISRA Rule 14.2: Well-formed for loop with compile-time bound */
        for (i = 0U; i < ARRAY_SIZE(CMD_TABLE); i++)
        {
            /* strcmp is safe here because p_received_cmd is guarded against NULL */
            if (strcmp(p_received_cmd, CMD_TABLE[i].p_command_str) == 0)
            {
                /* CERT EXP34-C: Guard the function pointer before invoking */
                if (CMD_TABLE[i].action != NULL)
                {
                    CMD_TABLE[i].action();
                }
                b_command_found = true;
                break; /* Match found, exit loop */
            }
        }

        if (b_command_found == false)
        {
            printf("[CMD] Unknown command: %s\n", p_received_cmd);
        }
    }
}

/* ========================================================================== */
/* Main Application Test                                                      */
/* ========================================================================== */

int main(void)
{
    /* Test all 5 valid commands */
    Dispatch_Command("LED_ON");
    Dispatch_Command("LED_OFF");
    Dispatch_Command("MOTOR_START");
    Dispatch_Command("MOTOR_STOP");
    Dispatch_Command("STATUS");
    
    /* Test 1 unknown command */
    Dispatch_Command("REBOOT");

    return 0;
}
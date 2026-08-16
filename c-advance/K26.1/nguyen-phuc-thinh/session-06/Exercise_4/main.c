#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

/**
 * @brief Enumeration of all valid traffic light states.
 * NUM_STATES acts as a sentinel value for bounds checking.
 */
typedef enum {
    RED = 0,
    GREEN = 1,
    YELLOW = 2,
    NUM_STATES
} traffic_state_t;

/**
 * @brief Uniform signature for all state handler functions.
 * 
 * @param[in]     tick         Current system tick count.
 * @param[in,out] p_next_state Pointer to the state variable. Updated to change state.
 */
typedef void (*traffic_handler_t)(uint32_t tick, traffic_state_t *p_next_state);

/* ========================================================================== */
/* FSM State Handlers                                                         */
/* ========================================================================== */

static void State_Red(uint32_t tick, traffic_state_t *p_next_state)
{
    printf("[RED]    Tick %u - Stop! Holding for 3 ticks.\n", tick);
    if ((tick % 3U) == 0U)
    {
        *p_next_state = GREEN;
    }
}

static void State_Green(uint32_t tick, traffic_state_t *p_next_state)
{
    printf("[GREEN]  Tick %u - Go! Holding for 3 ticks.\n", tick);
    if ((tick % 3U) == 0U)
    {
        *p_next_state = YELLOW;
    }
}

static void State_Yellow(uint32_t tick, traffic_state_t *p_next_state)
{
    printf("[YELLOW] Tick %u - Slow down!\n", tick);
    *p_next_state = RED;
}

/* ========================================================================== */
/* FSM Jump Table (Stored in Flash / .rodata)                                 */
/* ========================================================================== */

/**
 * @brief Static constant array mapping state enums to handler functions.
 * 
 * C99 designated initializers are used to explicitly link enum index to function.
 */
static const traffic_handler_t TrafficFSM[NUM_STATES] = {
    [RED]    = State_Red,
    [GREEN]  = State_Green,
    [YELLOW] = State_Yellow
};

/* ========================================================================== */
/* FSM Dispatcher                                                             */
/* ========================================================================== */

/**
 * @brief Executes one tick of the Function Pointer FSM.
 * 
 * @param[in]     tick    Current tick number.
 * @param[in,out] p_state Pointer to current state. Modified by handler if transitioning.
 */
void RunTrafficFSM(uint32_t tick, traffic_state_t *p_state)
{
    /* CERT EXP34-C / MISRA Directive 4.11: Validate pointer before dereferencing */
    if (p_state != NULL)
    {
        /* CERT ARR30-C / MISRA Rule 18.1: Strict array bounds check before dispatching */
        if (*p_state < NUM_STATES)
        {
            /* Defensively ensure the function pointer in the table is not NULL */
            if (TrafficFSM[*p_state] != NULL)
            {
                TrafficFSM[*p_state](tick, p_state);
            }
        }
        else
        {
            printf("[ERROR] FSM State out of bounds!\n");
        }
    }
}

/* ========================================================================== */
/* Main Application Test                                                      */
/* ========================================================================== */

int main(void)
{
    /* MISRA Rule 9.1: Initialize variable before use. No global variables. */
    traffic_state_t current_state = RED;
    uint32_t tick;

    /* MISRA Rule 14.2: Well-formed for loop for the 10-tick simulation */
    for (tick = 1U; tick <= 10U; tick++)
    {
        RunTrafficFSM(tick, &current_state);
    }

    return 0;
}
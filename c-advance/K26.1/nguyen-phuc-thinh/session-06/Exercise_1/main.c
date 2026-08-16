#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>

/**
 * @brief Type definition for the timer callback function pointer.
 */
typedef void (*timer_callback_t)(void);

/**
 * @brief Internal timer state (Singleton).
 * 
 * Hidden within the file scope using the 'static' keyword (MISRA Directive 4.12, Rule 8.9).
 * Initialized explicitly to prevent reading uninitialized memory (CERT EXP33-C, MISRA Rule 9.1).
 */
static struct {
    uint32_t         expire_at_tick; /**< Tick count at which the callback fires. */
    uint32_t         current_tick;   /**< Current elapsed tick count. */
    timer_callback_t p_on_expire;    /**< Callback to invoke on expiry. */
    bool             is_running;     /**< True if a timer is currently active. */
} s_timer = { 0U, 0U, NULL, false };

/**
 * @brief Register a callback to fire after a given number of ticks.
 * 
 * @param[in] expire_at_tick Tick number at which the callback fires.
 * @param[in] p_callback     Function pointer to invoke. Must not be NULL.
 */
void Timer_Register(uint32_t expire_at_tick, timer_callback_t p_callback)
{
    /* MISRA Rule 15.6: Mandatory braces for all control structures */
    if (s_timer.is_running == true)
    {
        printf("[WARN] Timer already running! Ignoring new registration.\n");
    }
    else
    {
        /* CERT EXP34-C / MISRA Directive 4.11: Validate pointer before use */
        if (p_callback != NULL)
        {
            s_timer.expire_at_tick = expire_at_tick;
            s_timer.current_tick   = 0U;
            s_timer.p_on_expire    = p_callback;
            s_timer.is_running     = true;
        }
    }
}

/**
 * @brief Advance the timer by one tick. Fires callback if expired.
 */
void Timer_Tick(void)
{
    if (s_timer.is_running == true)
    {
        s_timer.current_tick++;
        
        if (s_timer.current_tick == s_timer.expire_at_tick)
        {
            /* Guard against NULL before invocation (CERT EXP34-C) */
            if (s_timer.p_on_expire != NULL)
            {
                s_timer.p_on_expire();
            }
            s_timer.is_running = false;
        }
    }
}

/**
 * @brief Cancel any active timer.
 */
void Timer_Reset(void)
{
    s_timer.expire_at_tick = 0U;
    s_timer.current_tick   = 0U;
    s_timer.p_on_expire    = NULL;
    s_timer.is_running     = false;
}

/**
 * @brief Query whether a timer is currently active.
 * 
 * @return bool true if a timer is running, false otherwise.
 */
bool Timer_IsRunning(void)
{
    return s_timer.is_running;
}

/* ========================================================================== */
/* Test Callbacks and Main Function                                           */
/* ========================================================================== */

/**
 * @brief First alarm callback function.
 */
static void My_Alarm_Function(void)
{
    printf("[ALARM] Timer fired at tick 5!\n");
}

/**
 * @brief Second alarm callback function.
 */
static void My_Second_Alarm_Function(void)
{
    printf("[ALARM] Second alarm fired at tick 3!\n");
}

/**
 * @brief Application entry point.
 */
int main(void)
{
    uint32_t i;

    printf("--- Test 1: Alarm at tick 5, run for 10 ticks ---\n");
    Timer_Register(5U, My_Alarm_Function);
    
    for (i = 1U; i <= 10U; i++)
    {
        printf("Tick %u...\n", i);
        Timer_Tick();
        
        /* Demonstrate registering a second timer while the first is active */
        if (i == 6U)
        {
            Timer_Register(10U, My_Alarm_Function);
        }
    }

    printf("\n--- Test 2: Reset, then new alarm at tick 3 ---\n");
    Timer_Reset();
    printf("[TIMER] Reset.\n");
    
    Timer_Register(3U, My_Second_Alarm_Function);
    
    for (i = 1U; i <= 3U; i++)
    {
        printf("Tick %u...\n", i);
        Timer_Tick();
    }

    return 0;
}
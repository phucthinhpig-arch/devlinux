#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

/**
 * @brief Enumeration of all valid WiFi FSM states.
 * WIFI_MAX_STATES acts as a sentinel value for array bounds checking.
 */
typedef enum {
    WIFI_INIT       = 0,
    WIFI_CONNECTING = 1,
    WIFI_CONNECTED  = 2,
    WIFI_ERROR      = 3,
    WIFI_MAX_STATES      /**< Sentinel - do NOT assign explicitly. */
} WIFI_STATE;

/**
 * @brief Uniform state function signature for the WiFi FSM.
 * 
 * @param[in]     input        Hardware/event input (1 = link up, 0 = link down/fail).
 * @param[in,out] p_next_state Pointer to the current state; updated on transition.
 * @return 0U on success.
 */
typedef uint32_t (*wifi_handler_t)(uint8_t input, WIFI_STATE *p_next_state);

/* ========================================================================== */
/* FSM State Handlers (Forward Declarations & Implementations)                */
/* ========================================================================== */

/**
 * @brief Handler for WIFI_INIT state.
 */
static uint32_t wifi_state_init(uint8_t input, WIFI_STATE *p_next_state)
{
    (void)input; /* MISRA: Explicitly ignore unused parameter */
    
    printf("[INIT] Initializing... -> CONNECTING\n");
    *p_next_state = WIFI_CONNECTING;
    
    return 0U;
}

/**
 * @brief Handler for WIFI_CONNECTING state.
 * Contains a static retry counter that persists between calls.
 */
static uint32_t wifi_state_connecting(uint8_t input, WIFI_STATE *p_next_state)
{
    /* CERT EXP33-C: Static variable properly managing state locally */
    static uint32_t s_retry_count = 0U; 

    if (input == 1U)
    {
        s_retry_count = 0U; /* Reset on success */
        printf("[CONNECTING] Connected! Retry count reset.\n");
        *p_next_state = WIFI_CONNECTED;
    }
    else
    {
        s_retry_count++;
        if (s_retry_count >= 3U)
        {
            printf("[CONNECTING] Attempt %u failed. -> ERROR\n", s_retry_count);
            s_retry_count = 0U; /* Reset on giving up */
            *p_next_state = WIFI_ERROR;
        }
        else
        {
            printf("[CONNECTING] Attempt %u failed. Retrying...\n", s_retry_count);
            *p_next_state = WIFI_CONNECTING;
        }
    }
    
    return 0U;
}

/**
 * @brief Handler for WIFI_CONNECTED state.
 */
static uint32_t wifi_state_connected(uint8_t input, WIFI_STATE *p_next_state)
{
    if (input == 1U)
    {
        printf("[CONNECTED] Link stable. Online.\n");
        *p_next_state = WIFI_CONNECTED;
    }
    else
    {
        printf("[CONNECTED] Link dropped. Reconnecting...\n");
        *p_next_state = WIFI_CONNECTING;
    }
    
    return 0U;
}

/**
 * @brief Handler for WIFI_ERROR state.
 */
static uint32_t wifi_state_error(uint8_t input, WIFI_STATE *p_next_state)
{
    (void)input; /* Automatic transition, input ignored */
    
    printf("[ERROR] Recovery. Restarting -> INIT\n");
    *p_next_state = WIFI_INIT;
    
    return 0U;
}

/* ========================================================================== */
/* FSM Jump Table (Stored in Flash)                                           */
/* ========================================================================== */

/**
 * @brief Static constant array mapping states to their handler functions.
 * MISRA Directive 4.12: No dynamic allocation. Placed in .rodata.
 */
static const wifi_handler_t wifiFSM[WIFI_MAX_STATES] = {
    [WIFI_INIT]       = wifi_state_init,
    [WIFI_CONNECTING] = wifi_state_connecting,
    [WIFI_CONNECTED]  = wifi_state_connected,
    [WIFI_ERROR]      = wifi_state_error
};

/* ========================================================================== */
/* Dispatcher Implementation                                                  */
/* ========================================================================== */

/**
 * @brief Runs one step of the WiFi FSM.
 * 
 * @param[in]     input   Hardware/event input.
 * @param[in,out] p_state Pointer to the current state.
 * @return 0U on success, 0xFFU on invalid state/pointer error.
 */
uint32_t RunStateMachine(uint8_t input, WIFI_STATE *p_state)
{
    uint32_t status = 0xFFU; /* MISRA Rule 15.5: Single exit point */

    /* CERT EXP34-C / MISRA Dir 4.11: Validate pointer before dereferencing */
    if (p_state != NULL)
    {
        /* CERT ARR30-C / MISRA Rule 18.1: Strict bounds check */
        if (*p_state < WIFI_MAX_STATES)
        {
            /* Guard function pointer before invocation */
            if (wifiFSM[*p_state] != NULL)
            {
                status = wifiFSM[*p_state](input, p_state);
            }
        }
    }

    return status;
}

/* ========================================================================== */
/* Main Application Test                                                      */
/* ========================================================================== */

int main(void)
{
    /* MISRA Rule 9.1: Initialize state variables before use */
    WIFI_STATE current_state = WIFI_INIT;
    
    /* Input sequence exactly as requested in requirements */
    const uint8_t input_seq[] = {0, 0, 1, 0, 0, 0, 0, 1};
    const size_t num_steps    = sizeof(input_seq) / sizeof(input_seq[0]);
    size_t i;

    /* Lookup table for formatted printing */
    const char* state_names[WIFI_MAX_STATES] = {
        "WIFI_INIT",
        "WIFI_CONNECTING",
        "WIFI_CONNECTED",
        "WIFI_ERROR"
    };

    /* MISRA Rule 14.2: Well-formed for loop */
    for (i = 0U; i < num_steps; i++)
    {
        /* Formatting to match expected standard output cleanly */
        if (current_state == WIFI_ERROR)
        {
            printf("[Step %zu] (auto) State: %-15s | input=%u\n", 
                   i, state_names[current_state], input_seq[i]);
        }
        else
        {
            printf("[Step %zu] State: %-15s | input=%u\n", 
                   i, state_names[current_state], input_seq[i]);
        }
        
        /* Dispatch the FSM */
        (void)RunStateMachine(input_seq[i], &current_state);
        
        printf("\n"); /* Spacing for readability */
    }

    return 0;
}
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/**
 * @brief Private ADC internal state.
 * 
 * Declared as static to prevent external access. Only getter functions 
 * are allowed to read from this struct.
 */
static struct {
    bool     is_initialized; /**< Guards against double-initialization. */
    uint32_t active_channel; /**< Currently selected ADC channel. */
    uint32_t sample_rate_hz; /**< Configured sampling rate in Hz. */
} s_adc_ctx = { false, 0U, 0U };

/**
 * @brief Initializes the ADC peripheral.
 * 
 * @param[in] sample_rate_hz Desired sampling rate in Hz.
 * @return bool true if initialized successfully, false if already initialized.
 */
bool ADC_Init(uint32_t sample_rate_hz)
{
    bool b_status = false; /* MISRA Rule 15.5: Single exit point variable */

    if (s_adc_ctx.is_initialized == true)
    {
        printf("[ADC] Error: Already initialized! Call ADC_DeInit() first.\n");
    }
    else
    {
        s_adc_ctx.sample_rate_hz = sample_rate_hz;
        s_adc_ctx.active_channel = 0U;
        s_adc_ctx.is_initialized = true;
        printf("[ADC] Initialized at %u Hz on channel 0.\n", sample_rate_hz);
        b_status = true;
    }

    return b_status;
}

/**
 * @brief De-initializes the ADC peripheral.
 */
void ADC_DeInit(void)
{
    s_adc_ctx.is_initialized = false;
    s_adc_ctx.active_channel = 0U;
    s_adc_ctx.sample_rate_hz = 0U;
    printf("[ADC] De-initialized.\n");
}

/**
 * @brief Sets the active ADC channel.
 * 
 * @param[in] channel The ADC channel to select.
 */
void ADC_SetChannel(uint32_t channel)
{
    if (s_adc_ctx.is_initialized == true)
    {
        s_adc_ctx.active_channel = channel;
        printf("[ADC] Channel set to %u.\n", channel);
    }
}

/**
 * @brief Reads the current analog value from the active ADC channel.
 * 
 * @return uint32_t The mocked voltage value in mV, or 0 if not initialized.
 */
uint32_t ADC_Read(void)
{
    uint32_t u32_result = 0U;

    if (s_adc_ctx.is_initialized == true)
    {
        /* Mock hardware read */
        u32_result = 300U;
        printf("[ADC] Read ch%u -> %u mV\n", s_adc_ctx.active_channel, u32_result);
    }

    return u32_result;
}

/**
 * @brief Getter: Checks if the ADC is initialized.
 * @return bool true if initialized, false otherwise.
 */
bool ADC_IsInitialized(void)
{
    return s_adc_ctx.is_initialized;
}

/**
 * @brief Getter: Retrieves the currently active ADC channel.
 * @return uint32_t Active channel number.
 */
uint32_t ADC_GetChannel(void)
{
    return s_adc_ctx.active_channel;
}

/**
 * @brief Getter: Retrieves the current sampling rate.
 * @return uint32_t Sampling rate in Hz.
 */
uint32_t ADC_GetSampleRate(void)
{
    return s_adc_ctx.sample_rate_hz;
}

/* ========================================================================== */
/* Main Application Test                                                      */
/* ========================================================================== */

/**
 * @brief Application entry point.
 */
int main(void)
{
    uint32_t adc_val;

    /* 1. Init ADC */
    (void)ADC_Init(44100U);
    
    /* 2. Attempt Double-Init to verify guard */
    (void)ADC_Init(48000U);

    /* 3. Read via Getters */
    printf("Channel:\t%u\n", ADC_GetChannel());
    printf("Sample rate:\t%u Hz\n", ADC_GetSampleRate());
    printf("Init status:\t%s\n", ADC_IsInitialized() ? "YES" : "NO");

    /* 4. Set channel and Read */
    ADC_SetChannel(2U);
    adc_val = ADC_Read();
    printf("Result: %u mV\n", adc_val);

    /* 5. DeInit and check state */
    ADC_DeInit();
    printf("Is initialized? %s\n", ADC_IsInitialized() ? "YES" : "NO");

    /* 6. Prove strict encapsulation (Compile Error demonstration) */
    /* 
     * In a standard multi-file architecture (e.g. adc.c / main.c), 
     * s_adc_ctx is completely hidden from main.c due to 'static' scope. 
     */
    // s_adc_ctx.active_channel = 99; // COMPILE ERROR 

    return 0;
}
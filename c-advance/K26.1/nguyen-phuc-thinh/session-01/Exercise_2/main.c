/* resubmit for grading */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

/**
 * @brief Converts a single hexadecimal character to its integer value.
 *
 * @param[in] c The ASCII character to convert.
 * @return Integer value 0-15 on success, -1 on invalid character.
 */
 
static int8_t hex_to_byte(const char c)
{
    int8_t val = -1;

    if ((c >= '0') && (c <= '9'))
    {
        val = (int8_t)(c - '0');
    }
    else if ((c >= 'a') && (c <= 'f'))
    {
        val = (int8_t)((c - 'a') + 10);
    }
    else if ((c >= 'A') && (c <= 'F'))
    {
        val = (int8_t)((c - 'A') + 10);
    }
    else
    {
        val = -1; 
    }

    return val;
}

/**
 * @brief Parse a MAC address string into a 6-byte array safely.
 *
 * @param[in] mac_str   Null-terminated ASCII string (e.g., "00:1A:2B:3C:4D:5E").
 * @param[out] p_mac_out Pointer to a 6-byte array to store the parsed MAC address.
 * @return 0 on success, -1 on invalid input.
 */

int8_t parse_mac(const char *mac_str, uint8_t *p_mac_out)
{
    int8_t status = 0;           
    uint8_t p_temp_mac[6] = {0}; 
    uint8_t i = 0U;
    uint16_t str_idx = 0U;

    if ((NULL == mac_str) || (NULL == p_mac_out))
    {
        status = -1;
    }
    else
    {
        for (i = 0U; i < 6U; ++i)
        {
            char c1 = mac_str[str_idx];
            if ('\0' == c1)
            {
                status = -1;
                break;
            }
            
            char c2 = mac_str[str_idx + 1U];
            if ('\0' == c2)
            {
                status = -1;
                break;
            }

            int8_t high_nibble = hex_to_byte(c1);
            int8_t low_nibble = hex_to_byte(c2);

            if ((high_nibble < 0) || (low_nibble < 0))
            {
                status = -1;
                break; 
            }

            p_temp_mac[i] = (uint8_t)(((uint8_t)high_nibble << 4U) | (uint8_t)low_nibble);
            str_idx += 2U;

            if (i < 5U)
            {
                char delim = mac_str[str_idx];
                if ((delim != ':') && (delim != '-'))
                {
                    status = -1;
                    break; 
                }
                str_idx += 1U;
            }
        }

        if ((0 == status) && ('\0' != mac_str[str_idx]))
        {
            status = -1; 
        }

        if (0 == status)
        {
            for (i = 0U; i < 6U; ++i)
            {
                p_mac_out[i] = p_temp_mac[i];
            }
        }
    }

    return status;
}

static void test_parse(const char *test_str)
{
    uint8_t mac_out[6] = {0};
    int8_t result = parse_mac(test_str, mac_out);
    
    printf("parse_mac(\"%s\", mac_out) -> returns %d", test_str ? test_str : "NULL", result);
    if (0 == result)
    {
        printf(", mac_out = {0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X}\n",
               mac_out[0], mac_out[1], mac_out[2], mac_out[3], mac_out[4], mac_out[5]);
    }
    else
    {
        printf("\n");
    }
}

int main(void)
{
    test_parse("00:1A:2B:3C:4D:5E");       
    test_parse("00-1a-2b-3c-4d-5e");       
    test_parse("00:1A:2B:3C:4D");          
    test_parse("00:1A:2B:3C:4D:5E:6F");    
    test_parse("00:1A:2B:3C:4D:5G");       
    test_parse(NULL);                      
    return 0;
}
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

/**
 * @brief Parse an IPv4 address string into a 32-bit unsigned integer.
 *
 * @param[in] ip_str   Null-terminated ASCII string (e.g., "192.168.1.50").
 * @param[out] p_ip_out Pointer to store the parsed 32-bit IP address.
 * @return 0 on success, -1 on invalid input.
 */
int8_t parse_ipv4(const char *ip_str, uint32_t *p_ip_out)
{
    int8_t status = -1; 

    if ((NULL != ip_str) && (NULL != p_ip_out))
    {
        bool error_found = false;
        uint32_t temp_ip = 0U;
        uint32_t current_octet = 0U;
        uint8_t octet_count = 0U;
        uint8_t digit_count = 0U;
        uint32_t i = 0U;

        while (('\0' != ip_str[i]) && (!error_found))
        {
            char c = ip_str[i];

            if ((c >= '0') && (c <= '9'))
            {
                digit_count++;
                
                if (digit_count > 3U)
                {
                    error_found = true;
                }
                else
                {
                    current_octet = (current_octet * 10U) + (uint32_t)(c - '0');
                    
                    if (current_octet > 255U)
                    {
                        error_found = true;
                    }
                }
            }
            else if ('.' == c)
            {
                if (0U == digit_count)
                {
                    error_found = true;
                }
                else
                {
                    octet_count++;
                    
                    if (octet_count > 3U)
                    {
                        error_found = true;
                    }
                    else
                    {
                        temp_ip = (temp_ip << 8U) | current_octet;
                        current_octet = 0U;
                        digit_count = 0U;
                    }
                }
            }
            else
            {
                error_found = true;
            }

            i++;
        }

        if ((!error_found) && (3U == octet_count) && (digit_count > 0U))
        {
            temp_ip = (temp_ip << 8U) | current_octet;
            *p_ip_out = temp_ip;
            status = 0; 
        }
    }

    return status;
}

static void test_parse(const char *test_str)
{
    uint32_t ip_out = 0U;
    int8_t result = parse_ipv4(test_str, &ip_out);

    if (NULL == test_str)
    {
        printf("parse_ipv4(NULL, &ip_out) -> returns %d  (NULL input)\n", result);
    }
    else if (0 == result)
    {
        printf("parse_ipv4(\"%s\", &ip_out) -> returns %d, ip_out = 0x%08X (%u)\n", 
               test_str, result, ip_out, ip_out);
    }
    else
    {
        printf("parse_ipv4(\"%s\", &ip_out) -> returns %d  (Invalid input)\n", test_str, result);
    }
}

int main(void)
{
    test_parse("192.168.1.50");
    test_parse("256.0.0.1");
    test_parse(NULL);

    return 0;
}
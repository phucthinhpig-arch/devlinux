#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Parse an IPv4 address string into a 32-bit unsigned integer.
 *
 * @param[in]  ip_str    Null-terminated ASCII string (e.g., "192.168.1.50").
 * @param[out] p_ip_out  Pointer to store the parsed 32-bit IP address.
 * @return 0 on success, -1 on invalid input.
 */
int8_t parse_ipv4(const char *ip_str, uint32_t *p_ip_out);
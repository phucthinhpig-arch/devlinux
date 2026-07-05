#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Parse a MAC address string into a 6-byte array.
 *
 * @param[in]  mac_str    Null-terminated ASCII string (e.g., "00:1A:2B:3C:4D:5E").
 * @param[out] p_mac_out  Pointer to a 6-byte array to store the parsed MAC address.
 * @return 0 on success, -1 on invalid input.
 */
int8_t parse_mac(const char *mac_str, uint8_t *p_mac_out);
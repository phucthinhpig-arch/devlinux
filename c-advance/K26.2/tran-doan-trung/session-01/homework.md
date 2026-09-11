> **📣 Message from your instructor:**
>
> Hi folks,
>
> This advanced C programming course recommends young engineers to code on your own! If possible, let's first try to write code from scratch. If it's hard, you guys can ask AI coding tool assistant! Don't let AI agent generate code for you!!
>
> Happy coding geeks! 🚀

---

# Assignment — Session 01: Coding Standards
**Deadline: 2026-09-27 23:59:00**

---

## Exercise_1 [build]

### Problem Statement

**Safe Network Address Parser — IPv4 to `uint32_t`**

**Scenario:**
In embedded networking (e.g., IoT firmware), we often receive network settings as ASCII strings from a configuration file, CLI, or web interface. We must parse these strings into numeric values safely without triggering undefined behavior.

**Efficiency (Storage & CPU Speed) Reasons:**
- **Memory footprint:** The string `"192.168.1.50"` takes 13 bytes in memory (12 characters + `\0` null terminator). A `uint32_t` takes exactly 4 bytes.
- **CPU Comparison Speed:** If a router needs to check if a packet's destination IP matches a routing rule, comparing strings requires calling `strcmp()` (which loops character by character). This is extremely slow. Comparing two `uint32_t` integers takes one CPU instruction (a simple assembly-level `CMP` instruction). When processing millions of packets per second, string comparisons would crash the network.

**Requirements:**

Write a C program that implements an IPv4 address parser with the following prototype:

```c
int8_t parse_ipv4(const char *ip_str, uint32_t *p_ip_out);
```

**Rules:**
- Check for `NULL` pointers defensively.
- Ensure the IP address components are valid (0 to 255).
- Return `0` on success, or a negative error code (e.g., `-1`) if the format is invalid.
- Avoid unsafe standard library functions like `atoi()` (which has undefined behavior on overflow). Use safe parsing logic instead.
- Follow BARR-C coding style (fixed-width integers, mandatory braces, pointer naming with `p_` prefix, Doxygen comments).
- Use `cppcheck` and `clang-tidy` to analysis, and make sure there are no warning or error message.
- Compile with strict flags: `-Wall -Wextra -pedantic -Werror -std=c99`.

### Coding Standards Reference

This exercise is a good opportunity to study the following industry rules. You don't need to memorize them — read the rule, understand *why* it exists, and apply the pattern in your code.

**MISRA-C 2012 (Safety):**
| Rule | Category | Relevance to This Exercise |
|---|---|---|
| Directive 4.11 | Required | The validity of values passed to library functions shall be checked → validate `ip_str` and `p_ip_out` for `NULL` before use. |
| Directive 4.12 | Required | Dynamic memory allocation shall not be used → use stack-based buffers and fixed-width integers only. |
| Directive 4.14 | Required | The validity of values received from external sources shall be checked → the IP string comes from untrusted input; every parsed octet must be validated (0–255, exactly 4 octets, correct `.` delimiters). |
| Rule 9.1 | Mandatory | The value of an object with automatic storage duration shall not be read before it has been set → initialize all local variables (e.g., `uint32_t ip_out = 0;`). |
| Rule 10.3 | Required | The value of an expression shall not be assigned to an object with a narrower essential type → be careful when converting parsed octet values (potentially `int`) into `uint8_t` range (0–255). |
| Rule 15.5 | Advisory | A function should have a single point of exit at the end → consider structuring early-return error checks carefully. |
| Rule 21.7 | Required | The Standard Library functions `atof`, `atoi`, `atol` and `atoll` shall not be used → implement your own safe parsing logic instead. |

**CERT-C 2016 (Security):**
| Rule | Relevance to This Exercise |
|---|---|
| EXP34-C | Do not dereference null pointers → validate `ip_str` and `p_ip_out` before dereferencing. |
| ARR30-C | Do not form or use out-of-bounds pointers or array subscripts → ensure character-by-character parsing never reads past the `\0` terminator. |
| INT32-C | Ensure that operations on signed integers do not result in overflow → when accumulating octet values (e.g., `octet = octet * 10 + digit`), validate range before the arithmetic. |
| INT31-C | Ensure that integer conversions do not result in lost or misinterpreted data → validate octet value fits in `uint8_t` range before narrowing conversion; careful when assembling 4 octets into `uint32_t`. |
| MSC24-C | Do not use deprecated or obsolescent functions → `atoi()` cannot report errors; use safe character-by-character parsing instead. |
| ERR33-C | Detect and handle standard library errors → check return values of any library calls used. |
| STR31-C | Guarantee that storage for strings has sufficient space for character data and the null terminator → applies if you copy parts of the IP string into local buffers. |

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs and read the full description of each rule above. After writing your code, verify your implementation follows these rules.


### Design Hints (optional)

```c
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
```

### Expected Output

```
parse_ipv4("192.168.1.50", &ip_out)  -> returns  0, ip_out = 0xC0A80132 (3232235826)
parse_ipv4("256.0.0.1",    &ip_out)  -> returns -1  (octet out of range)
parse_ipv4(NULL,            &ip_out)  -> returns -1  (NULL input)
```

Exit code: `0` on success, non-zero on error.

### Submission

```
Exercise_1/
├── main.c        (required)
├── Makefile      (required — targets: all, clean)
└── *.h           (if any)
```

---

## Exercise_2 [build]

### Problem Statement

**Safe Network Address Parser — MAC Address to `uint8_t[6]`**

**Scenario:**
Same embedded networking scenario as Exercise 1. Here, we parse MAC addresses received as ASCII strings into a 6-byte array safely, without triggering undefined behavior.

**Requirements:**

Write a C program that implements a MAC address parser with the following prototype:

```c
int8_t parse_mac(const char *mac_str, uint8_t *p_mac_out);
```

**Rules:**
- Check for `NULL` pointers defensively.
- Support both uppercase and lowercase hex digits (`0-9`, `a-f`, `A-F`).
- Validate structure (ensure correct placement of `:` or `-` delimiters).
- Ensure you do not overflow `p_mac_out` or perform pointer arithmetic past the end of the input string (`mac_str`).
- Return `0` on success, or a negative error code on failure.
- Follow BARR-C coding style (fixed-width integers, mandatory braces, pointer naming with `p_` prefix, Doxygen comments).
- Use `cppcheck` and `clang-tidy` to analysis, and make sure there are no warning or error message.
- Compile with strict flags: `-Wall -Wextra -pedantic -Werror -std=c99`.

### Coding Standards Reference

This exercise is a good opportunity to study the following industry rules. You don't need to memorize them — read the rule, understand *why* it exists, and apply the pattern in your code.

**MISRA-C 2012 (Safety):**
| Rule | Category | Relevance to This Exercise |
|---|---|---|
| Directive 4.11 | Required | The validity of values passed to library functions shall be checked → validate `mac_str` and `p_mac_out` for `NULL` before use. |
| Directive 4.14 | Required | The validity of values received from external sources shall be checked → the MAC string comes from untrusted input; validate exactly 6 hex-byte octets, correct delimiters (`:` or `-`), and valid hex characters. |
| Rule 9.1 | Mandatory | The value of an object with automatic storage duration shall not be read before it has been set → initialize all local variables. |
| Rule 10.3 | Required | The value of an expression shall not be assigned to an object with a narrower essential type → be careful when converting parsed hex values into `uint8_t`. |
| Rule 14.2 | Required | A `for` loop shall be well-formed → if using a `for` loop to iterate over the 6 octets, ensure counter bounds are correct. |
| Rule 15.5 | Advisory | A function should have a single point of exit at the end → consider structuring early-return error checks carefully. |
| Rule 21.7 | Required | The Standard Library functions `atof`, `atoi`, `atol` and `atoll` shall not be used → implement your own hex-to-byte conversion logic. |

**CERT-C 2016 (Security):**
| Rule | Relevance to This Exercise |
|---|---|
| EXP34-C | Do not dereference null pointers → validate `mac_str` and `p_mac_out` before dereferencing. |
| ARR30-C | Do not form or use out-of-bounds pointers or array subscripts → ensure you never write beyond `p_mac_out[5]` and never read past the end of `mac_str`. |
| ARR38-C | Guarantee that library functions do not form invalid pointers → applies when using pointer arithmetic to walk through the MAC string. |
| INT32-C | Ensure that operations on signed integers do not result in overflow → validate hex digit values before arithmetic conversion. |
| INT31-C | Ensure that integer conversions do not result in lost or misinterpreted data → validate parsed hex pair fits in `uint8_t` before narrowing conversion. |
| MSC24-C | Do not use deprecated or obsolescent functions → avoid `atoi()`, `sscanf()` etc.; use safe character-by-character hex parsing instead. |
| STR31-C | Guarantee that storage for strings has sufficient space for character data and the null terminator → applies if you copy substrings during parsing. |

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs (under `C_Books/`) and read the full description of each rule above. After writing your code, verify your implementation follows these rules.

### Design Hints (optional)

```c
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
```

### Expected Output

```
parse_mac("00:1A:2B:3C:4D:5E", mac_out)  -> returns  0, mac_out = {0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E}
parse_mac("00-1a-2b-3c-4d-5e", mac_out)  -> returns  0, mac_out = {0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E}
parse_mac("00:1A:2B:3C:4D",    mac_out)  -> returns -1  (Not enough octets)
parse_mac("00:1A:2B:3C:4D:5E:6F", mac_out) -> returns -1  (Too many octets)
parse_mac("00:1A:2B:3C:4D:5G", mac_out)  -> returns -1  (Invalid hex character 'G')
parse_mac(NULL,                 mac_out)  -> returns -1  (Null input)
```

Exit code: `0` on success, non-zero on error.

### Submission

```
Exercise_2/
├── main.c        (required)
├── Makefile      (required — targets: all, clean)
└── *.h           (if any)
```

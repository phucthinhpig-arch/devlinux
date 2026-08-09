> **📣 Message from your instructor:**
>
> Hi folks,
>
> This advanced C programming course recommends young engineers to code on your own!
> If possible, let's first try to write code from scratch. If it's hard, you guys can
> ask AI coding tool assistant! Don't let AI agent generate code for you!!
>
> Happy coding geeks! 🚀

---

# Assignment — Session 08: Secure Standard I/O
**Deadline: 2026-08-09 23:59:00**

---

## Overview

This homework covers the critical security concepts from Lecture 8:

| Exercise | Topic | Difficulty |
|:---|:---|:---:|
| Exercise_1 | Hacking Insecure I/O (Buffer Overflows) | ★★☆ |
| Exercise_2 | The Format String Attack | ★★☆ |
| Exercise_3 | Secure Serial Parser (Bounds Checking) | ★★★ |

> **Prerequisite**: Review the 3-step Real-World Embedded I/O Workflow from `L8.md` before starting.

---

## Exercise_1 [build]

### Problem Statement

**Hacking Insecure I/O (Buffer Overflows)**

**Scenario:**
You have inherited an old authentication server program. It reads a password from standard input using the deprecated and unsafe `gets()` function. 
Your task is three-fold:
1. **The Attack (Execution):** Compile and run the vulnerable code first to analyze the flaw. Provide an input that overflows the buffer to bypass the authentication check (triggering the "Access Granted" message even with a wrong password). *(Note: To successfully simulate the stack overflow attack on modern compilers and enable debugging, compile with `gcc -g -fno-stack-protector main.c`).*
2. **The Investigation (GDB):** Run the vulnerable program inside `gdb`. Set a breakpoint at the `if (strcmp...)` line. Run the program, enter your malicious payload, and when the breakpoint hits, use `print password` and `print is_admin` to physically observe how the adjacent memory was overwritten.
3. **The Patch:** Rewrite the program to be completely secure. Replace `gets()` with a safe, bounds-checked alternative from `<stdio.h>`.

**Vulnerable Starting Code (`main.c`):**
```c
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

int main(void) {
    bool is_admin = false;
    char password[8];

    printf("Enter admin password: ");
    gets(password); /* ⚠️ DANGER: Unbounded read! */

    if (strcmp(password, "secret") == 0) {
        is_admin = true;
    }

    if (is_admin) {
        printf("Access Granted: Admin privileges unlocked.\n");
    } else {
        printf("Access Denied.\n");
    }

    return 0;
}
```

**Requirements for the Patch:**
- Replace `gets()` with `fgets()`.
- Ensure the newline character read by `fgets()` is properly stripped before comparing with `strcmp()`.
- Ensure no buffer overflow can occur, regardless of how many characters the user types.

**Rules:**
- Follow BARR-C coding style.
- **Code Documentation:** All functions and data structures MUST be fully documented using Doxygen-style comments.
- Use `cppcheck` and `clang-tidy` to analyze, and make sure there are no warning or error messages.
- Compile with strict flags: `-Wall -Wextra -pedantic -Werror -std=c99`.

### Coding Standards Reference

**MISRA-C 2012 (Safety):**
| Rule | Category | Relevance to This Exercise |
|---|---|---|
| Directive 4.14 | Required | The validity of values received from external sources shall be checked (Ensure I/O is bounded). |

**CERT-C 2016 (Security):**
| Rule | Relevance to This Exercise |
|---|---|
| MSC24-C | Do not use deprecated or obsolescent functions (e.g., `gets`). |
| STR31-C | Guarantee that storage for strings has sufficient space for character data and the null terminator. |

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs (under `C_Books/`)
> and read the full description of each rule above. After writing your code,
> verify your implementation follows these rules.

### Design Hints (optional)

```c
/* Safe string input pattern */
if (fgets(password, sizeof(password), stdin) != NULL) {
    /* Strip the trailing newline left by fgets */
    size_t len = strlen(password);
    if ((len > 0) && (password[len - 1] == '\n')) {
        password[len - 1] = '\0';
    }
}
```

### Acceptance Criteria (Scoring)

- **[10%]** Documented attack payload: Include a comment at the top of your patched `main.c` describing the exact string you used to hack the vulnerable version.
- **[10%]** GDB Investigation: Submit a brief text file (`gdb_output.txt`) showing your GDB session where `is_admin` is shown to be corrupted by your payload.
- **[15%]** Code builds successfully without warnings or errors.
- **[20%]** Code passes static analysis (`cppcheck`, `clang-tidy`).
- **[15%]** Code contains required Doxygen documentation.
- **[30%]** The patched program uses `fgets()`, safely strips the newline, and cannot be overflowed.

### Expected Output

**Program Execution:**
```
=== Exercise 1: Secure I/O ===
Enter admin password: AAAAAAAAAAAAAAAAAAA
Access Denied.
```

**Expected `gdb_output.txt` (Vulnerable Version):**
```text
(gdb) break main.c:13
(gdb) run
Enter admin password: AAAAAAAAAAAA
Breakpoint 1, main () at main.c:13
(gdb) print password
$1 = "AAAAAAAAAAAA"
(gdb) print is_admin
$2 = true
```

Exit code: `0` on success.

### Submission

```
Exercise_1/
├── main.c           (required)
├── Makefile         (required — targets: all, clean)
└── gdb_output.txt   (required)
```

---

## Exercise_2 [build]

### Problem Statement

**The Format String Attack**

**Scenario:**
You are maintaining a smart home hub. The device takes user-provided device names and prints them to a status log. Unfortunately, the original developer passed the untrusted user string directly into the format parameter of `printf()`.

**Vulnerable Starting Code (`main.c`):**
```c
#include <stdio.h>
#include <string.h>

void log_device_status(const char* device_name) {
    char status_message[128];
    snprintf(status_message, sizeof(status_message), "Device online: %s", device_name);
    
    /* ⚠️ DANGER: Format String Vulnerability in the Processing Step! */
    printf(status_message); 
    printf("\n");
}

int main(void) {
    /* Simulated malicious input from network */
    const char* attacker_payload = "Sensor_%x_%x_%x_%x"; 
    log_device_status(attacker_payload);
    return 0;
}
```

**Your Task:**
1. **The Attack (Execution):** Run the vulnerable code and observe how `%x` forces `printf` to dump memory addresses from the stack, leaking internal data. *(Compile with `gcc -g main.c`)*.
2. **The Investigation (GDB):** Run the vulnerable program inside `gdb`. Set a breakpoint just before the `printf(status_message);` line. Use `info locals` or `x/16x $sp` to view the stack memory directly. Compare the raw stack bytes in GDB with the hex output printed by the program to prove that the format string is leaking actual stack data.
3. **The Patch:** Fix the vulnerability in `log_device_status()` so that `status_message` is printed safely. Even if the device name contains `%x` or `%n`, it must be treated purely as string data, not as a formatting command.

**Rules:**
- Follow BARR-C coding style.
- **Code Documentation:** All functions MUST be fully documented using Doxygen-style comments.
- Use `cppcheck` and `clang-tidy` to analyze, and make sure there are no warning or error messages.
- Compile with strict flags: `-Wall -Wextra -pedantic -Werror -std=c99`.

### Coding Standards Reference

**MISRA-C 2012 (Safety):**
| Rule | Category | Relevance to This Exercise |
|---|---|---|
| Directive 4.14 | Required | The validity of values received from external sources shall be checked (Do not trust user strings as format instructions). |

**CERT-C 2016 (Security):**
| Rule | Relevance to This Exercise |
|---|---|
| FIO30-C | Exclude user input from format strings. |

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs (under `C_Books/`)
> and read the full description of each rule above. After writing your code,
> verify your implementation follows these rules.

### Design Hints (optional)

```c
/* To fix a format string attack, always separate the format string from the data */
printf("%s", untrusted_buffer);
```

### Acceptance Criteria (Scoring)

- **[10%]** GDB Investigation: Submit a brief text file (`gdb_output.txt`) showing your GDB session proving the leaked `%x` values match the stack memory.
- **[15%]** Code builds successfully without warnings or errors.
- **[20%]** Code passes static analysis (`cppcheck`, `clang-tidy`).
- **[15%]** Code contains required Doxygen documentation.
- **[40%]** The patched program safely prints the payload literal string `"Sensor_%x_%x_%x_%x"` without dumping stack memory addresses.

### Expected Output

**Program Execution:**
```
=== Exercise 2: Format String Patch ===
Device online: Sensor_%x_%x_%x_%x
```

**Expected `gdb_output.txt` (Vulnerable Version):**
```text
(gdb) break main.c:9
(gdb) run
Breakpoint 1, log_device_status (...) at main.c:9
(gdb) x/4x $sp
0x7fffffffe000: 0x41414141 0x00000000 0x12345678 0xdeadbeef
```

Exit code: `0` on success.

### Submission

```
Exercise_2/
├── main.c           (required)
├── Makefile         (required — targets: all, clean)
└── gdb_output.txt   (required)
```

---

## Exercise_3 [build]

### Problem Statement

**Secure Serial Parser**

**Scenario:**
You are writing a driver to receive packets over a UART interface. The custom packet protocol defines a frame as:
`[START_BYTE (0xAA)] [LENGTH_BYTE] [PAYLOAD...] [CHECKSUM]`

The hardware provides the raw bytes, but network data arrives in chunks (The Fragmentation Problem). The current implementation blindly trusts the `LENGTH_BYTE` provided by the sender and copies it into a local fixed-size buffer, creating a massive buffer overflow vulnerability if the sender lies about the length.

**Vulnerable Starting Code (`main.c`):**
```c
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define MAX_PAYLOAD_SIZE 64
#define START_BYTE 0xAA

typedef struct {
    uint8_t length;
    uint8_t payload[MAX_PAYLOAD_SIZE];
} packet_t;

void parse_packet(const uint8_t* raw_data) {
    packet_t pkt;
    
    if (raw_data[0] != START_BYTE) {
        return; /* Invalid start */
    }
    
    pkt.length = raw_data[1];
    
    /* ⚠️ DANGER: Blindly trusting sender's length! Buffer overflow possible. */
    memcpy(pkt.payload, &raw_data[2], pkt.length); 
    
    printf("Successfully parsed packet of length: %d\n", pkt.length);
}

int main(void) {
    /* Simulated malicious packet: Claims length is 200, but buffer is only 64! */
    uint8_t malicious_uart_data[] = {0xAA, 200, 0x01, 0x02, 0x03}; 
    parse_packet(malicious_uart_data);
    return 0;
}
```

**Your Task:**
1. **The Attack (Execution):** Compile and run the vulnerable code. Observe the Segmentation Fault (crash) caused by `memcpy` attempting to copy 200 bytes into a 64-byte buffer. *(Compile with `gcc -g main.c`)*.
2. **The Investigation (GDB):** Run the vulnerable program inside `gdb`. When the Segmentation Fault occurs, it will pause execution. Use the `backtrace` (or `bt`) command to see exactly which line crashed, and use `print pkt.length` to observe the malicious length value that caused the crash.
3. **The Patch:** Rewrite `parse_packet()` to implement **strict bounds checking**.
   - Verify that `raw_data[1]` (the `LENGTH_BYTE`) does not exceed `MAX_PAYLOAD_SIZE`.
   - If the length is valid, perform the `memcpy`.
   - If the length is invalid (e.g., larger than `MAX_PAYLOAD_SIZE`), safely discard the packet, print an error message, and do not execute the copy.

**Rules:**
- Follow BARR-C coding style.
- **Code Documentation:** All functions MUST be fully documented using Doxygen-style comments.
- Use `cppcheck` and `clang-tidy` to analyze, and make sure there are no warning or error messages.
- Compile with strict flags: `-Wall -Wextra -pedantic -Werror -std=c99`.

### Coding Standards Reference

**MISRA-C 2012 (Safety):**
| Rule | Category | Relevance to This Exercise |
|---|---|---|
| Rule 18.1 | Required | A pointer resulting from arithmetic on a pointer operand shall address an element of the same array that was addressed by the operand. (Prevent buffer overflow). |

**CERT-C 2016 (Security):**
| Rule | Relevance to This Exercise |
|---|---|
| ARR30-C | Do not form or use out-of-bounds pointers or array subscripts. |
| MEM35-C | Allocate sufficient memory for an object. |

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs (under `C_Books/`)
> and read the full description of each rule above. After writing your code,
> verify your implementation follows these rules.

### Design Hints (optional)

```c
/* Always clamp/check lengths before memory operations */
if (raw_data[1] > MAX_PAYLOAD_SIZE) {
    printf("ERROR: Packet length (%d) exceeds buffer size!\n", raw_data[1]);
    return;
}
```

### Acceptance Criteria (Scoring)

- **[10%]** GDB Investigation: Submit a brief text file (`gdb_output.txt`) showing your GDB session catching the Segmentation Fault and printing the malicious `pkt.length`.
- **[15%]** Code builds successfully without warnings or errors.
- **[20%]** Code passes static analysis (`cppcheck`, `clang-tidy`).
- **[15%]** Code contains required Doxygen documentation.
- **[40%]** The patched program successfully rejects the malicious packet and prints the error message, preventing the `memcpy` buffer overflow.

### Expected Output

**Program Execution:**
```
=== Exercise 3: Secure Serial Parser ===
ERROR: Packet length (200) exceeds buffer size!
```

**Expected `gdb_output.txt` (Vulnerable Version):**
```text
(gdb) run
Program received signal SIGSEGV, Segmentation fault.
__memmove_avx_unaligned_erms () at ../sysdeps/x86_64/multiarch/memmove-vec-unaligned-erms.S:412
(gdb) bt
#0  __memmove_avx_unaligned_erms ()
#1  0x00005555555551c9 in parse_packet (raw_data=...) at main.c:23
#2  0x000055555555521b in main () at main.c:30
(gdb) frame 1
(gdb) print pkt.length
$1 = 200
```

Exit code: `0` on success.

### Submission

```
Exercise_3/
├── main.c           (required)
├── Makefile         (required — targets: all, clean)
└── gdb_output.txt   (required)
```

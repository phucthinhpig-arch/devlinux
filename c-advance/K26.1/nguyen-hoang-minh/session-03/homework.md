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

# Assignment — session-03
**Deadline: 2026-07-05 23:59:00**

---

## Exercise_1 [build]

### Problem Statement

Write a C program that uses a `union` containing a `uint32_t` and a `uint8_t` array to determine if the computer it is running on is Little-Endian or Big-Endian.

*Hint:* Store `0x11223344` in the integer and print the first byte of the array.

**Discussion Task:** In a block comment at the top of your `main.c`, explain:
1. What does hardware "Endianness" mean in terms of memory addresses?
2. What do LSB (Least Significant Byte) and MSB (Most Significant Byte) mean in this context?
3. Why is it critically important for embedded engineers to understand endianness when transmitting data over a network or reading from a hardware sensor?

### Design Hints (optional)

```c
#include <stdint.h>
#include <stdio.h>

typedef union {
    uint32_t full_word;
    uint8_t  bytes[4];
} endian_checker_u;
```

### Expected Output

Your output will depend on your machine's architecture (most x86/ARM Cortex-M are Little-Endian), but should look similar to:

```text
=== Endianness Checker ===
Stored Value: 0x11223344
First Byte in Memory: 0x44
Result: This system is Little-Endian!
```

### Acceptance Criteria (Scoring)
- **[20%]** Correctly uses a `union` combining a 32-bit unsigned integer and a 4-byte unsigned integer array.
- **[40%]** Logic correctly identifies and prints whether the system is "Little-Endian" or "Big-Endian" based on the byte value.
- **[20%]** Code builds successfully without warnings or errors.
- **[20%]** `cppcheck` and `clang-tidy` pass without issues.

### Rules
- Follow BARR-C coding style (fixed-width integers, mandatory braces, pointer naming with `p_` prefix, Doxygen comments).
- Use `cppcheck` and `clang-tidy` to analyze, and make sure there are no warning or error messages.
- Compile with strict flags: `-Wall -Wextra -pedantic -Werror -std=c99`.

### Coding Standards Reference

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs 
> and read the full description of each rule below. After writing your code,
> verify your implementation follows these rules.

**MISRA-C 2012 (Safety):**
| Rule | Category | Relevance to This Exercise |
|---|---|---|
| Rule 9.1 | Mandatory | Initialize before read | Ensure the union is fully initialized before attempting to read the byte array. |
| Rule 15.6 | Required | Mandatory braces | Use braces for all `if/else` statements when checking endianness. |

**CERT-C 2016 (Security):**
| Rule | Relevance to This Exercise |
|---|---|
| EXP33-C | No uninitialized memory reads. |

### Submission

```
Exercise_1/
├── main.c
└── Makefile
```

---

## Exercise_2 [build]

### Problem Statement

Create a `struct` with variables of types `char`, `int`, `double`, and `short`. 

1. Use `sizeof()` to find and print the total size of the original unoptimized struct.
2. Use the `offsetof()` macro (from `<stddef.h>`) to print the exact byte offset of each variable within the struct.
3. Reorder the variables to minimize the total size (optimize for padding).
4. Print the total size of the optimized struct using `sizeof()` to prove the size reduction.
5. Create a third version of the original unoptimized struct, but this time apply the `__attribute__((packed))` directive (or `#pragma pack`). 
6. Print its total size using `sizeof()` to demonstrate how packing forces the compiler to remove padding.
7. **Pointer vs Direct Access Task**: Create an instance of your packed struct. First, access its unaligned member directly (e.g., `packed_struct.my_int = 10;`). Then, attempt to access the same unaligned member using a pointer (e.g., `int *p = &packed_struct.my_int; *p = 20;`). Observe any compiler warnings. In a block comment, discuss the trade-offs and dangers of unaligned memory access (Hardware Faults on ARM) and why the pointer approach is unsafe.

### Design Hints (optional)

```c
#include <stdio.h>
#include <stddef.h> // For offsetof()

// 1. Unoptimized struct
typedef struct {
    char   c;      // 1 byte
    int    i;      // 4 bytes
    double d;      // 8 bytes
    short  s;      // 2 bytes
} unoptimized_t;

// 2. Optimized struct (Hint: order by size descending!)
typedef struct {
    double d;
    // ... complete the rest
} optimized_t;

// 3. Packed struct
typedef struct __attribute__((packed)) {
    char   c;
    int    i;
    double d;
    short  s;
} packed_t;
```

### Expected Output

Addresses and exact sizes may vary by architecture, but the padding reduction should look similar to:

```text
=== Struct Padding Analyzer ===
[Unoptimized Struct]
Size: 24 bytes
Offsets: c(0), i(4), d(8), s(16)

[Optimized Struct]
Size: 16 bytes
Offsets: d(0), i(8), s(12), c(14)

[Packed Struct]
Size: 15 bytes
Offsets: c(0), i(1), d(5), s(13)

Attempting direct access to packed member... Success!
Attempting pointer access to packed member... (Warning generated during compilation)
```

### Acceptance Criteria (Scoring)
- **[15%]** Program correctly declares the unoptimized struct and prints its total size and member offsets.
- **[20%]** Program declares an optimized version of the struct correctly ordered by size to eliminate padding.
- **[15%]** Program correctly implements a `__packed` version of the unoptimized struct and proves padding removal via `sizeof()`.
- **[20%]** Correctly demonstrates direct vs pointer access on a packed member and discusses the HardFault dangers.
- **[15%]** Code builds successfully without errors.
- **[15%]** `cppcheck` and `clang-tidy` pass without issues, and functions/structs use Doxygen comments.

### Rules
- Follow BARR-C coding style (fixed-width integers, mandatory braces, pointer naming with `p_` prefix, Doxygen comments).
- Use `cppcheck` and `clang-tidy` to analyze, and make sure there are no warning or error messages.
- Compile with strict flags: `-Wall -Wextra -pedantic -Werror -std=c99`.

### Coding Standards Reference

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs 
> and read the full description of each rule below. After writing your code,
> verify your implementation follows these rules.

**MISRA-C 2012 (Safety):**
| Rule | Category | Relevance to This Exercise |
|---|---|---|
| Rule 10.3 | Required | No narrowing type assignment | When calculating sizes or offsets (which are `size_t`), ensure format specifiers `%zu` are used to prevent narrowing. |

### Submission

```
Exercise_2/
├── main.c
└── Makefile
```

---

## Exercise_3 [build]

### Problem Statement

1. Create an `enum` for system permissions (`Read`, `Write`, `Execute`, `Delete`) mapping to specific bit flags (e.g., `1 << 0`, `1 << 1`, etc.). 
2. Write a function `bool has_permission(uint8_t user_perms, uint8_t required_perms)` that uses bitwise operators (`&`) to check if a user has the specified permissions.
3. In `main()`, test this function with different combinations of user permissions and required permissions.
4. **Enum Size Pitfall**: Print the size of your `enum` using `sizeof()`. In a block comment, answer: Why might the size of this enum differ between a GCC compiler for an x86 PC vs an ARM Cortex-M micro-controller? What is the compiler flag `-fshort-enums` and why is it dangerous when linking libraries?

### Design Hints (optional)

```c
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    PERM_READ    = (1 << 0),  // 0001
    PERM_WRITE   = (1 << 1),  // 0010
    PERM_EXECUTE = (1 << 2),  // 0100
    PERM_DELETE  = (1 << 3)   // 1000
} sys_perms_e;

/**
 * @brief Checks if the user has all the required permissions.
 */
bool has_permission(uint8_t user_perms, uint8_t required_perms);
```

### Expected Output

```text
=== Bitmask Permissions Tester ===
Enum size: 4 bytes (Standard GCC)
User 1 (Read|Write): 0x03
Checking for Read permission... GRANTED
Checking for Execute permission... DENIED
Checking for Read AND Write... GRANTED
```

### Acceptance Criteria (Scoring)
- **[20%]** `enum` is correctly implemented using explicit bit shifts for flags.
- **[40%]** `has_permission` function accurately uses bitwise `&` logic to verify if ALL required permissions are present.
- **[10%]** `main()` function tests multiple cases (success and failure).
- **[10%]** Code builds successfully without warnings or errors.
- **[20%]** `cppcheck` and `clang-tidy` pass without issues.

### Rules
- Follow BARR-C coding style (fixed-width integers, mandatory braces, pointer naming with `p_` prefix, Doxygen comments).
- Use `cppcheck` and `clang-tidy` to analyze, and make sure there are no warning or error messages.
- Compile with strict flags: `-Wall -Wextra -pedantic -Werror -std=c99`.

### Coding Standards Reference

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs 
> and read the full description of each rule below. After writing your code,
> verify your implementation follows these rules.

**MISRA-C 2012 (Safety):**
| Rule | Category | Relevance to This Exercise |
|---|---|---|
| Rule 10.4 | Required | Same essential type for arithmetic operands | Ensure the bitwise operator operands in `has_permission` share the same unsigned type. |

**CERT-C 2016 (Security):**
| Rule | Relevance to This Exercise |
|---|---|
| INT31-C | Safe integer narrowing conversions | Bitwise operations on `uint8_t` are promoted to `int` in C. Ensure safe conversion back if needed. |

### Submission

```
Exercise_3/
├── main.c
└── Makefile
```

---

## Exercise_4 [build]

### Problem Statement

1. **Packed Union Task**:
   - Define a `union` containing a `uint32_t` and a 5-byte array.
   - First, nest this unpacked union inside a packed struct and print the struct's size.
   - Second, apply `__attribute__((packed))` directly to the `union` definition itself, nest it in the packed struct, and print the size again.
   - In a comment, explain why packing the union itself was necessary to eliminate tail padding.
2. **Struct Bit-fields & Hardware Mapping**:
   - Create a `struct` with bit-fields matching a hypothetical 32-bit hardware register (e.g., `uint32_t EN:1; uint32_t MODE:3; uint32_t FLAG:1; uint32_t res:27;`).
   - Print `sizeof()` this struct.
3. **Peripheral Union Pattern**:
   - Nest this bit-field struct inside a `union` alongside a `uint32_t ALL` member.
   - Write code that sets the `EN` bit using the bit-field and then clears the entire register using the `ALL` member.
4. **Discussion Task**: In a block comment, discuss:
   - Why does the strict CMSIS standard forbid using struct bit-fields for mapping hardware registers?
   - What are the two compiler-dependent behaviors (endianness and padding boundaries) that break portability?

### Design Hints (optional)

```c
#include <stdio.h>
#include <stdint.h>

// --- Task 1: Packed Unions ---
typedef union {
    uint32_t val;
    uint8_t  arr[5];
} unpacked_union_t;

typedef struct __attribute__((packed)) {
    unpacked_union_t u;
} packed_struct_with_unpacked_union_t;

// --- Task 2 & 3: Peripheral Union Pattern ---
typedef struct {
    uint32_t EN   : 1;
    uint32_t MODE : 3;
    uint32_t FLAG : 1;
    uint32_t res  : 27;
} hw_reg_bits_t;

typedef union {
    uint32_t      ALL;
    hw_reg_bits_t BIT;
} hw_reg_t;
```

### Expected Output

```text
=== Advanced Nested Packing ===
Size of struct with UNPACKED union: 8 bytes (tail padding exists)
Size of struct with PACKED union: 5 bytes (tail padding eliminated)

=== Struct Bit-Fields & Hardware Mapping ===
Size of hw_reg_bits_t: 4 bytes
Register ALL before: 0x00000000
Setting EN bit via bit-field...
Register ALL after: 0x00000001
Clearing register via ALL...
Register ALL final: 0x00000000
```

### Acceptance Criteria (Scoring)
- **[30%]** Correctly demonstrates the size difference between a nested unpacked union vs a nested packed union.
- **[30%]** Correctly implements the bit-field struct and Peripheral Union Pattern to access the register.
- **[20%]** Block comment thoroughly explains the CMSIS portability dangers of bit-fields.
- **[10%]** Code builds successfully without warnings or errors.
- **[10%]** `cppcheck` and `clang-tidy` pass without issues, and functions/structs use Doxygen comments.

### Rules
- Follow BARR-C coding style (fixed-width integers, mandatory braces, pointer naming with `p_` prefix, Doxygen comments).
- Use `cppcheck` and `clang-tidy` to analyze, and make sure there are no warning or error messages.
- Compile with strict flags: `-Wall -Wextra -pedantic -Werror -std=c99`.

### Coding Standards Reference

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs 
> and read the full description of each rule below. After writing your code,
> verify your implementation follows these rules.

**MISRA-C 2012 (Safety):**
| Rule | Category | Relevance to This Exercise |
|---|---|---|
| Rule 19.2 | Advisory | The `union` keyword should not be used | MISRA warns against unions due to type punning risks. In this lab, we use unions for hardware mapping, so understand *why* the rule exists even if we intentionally violate it here for educational purposes. |

### Submission

```
Exercise_4/
├── main.c
└── Makefile
```

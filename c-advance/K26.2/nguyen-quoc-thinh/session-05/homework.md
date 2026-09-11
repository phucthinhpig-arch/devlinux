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

# Assignment — Session 05: Advanced Pointers & Dispatch Tables
**Deadline: 2026-07-18 23:59:00**

---

## Exercise_1 [build]

### Problem Statement

**Error Message Table (Array of String Pointers)**

In firmware development, converting error codes into human-readable strings is a common task. To save RAM, these strings should not be stored as a 2D array of `char` (which wastes memory padding). Instead, they should be stored as an array of `const char *` pointers.

Write a C program that builds and queries an error message lookup table:
1. Create an `enum` for error codes: `ERR_OK=0`, `ERR_TIMEOUT`, `ERR_HW_FAIL`, `ERR_COUNT`.
2. Create a global `const char * const` array of strings that maps exactly to the enum values.
3. Write a function `const char *get_error_string(uint8_t err_code)` that returns the correct string.
4. **Safety critical:** Your function MUST include bounds-checking. If an invalid error code (e.g., `99`) is passed, it must safely return a string like `"UNKNOWN_ERROR"`.
5. Test it in `main()` by printing the string for `ERR_TIMEOUT` and an invalid code `99`.

### Design Hints (optional)

```c
typedef enum {
    ERR_OK = 0,
    ERR_TIMEOUT,
    ERR_HW_FAIL,
    ERR_COUNT  /* Trick: Automatically equals the number of real errors */
} error_code_t;

/* Array of pointers to string literals */
static const char * const p_error_strings[] = {
    [ERR_OK]      = "OK",
    [ERR_TIMEOUT] = "TIMEOUT_ERROR",
    [ERR_HW_FAIL] = "HARDWARE_FAILURE"
};
```

### Acceptance Criteria (Scoring)

- **[20%]** Code builds successfully without warnings or errors.
- **[20%]** Code passes static analysis (`cppcheck`, `clang-tidy`).
- **[20%]** Code contains required Doxygen documentation.
- **[40%]** Logic correctly bounds-checks the input and returns the correct strings.

### Expected Output

When run, the output must be:
```
Error code 1: TIMEOUT_ERROR
Error code 99: UNKNOWN_ERROR
```

### Rules

- Follow BARR-C coding style (fixed-width integers, mandatory braces, pointer naming with `p_` prefix).
- **Code Documentation:** All functions and data structures MUST be fully documented using Doxygen-style comments.
- Use `cppcheck` and `clang-tidy` to analyze, and make sure there are no warning or error messages.
- Compile with strict flags: `-Wall -Wextra -pedantic -Werror -std=c99`.

### Coding Standards Reference

**MISRA-C 2012 (Safety):**
| Rule | Category | Relevance to This Exercise |
|---|---|---|
| Directive 4.14 | Required | The validity of values received from external sources shall be checked → Validate the `err_code` before using it as an array index. |
| Rule 18.1 | Required | A pointer resulting from arithmetic on a pointer operand shall address an element of the same array → Array indexing is pointer arithmetic; you must ensure it does not go out of bounds. |

**CERT-C 2016 (Security):**
| Rule | Relevance to This Exercise |
|---|---|
| ARR30-C | Do not form or use out-of-bounds pointers or array subscripts → A core requirement of `get_error_string`. |

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs (under `C_Books/`) and read the full description of each rule above. After writing your code, verify your implementation follows these rules.

### Submission

```
Exercise_1/
├── hw_array_of_pointers.c    (required)
└── Makefile                  (required — targets: all, clean)
```

---

## Exercise_2 [build]

### Problem Statement

**UI Menu Dispatcher (Jump Table)**

You are building an embedded device with a physical screen and 3 menus: Menu, Settings, and About. Instead of a massive, slow `switch` statement, you must implement an **O(1) Jump Table** (an array of function pointers). Furthermore, to save RAM, this table must be placed explicitly in a custom flash section.

Write a C program that implements this dispatcher:
1. Define a `typedef` for a function pointer that takes a `uint8_t` (page ID) and returns `void`.
2. Write 3 simple mock handler functions (`draw_menu`, `draw_settings`, `draw_about`). Each should just `printf` a message like `"Drawing Settings Menu..."`.
3. Build a `const` array of these function pointers.
4. **Flash Optimization:** Use `__attribute__((section(".my_dispatch_table")))` to explicitly place this array into a custom memory section.
5. Write a `void dispatch_ui(uint8_t menu_index)` function that performs bounds-checking and invokes the correct handler.
6. Test it in `main()` by dispatching all valid menus and one invalid menu.

### Acceptance Criteria (Scoring)

- **[15%]** Code builds successfully without warnings or errors.
- **[15%]** Code passes static analysis (`cppcheck`, `clang-tidy`).
- **[20%]** Code contains required Doxygen documentation.
- **[25%]** Jump table executes correctly and protects against invalid indices.
- **[25%]** Jump table is successfully placed in `.my_dispatch_table` (verified via `nm hw_jump_table | grep my_dispatch_table`).

### Expected Output

When run, the output must be:
```
Drawing Main Menu...
Drawing Settings Menu...
Drawing About Menu...
Error: Invalid menu index!
```

### Rules

- Follow BARR-C coding style (fixed-width integers, mandatory braces, pointer naming with `p_` prefix).
- **Code Documentation:** All functions and data structures MUST be fully documented using Doxygen-style comments.
- Use `cppcheck` and `clang-tidy` to analyze, and make sure there are no warning or error messages.
- Compile with strict flags: `-Wall -Wextra -pedantic -Werror -std=c99`.

### Coding Standards Reference

**MISRA-C 2012 (Safety):**
| Rule | Category | Relevance to This Exercise |
|---|---|---|
| Rule 8.13 | Advisory | A pointer should point to a const-qualified type whenever possible → Enforces making your jump table `const`. |
| Rule 17.3 | Mandatory | A function shall not be declared implicitly → Ensure your function pointer type signatures exactly match the handler functions. |
| Rule 18.1 | Required | Bounds-check the `menu_index` before looking up the function pointer in the array. |

**CERT-C 2016 (Security):**
| Rule | Relevance to This Exercise |
|---|---|
| EXP34-C | Do not dereference null pointers → Extremely critical for function pointers. You must validate the pointer is not NULL (or bounds-check the array index) before calling it. |

**BARR-C:2018:**
| Rule | Relevance to This Exercise |
|---|---|
| 6.3 | The `const` keyword shall be used wherever possible to reduce RAM footprint. |

> **How to use:** Open the MISRA-C 2012 and CERT-C 2016 PDFs (under `C_Books/`) and read the full description of each rule above. After writing your code, verify your implementation follows these rules.

### Submission

```
Exercise_2/
├── hw_jump_table.c    (required)
└── Makefile           (required — targets: all, clean)
```

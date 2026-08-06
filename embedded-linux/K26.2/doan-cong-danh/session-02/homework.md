# Assignment — session-02
**Deadline: 2026-08-09 23:59:00**

---

## Exercise_1 [build]

### Problem Statement

Create a static library `libmathutils.a` in C on Linux and use a Makefile to automate the build process. The library must provide the following functions:

- Add two integers
- Subtract two integers
- Compute the factorial of a non-negative integer

Write a main program inside `app/` that reads input from the terminal, calls the three library functions, and prints the results to stdout.

### Design Hints

```c
/* include/mathutils.h */
int math_add(int a, int b);
int math_sub(int a, int b);
int math_factorial(int n);
```

### Suggested Approach

```
1. Implement the three functions in src/mathutils.c
2. Compile to object file: gcc -c src/mathutils.c -o obj/mathutils.o
3. Archive into static lib: ar rcs lib/libmathutils.a obj/mathutils.o
4. Compile main: gcc app/main.c -Iinclude -Llib -lmathutils -o main
5. Wire all steps into Makefile targets
```

### Expected Output

The following commands must all succeed without errors:

```bash
make        # build succeeds
make run    # program runs, reads input, prints results
make clean  # removes obj/, lib/, and the executable
make        # rebuilds successfully from scratch
```

### Submission

```
Exercise_1/
├── include/
│   └── mathutils.h    (required)
├── src/
│   └── mathutils.c    (required)
├── app/
│   └── main.c         (required)
└── Makefile           (required — targets: all, clean, run)
```

---

## Exercise_2 [build]

### Problem Statement

Create a shared library `libstringutils.so` in C on Linux and dynamically link it with an executable. The library must provide the following functions:

- Convert a string to uppercase in-place
- Count the number of characters in a string (excluding the null terminator)
- Reverse a string in-place

Write a main program that accepts a string from a command line argument (`argv[1]`), calls all three library functions on the input string, and prints the result of each function to the terminal.

### Design Hints

```c
/* include/stringutils.h */
void str_to_upper(char *s);
int  str_count(const char *s);
void str_reverse(char *s);
```

### Suggested Approach

```
1. Implement the three functions in src/stringutils.c
2. Compile with -fPIC: gcc -fPIC -c src/stringutils.c -o build/stringutils.o
3. Link as shared lib: gcc -shared -o bin/libstringutils.so build/stringutils.o
4. Compile main: gcc main.c -Iinclude -Lbin -lstringutils -o bin/main
5. Verify: ldd bin/main — libstringutils.so must appear
```

### Expected Output

```bash
make        # build succeeds
make run    # prints results of all 3 functions on a sample string
make clean  # removes build/ and bin/
```

Additional verification:

```bash
ldd bin/<executable>   # libstringutils.so must appear in the dependency list
```

### Submission

```
Exercise_2/
├── include/
│   └── stringutils.h    (required)
├── src/
│   └── stringutils.c    (required)
├── build/               (created by Makefile)
├── bin/                 (created by Makefile)
└── Makefile             (required — targets: all, clean, run)
```

---

## Exercise_3 [build]

### Problem Statement

Build a Linux project that uses both a static library and a shared library, managed by a single Makefile with multiple targets.

**Static library — `libcalc.a`:** provides basic arithmetic for floating-point numbers: addition, subtraction, multiplication, and division (must handle division by zero).

**Shared library — `liblogger.so`:** provides logging functions: write a log entry to `app.log`, print the current timestamp in `YYYY-MM-DD HH:MM:SS` format, and write an error log entry prefixed with `[ERROR]`.

**Main program** must use both libraries: perform several arithmetic operations using `libcalc.a`, log results and any errors to `app.log` using `liblogger.so`, and print a summary to the terminal.

### Design Hints

```c
/* include/calc.h */
float calc_add(float a, float b);
float calc_sub(float a, float b);
float calc_mul(float a, float b);
float calc_div(float a, float b);   /* returns NAN on division by zero */

/* include/logger.h */
void log_write(const char *msg);
void log_timestamp(void);
void log_error(const char *msg);
```

### Suggested Approach

```
1. Build libcalc.a  — compile src/calc.c → obj/calc.o → ar rcs
2. Build liblogger.so — compile src/logger.c with -fPIC → -shared
3. Compile main linking both: -lcalc (static) and -llogger (shared)
4. Use $(wildcard src/*.c) and pattern rule %.o: %.c in Makefile
5. Verify: ldd shows liblogger.so but NOT libcalc (it is baked in)
```

### Expected Output

```bash
make static    # produces lib/libcalc.a
make shared    # produces lib/liblogger.so
make           # full build succeeds
make run       # prints results to terminal, creates app.log
make clean     # removes obj/, lib/, bin/
make rebuild   # clean + full rebuild succeeds
```

Additional verification:

```bash
cat app.log           # contains log entries with timestamps
ldd bin/<executable>  # liblogger.so is listed; libcalc is not (static)
nm  lib/libcalc.a     # symbols visible: add, sub, mul, div
```

### Submission

```
Exercise_3/
├── include/
│   ├── calc.h        (required)
│   └── logger.h      (required)
├── src/
│   ├── calc.c        (required)
│   ├── logger.c      (required)
│   └── main.c        (required)
└── Makefile          (required — targets: all, libs, static, shared, run, clean, rebuild)
```

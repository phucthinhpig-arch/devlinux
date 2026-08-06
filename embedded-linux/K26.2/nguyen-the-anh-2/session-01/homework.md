# Assignment — session-01
**Deadline: 2026-08-09 23:59:00**

---

## Exercise_1 [review-only]

### Problem Statement

Create a shell script file `info.sh` that prints the following information when executed:

```
Hello, <current logged-in username>!
Your home directory is: <home directory>
Your shell is: <current shell>
Current directory: <current directory>
```

**Requirements:** no hardcoding — must use system variables only.

### Submission

```
Exercise_1/
└── info.sh    (required)
```

---

## Exercise_2 [review-only]

### Problem Statement

Create a shell script file `greet.sh` that accepts 2 parameters: name and age. When called correctly, print:

```
Hello, my name is <name> and I am <age> years old.
Total arguments received: <number of arguments>
```

If fewer than 2 parameters are provided, print usage instructions and exit with a non-zero exit code.

### Submission

```
Exercise_2/
└── greet.sh    (required)
```

---

## Exercise_3 [review-only]

### Problem Statement

Open a terminal and perform the following tasks. Record all commands and their output in a file `env.sh`:

- Print the value of `$PATH`, `$HOME`, `$USER`, `$SHELL`
- Count how many directories are in `$PATH` (hint: use `tr` and `wc`)
- Use `printenv` to list all environment variables and count how many there are

### Submission

```
Exercise_3/
└── env.sh    (required — contains all commands used)
```

---

## Exercise_4 [review-only]

### Problem Statement

Follow each step below and observe the result. Record your answers in `answers.md`.

```bash
# Step 1 — create a regular variable
MY_NAME="Quan"
echo $MY_NAME        # visible?

# Step 2 — open a child shell and check
bash
echo $MY_NAME        # visible? why?
exit

# Step 3 — export the variable
export MY_NAME="Quan"
bash
echo $MY_NAME        # visible? why?
exit

# Step 4 — modify variable inside child shell
bash
MY_NAME="Alice"
echo $MY_NAME        # what do you see inside child shell?
exit
echo $MY_NAME        # back in parent shell — what do you see? why?
```

For each step, explain **why** the result is different. Your answer must cover:

- The difference between a regular variable and an exported variable
- Why changes made inside a child shell do not affect the parent shell

### Submission

```
Exercise_4/
└── answers.md    (required — explain the result at each step)
```

cmake_minimum_required(VERSION 3.10)
project(hw_lab1 C)

# Force strict C11 compiler flags per requirements
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -pedantic -Werror)

# --- Module 1: CRC Calculation Library ---
add_library(crc_lib STATIC src/crc_calc.c)
# PUBLIC allows any target linking to crc_lib to automatically include this directory
target_include_directories(crc_lib PUBLIC include)

# --- Module 2: Ring Buffer Library ---
add_library(ring_buf_lib STATIC src/ring_buffer.c)
target_include_directories(ring_buf_lib PUBLIC include)

# --- Module 3: Main Executable ---
add_executable(hw_lab1 src/main.c)
# Link the executable with the static libraries using PRIVATE scope
target_link_libraries(hw_lab1 PRIVATE crc_lib ring_buf_lib)
cmake_minimum_required(VERSION 3.14)
project(Exercise_2_TDD C)

# Enforce strict C11 flags
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -pedantic -Werror)

# Fetch Unity Testing Framework from GitHub
include(FetchContent)
FetchContent_Declare(
  unity
  GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(unity)

# Create the test executable
add_executable(test_sensor test_sensor.c sensor.c)

# Link Unity to the test executable
target_link_libraries(test_sensor PRIVATE unity)

# Register the executable with CTest
include(CTest)
add_test(NAME test_sensor COMMAND test_sensor)
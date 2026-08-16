cmake_minimum_required(VERSION 3.14)
project(Exercise_3_Mock C)

# Enforce strict C11 compiler flags
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -pedantic -Werror)

include(FetchContent)

# 1. Fetch Unity Testing Framework
FetchContent_Declare(
  unity
  GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(unity)

# 2. Fetch Fake Function Framework (FFF)
FetchContent_Declare(
  fff
  GIT_REPOSITORY https://github.com/meekrosoft/fff.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(fff)

# 3. Create the test executable (SUT + Test File, NO hardware HAL source)
add_executable(test_handler test_handler.c command_handler.c)

# 4. Link Unity and include FFF headers
target_include_directories(test_handler PRIVATE ${fff_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(test_handler PRIVATE unity)

# 5. Register with CTest
include(CTest)
add_test(NAME test_handler COMMAND test_handler)
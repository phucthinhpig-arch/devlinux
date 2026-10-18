cmake_minimum_required(VERSION 3.14)
project(iot_monitor VERSION 1.0.0 LANGUAGES C)

option(ENABLE_COVERAGE "Enable coverage reporting" OFF)
if(ENABLE_COVERAGE)
    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
endif()

# set(CMAKE_C_STANDARD 11)set(CMAKE_C_STANDARD 11)311
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SANITIZER "none" CACHE STRING "Sanitizer (none|asan|ubsan|asan+ubsan)")

find_program(CLANG_TIDY_EXE NAMES clang-tidy)
if(CLANG_TIDY_EXE)
    set(CMAKE_C_CLANG_TIDY ${CLANG_TIDY_EXE} --header-filter=${CMAKE_SOURCE_DIR}/components/.*)
    message(STATUS "[Quality Gate] clang-tidy: ENABLED")
endif()

find_program(CPPCHECK_EXE NAMES cppcheck)
if(CPPCHECK_EXE)
    add_custom_target(cppcheck
        COMMAND ${CPPCHECK_EXE}
            --enable=all --inconclusive --std=c11
            --suppress=missingIncludeSystem
            --suppress=unusedFunction
            --project=${CMAKE_BINARY_DIR}/compile_commands.json
            --error-exitcode=1
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "[Quality Gate] Running cppcheck..."
    )
endif()

add_compile_options(-Wall -Wextra -pedantic -Werror -std=c11 -D_POSIX_C_SOURCE=200809L)

if(SANITIZER STREQUAL "asan+ubsan")
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
endif()

add_subdirectory(components)
add_subdirectory(app)

enable_testing()
add_subdirectory(test)

add_executable(iot_app src/main.c)
target_link_libraries(iot_app PRIVATE app)
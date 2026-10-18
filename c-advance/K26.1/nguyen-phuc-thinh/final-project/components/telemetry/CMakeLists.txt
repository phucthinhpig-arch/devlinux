add_library(telemetry STATIC src/telemetry.c)
target_include_directories(telemetry PUBLIC include)
target_link_libraries(telemetry PUBLIC common)
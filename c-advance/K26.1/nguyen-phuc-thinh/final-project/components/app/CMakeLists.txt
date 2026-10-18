add_library(app STATIC src/app_core.c)
target_include_directories(app PUBLIC include)
target_link_libraries(app PUBLIC common drivers data telemetry)
add_library(common STATIC src/logger.c)
target_include_directories(common PUBLIC include)
target_link_libraries(common PRIVATE hal)
add_library(hal STATIC src/hal_sim.c src/hal_timer.c)
target_include_directories(hal PUBLIC include)
target_link_libraries(hal PUBLIC common)
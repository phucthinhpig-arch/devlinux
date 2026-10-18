include(FetchContent)
FetchContent_Declare(unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG        v2.6.0)
FetchContent_MakeAvailable(unity)

# Vô hiệu hóa clang-tidy cho mã nguồn của Unity để tránh cảnh báo rác
set_target_properties(unity PROPERTIES C_CLANG_TIDY "")

# Hàm hỗ trợ tạo bài test nhanh gọn
function(add_iot_test TEST_NAME SOURCE_FILE)
    add_executable(${TEST_NAME} ${SOURCE_FILE})
    target_link_libraries(${TEST_NAME} PRIVATE ${ARGN} unity)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endfunction()

# Khai báo bài test cho HAL
add_iot_test(test_hal test_hal_sim.c hal)
add_iot_test(test_logger test_logger.c common hal)
add_iot_test(test_sensors test_sensor_drv.c drivers)
add_iot_test(test_data test_data_structures.c data)
add_iot_test(test_app test_app_core.c app)
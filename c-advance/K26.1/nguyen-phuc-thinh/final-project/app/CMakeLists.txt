add_executable(iot_monitor main.c)
# Tạm thời chỉ link với hal, các module khác sẽ thêm vào ở các Task sau
target_link_libraries(iot_monitor PRIVATE hal)
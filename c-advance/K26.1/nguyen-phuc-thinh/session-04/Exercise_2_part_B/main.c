/*
 * Exercise_2 – Part B
 *
 * Phân tích bố cục bộ nhớ của chương trình:
 *
 * 1. Mảng `packet_pool[5]` và `packet_in_use[5]` đều là biến static
 *    không có khởi tạo cụ thể. Theo chuẩn C, các biến static mặc định
 *    được zero-initialized và được đặt trong section `.bss`.
 *    `.bss` là nơi dành cho dữ liệu tĩnh chưa khởi tạo, giúp tiết kiệm
 *    dung lượng file vì chỉ cần khai báo kích thước, không cần lưu giá trị.
 *
 * 2. Nếu ta viết `static network_packet_t packet_pool[5] = {0};`
 *    thì vẫn nằm trong `.bss`. Chuẩn C quy định rằng khởi tạo bằng 0
 *    tương đương với không khởi tạo (zero-init mặc định). Vì vậy,
 *    không có chuyện chuyển sang `.data`. `.data` chỉ dành cho biến
 *    static có giá trị khởi tạo khác 0.
 *
 * 3. Các hàm `packet_alloc()` và `packet_free()` sau khi biên dịch
 *    thành mã máy sẽ nằm trong section `.text`. Trên vi điều khiển thực tế:
 *    - `.text` → nằm trong Flash (64KB), nơi lưu code chương trình.
 *    - `.bss` và `.data` → nằm trong RAM (16KB), nơi lưu dữ liệu runtime.
 *    Đây là cách phân chia kinh điển trong hệ thống nhúng: code ở Flash,
 *    dữ liệu ở RAM.
 *
 * 4. Nếu tăng `POOL_SIZE` từ 5 lên 50:
 *    - Mỗi `network_packet_t` có kích thước 68 bytes.
 *    - Tổng kích thước pool = 50 × 68 = 3400 bytes.
 *    - Mảng `packet_in_use[50]` = 50 × sizeof(bool) = 50 bytes.
 *    - Tổng cộng `.bss` tăng thêm khoảng 3450 bytes.
 *    Điều này cho thấy `.bss` sẽ phình to theo số lượng phần tử pool,
 *    còn `.text` và `.data` không đổi.
 *
 * Tóm lại: chương trình của chúng ta sử dụng `.bss` để quản lý pool
 * một cách an toàn, không gây phân mảnh, và đảm bảo thời gian cấp phát
 * O(1). Đây chính là lý do tại sao object pool là giải pháp chuẩn trong
 * hệ thống nhúng an toàn.
 */

# TEST CASES — Project 1: IoT Gateway SmartHome (bản dành cho học viên)

Bắt buộc **tự chạy đủ** các test case dưới đây trên phần cứng thật, rồi ghi kết quả vào `docs/test_report.md` (theo `TEST_REPORT_TEMPLATE.md`).

**Cách ghi báo cáo:** với mỗi case, mô tả **cụ thể và trung thực** những gì em quan sát được trong lần chạy thực tế — không diễn giải chung chung kiểu "hoạt động đúng" hay "OK". Hãy ghi số liệu/nội dung thật (tên SSID thực tế, địa chỉ IP thực tế, giờ:phút:giây thực tế, nội dung text thật hiển thị trên màn hình, log thật...). Báo cáo càng cụ thể, càng phản ánh đúng em đã thực sự làm, càng dễ được đánh giá công bằng.

| Test ID | Requirement ID | Mô tả | Các bước thực hiện |
|---|---|---|---|
| TC-P1-01 | P1-M1 | Giữ nút ≥5s vào Soft AP | Nhấn giữ nút Smart Config đủ 5 giây |
| TC-P1-02 | P1-M1 | Nhấn ngắn (<5s) không vào Soft AP | Nhấn nhả nút trong 1-2 giây |
| TC-P1-03 | P1-M2, P1-M8 | LED đúng 3 trạng thái | Quan sát LED lúc: đang kết nối wifi / đã kết nối / mất kết nối |
| TC-P1-04 | P1-M3 | Điều khiển từng relay độc lập | Gửi lệnh ON/OFF cho từng relay 1-4 qua ioctl/webserver |
| TC-P1-05 | P1-M4, P1-M5 | Cấu hình wifi qua Soft AP thành công | Vào Soft AP, kết nối điện thoại, nhập đúng SSID/password wifi nhà qua form web, bấm OK |
| TC-P1-06 | P1-M4 | Sai password wifi khi cấu hình | Lặp lại TC-P1-05 nhưng nhập sai password |
| TC-P1-07 | P1-M5 | Điều khiển 4 relay qua trang web (Station mode) | Sau khi đã kết nối wifi, vào trang điều khiển, bấm ON/OFF từng nút |
| TC-P1-08 | P1-M6 | Tự kết nối lại sau khi khởi động lại | Sau khi đã cấu hình wifi thành công, khởi động lại gateway (tắt/mở nguồn) |
| TC-P1-09 | P1-M7 | OLED hiển thị đúng — chế độ Station | Sau khi kết nối wifi thành công, đọc màn hình OLED |
| TC-P1-10 | P1-M7 | OLED hiển thị đúng — chế độ Soft AP | Khi đang ở Soft AP, đọc màn hình OLED |
| TC-P1-11 | P1-M9 | systemd tự khởi động & tự restart | `systemctl status <service>` sau khi reboot; sau đó `kill -9` tiến trình |

**Về bằng chứng:** ảnh/video (nếu em có chụp/quay thêm) chỉ để mentor xem đối chứng khi cần, đặt trong `docs/demo/` — hệ thống chấm điểm tự động không xem ảnh/video, chỉ đọc phần mô tả text em viết trong `docs/test_report.md`.

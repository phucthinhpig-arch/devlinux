# TEST CASES — Project 2: Smart Weather Alarm Clock (bản dành cho học viên)

Bắt buộc **tự chạy đủ** các test case dưới đây trên phần cứng thật, rồi ghi kết quả vào `docs/test_report.md`.

**Cách ghi báo cáo:** mô tả **cụ thể và trung thực** những gì em quan sát được — không diễn giải chung chung. Ghi số liệu/nội dung thật (giờ:phút:giây thực tế tại từng mốc thời gian, nội dung màn hình thật, giờ đặt báo thức thật, log thật...).

| Test ID | Requirement ID | Mô tả | Các bước thực hiện |
|---|---|---|---|
| TC-P2-01 | P2-M1 | Vào chế độ Smart Config (Soft AP) | Giữ nút ≥5s |
| TC-P2-02 | P2-M2 | Điều khiển buzzer | Kích hoạt buzzer từ chương trình (qua lệnh test riêng nếu có) |
| TC-P2-03 | P2-M3 | Đồng bộ giờ qua NTP | Sau khi kết nối wifi lần đầu, so giờ hiển thị với giờ thực tế |
| TC-P2-04 | P2-M4 | Giây nhảy đúng, không trôi | Quan sát màn hình đồng hồ tại 1 thời điểm T0, rồi quan sát lại sau 5 phút, so với đồng hồ chuẩn |
| TC-P2-05 | P2-M5 | Màn hình thời tiết refresh đúng nhịp | Chuyển sang màn hình thời tiết, đứng lại quan sát ≥5 phút |
| TC-P2-06 | P2-M5 | Mất kết nối mock weather server | Tắt mock server trong lúc đang ở màn hình thời tiết |
| TC-P2-07 | P2-M6 | Nhấn nút chuyển màn hình | Nhấn ngắn khi đang ở màn hình đồng hồ, rồi lại nhấn tiếp |
| TC-P2-08 | P2-M7 | Đặt báo thức qua web, lưu đè | Đặt báo thức lần 1, sau đó đặt lại lần 2 giờ khác |
| TC-P2-09 | P2-M8 | Buzzer kêu đúng giờ báo thức | Đặt báo thức gần với giờ hiện tại (1-2 phút sau), chờ tới giờ |
| TC-P2-10 | P2-M9 | Tắt buzzer bằng nút, quay lại đúng luồng | Khi buzzer đang kêu, nhấn ngắn nút, sau đó nhấn nút thêm 1 lần nữa |
| TC-P2-11 | — (edge case) | Khởi động lại giữ nguyên cấu hình | Khởi động lại đồng hồ sau khi đã cấu hình wifi + báo thức |

**Về bằng chứng:** ảnh/video (nếu có) chỉ để mentor xem đối chứng, đặt trong `docs/demo/` — hệ thống chấm điểm tự động không xem ảnh/video, chỉ đọc phần mô tả text em viết trong `docs/test_report.md`.

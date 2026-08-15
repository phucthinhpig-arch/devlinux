# DESIGN.md — Project 1: IoT Gateway SmartHome

**Học viên:** ___________ | **Ngày nộp:** ___________

> File này nộp **trước khi mở PR code**. Mục tiêu: trình bày rõ kiến trúc trước khi bắt tay viết, để mentor/reviewer phát hiện sớm các thiết kế sai hướng.
> Điền đầy đủ các mục bên dưới. Không cần văn hoa — càng cụ thể, càng dễ review.

---

## 1. Kiến trúc tổng thể

Vẽ sơ đồ khối (có thể dùng ASCII art hoặc mô tả bằng gạch đầu dòng) thể hiện:
- Các tiến trình/thread chính trong hệ thống (ví dụ: main process, thread đọc nút nhấn, thread điều khiển LED, webserver...).
- Cách chúng giao tiếp với nhau (biến chia sẻ + mutex? pipe? file?).

```
(vẽ sơ đồ khối ở đây)
```

## 2. Danh sách Process/Thread (bắt buộc, liệt kê từng cái cụ thể)

| Tên | Loại (process/thread) | Vai trò | Tạo lúc nào | Kết thúc lúc nào | Giao tiếp với ai — qua kênh gì |
|---|---|---|---|---|---|
| main | process | | khởi động chương trình | | |
| btn_thread | thread | đọc sự kiện GPIO interrupt nút nhấn | | | |
| led_thread | thread | điều khiển LED theo trạng thái | | | |
| webserver_thread | thread | phục vụ HTTP (config wifi + điều khiển relay) | | | |
| (thêm dòng nếu có) | | | | | |

Với mỗi thread: nêu rõ dữ liệu nào nó đọc/ghi mà thread khác cũng đụng vào (để đối chiếu với mục 6 — Đồng bộ hoá).

## 3. Resource Mapping — GPIO / I2C

| Thiết bị | GPIO pin (dự kiến) | Cách khai báo | Ghi chú |
|---|---|---|---|
| Nút nhấn Smart Config | | Device Tree overlay | |
| LED trạng thái | | Device Tree overlay | |
| Relay 1 | | Device Tree overlay | |
| Relay 2 | | Device Tree overlay | |
| Relay 3 | | Device Tree overlay | |
| Relay 4 | | Device Tree overlay | |
| OLED SSD1306 | (địa chỉ I2C) | | |

## 4. Thiết kế từng driver char device

Với mỗi driver (nút nhấn, LED, relay), mô tả ngắn gọn:
- `open/read/write/ioctl` sẽ làm gì.
- Với driver nút nhấn: cơ chế đo thời gian giữ ≥5 giây — xử lý ở tầng driver hay userspace? Dựa vào cạnh lên/xuống nào?

```
(mô tả ở đây)
```

## 5. Luồng Smart Config (Soft AP)

Mô tả từng bước theo đúng trình tự thực tế sẽ chạy:
1. ...
2. ...
3. ...

Cách chuyển đổi `hostapd` ↔ `wpa_supplicant`: ...

Xử lý khi kết nối wifi thất bại (sai password): ...

## 6. Thiết kế Webserver

- Giao thức: HTTP thô qua socket TCP, hay dùng thư viện nào khác? (nêu rõ)
- Route/endpoint dự kiến (ví dụ `GET /`, `POST /wifi-config`, `POST /relay/<id>`).
- Cách webserver và driver relay giao tiếp với nhau (gọi trực tiếp? qua hàng đợi?).

## 7. Đồng bộ hoá & Race Condition

Liệt kê các điểm dữ liệu dùng chung giữa nhiều thread/tiến trình (VD: trạng thái LED, trạng thái wifi, trạng thái relay) và cách đồng bộ (mutex nào bảo vệ cái gì).

## 8. Edge Case / Failure Handling dự kiến

| Tình huống | Cách xử lý dự kiến |
|---|---|
| Sai password wifi khi Smart Config | |
| Mất điện giữa lúc đang ghi file cấu hình wifi | |
| Webserver nhận request không hợp lệ | |
| Nhấn giữ nút trong lúc relay đang bật | |
| (thêm nếu có) | |

## 9. Bảng đối chiếu Requirement Coverage

Đánh dấu mỗi Must-have đã được đề cập ở mục nào trong thiết kế trên.

| Requirement ID | Mô tả ngắn | Đề cập ở mục | Ghi chú |
|---|---|---|---|
| P1-M1 | Driver nút nhấn, GPIO interrupt, giữ ≥5s | | |
| P1-M2 | Driver LED 3 trạng thái | | |
| P1-M3 | Driver 4 relay | | |
| P1-M4 | Chuyển đổi Soft AP ↔ Station | | |
| P1-M5 | Webserver (config wifi + điều khiển relay) | | |
| P1-M6 | Lưu wifi, tự kết nối lại | | |
| P1-M7 | OLED hiển thị đúng 2 trạng thái | | |
| P1-M8 | LED phản ánh đúng trạng thái realtime | | |
| P1-M9 | systemd service | | |

## 10. Rủi ro em thấy khó nhất trong project này

```
(1-3 câu ngắn)
```

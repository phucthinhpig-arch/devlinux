# DESIGN.md — Project 2: Smart Weather Alarm Clock

**Học viên:** ___________ | **Ngày nộp:** ___________

> File này nộp **trước khi mở PR code**. Phần thiết kế nên tập trung vào chỗ dễ sai nhất: đa mục đích của nút nhấn (chuyển màn hình / Smart Config / tắt báo thức), 2 màn hình, và buzzer.

---

## 1. Kiến trúc tổng thể

Sơ đồ khối các tiến trình/thread chính (main, thread đọc nút, thread cập nhật giờ, thread lấy thời tiết, webserver, buzzer...).

```
(vẽ sơ đồ khối ở đây)
```

## 2. Danh sách Process/Thread (bắt buộc, liệt kê từng cái cụ thể)

| Tên | Loại (process/thread) | Vai trò | Tạo lúc nào | Kết thúc lúc nào | Giao tiếp với ai — qua kênh gì |
|---|---|---|---|---|---|
| main | process | | khởi động chương trình | | |
| btn_thread | thread | đọc + phân loại sự kiện nút nhấn | | | |
| clock_thread | thread | cập nhật giờ mỗi giây lên OLED | | | |
| weather_thread | thread | lấy dữ liệu thời tiết (lúc chuyển màn hình + mỗi 5 phút) | | | |
| webserver_thread | thread | cấu hình báo thức qua web | | | |
| alarm_thread | thread | so giờ hệ thống với giờ báo thức, kích buzzer | | | |
| (thêm dòng nếu có) | | | | | |

Với mỗi thread: nêu rõ dữ liệu nào nó đọc/ghi mà thread khác cũng đụng vào (để đối chiếu với mục 7 — Đồng bộ hoá).

## 3. Resource Mapping — GPIO / I2C

| Thiết bị | GPIO pin (dự kiến) | Cách khai báo | Ghi chú |
|---|---|---|---|
| Nút nhấn (đa chức năng) | | Device Tree overlay | |
| Buzzer | | Device Tree overlay | |
| OLED SSD1306 | (địa chỉ I2C) | | |

## 4. Logic phân loại sự kiện nút nhấn (quan trọng nhất của project)

Nút nhấn dùng chung cho 3 mục đích: chuyển màn hình / vào Smart Config / tắt báo thức. Mô tả rõ cách phân biệt theo ngữ cảnh:

| Ngữ cảnh hiện tại | Hành động nhấn | Kết quả |
|---|---|---|
| Buzzer đang kêu | nhấn ngắn | tắt báo thức |
| Buzzer không kêu, đang xem 1 trong 2 màn hình | nhấn ngắn | chuyển màn hình |
| Bất kỳ lúc nào | nhấn giữ ≥5s | vào Smart Config |

Logic này đặt ở tầng driver hay userspace? Driver báo sự kiện gì (raw press/release + timestamp, hay đã phân loại sẵn)?

```
(mô tả ở đây)
```

## 5. Thiết kế 2 màn hình hiển thị

**Màn hình đồng hồ:**
- Cơ chế cập nhật giây (timer nào dùng: `timerfd`, `clock_nanosleep`...?).
- Cách tránh trôi giờ theo thời gian.

**Màn hình thời tiết:**
- Khi nào gọi lấy dữ liệu mới (lúc chuyển vào + mỗi 5 phút khi đứng lại).
- Cơ chế non-blocking dùng gì để không chặn cập nhật giờ hay phản hồi nút nhấn.
- Xử lý khi mock weather server không phản hồi.

## 6. Thiết kế Webserver cấu hình báo thức

- Route/endpoint dự kiến.
- Định dạng lưu file cấu hình báo thức (chỉ 1 lịch, ghi đè).

## 7. Đồng bộ hoá & Race Condition

Liệt kê dữ liệu dùng chung giữa các thread (giờ hiện tại, dữ liệu thời tiết, trạng thái báo thức, màn hình đang hiển thị) và mutex/cơ chế bảo vệ tương ứng.

## 8. Edge Case / Failure Handling dự kiến

| Tình huống | Cách xử lý dự kiến |
|---|---|
| Mất kết nối khi đang ở màn hình thời tiết | |
| Đến giờ báo thức đúng lúc đang ở giữa Smart Config | |
| Nhấn giữ nút trong lúc buzzer đang kêu | |
| Mất điện giữa lúc lưu cấu hình báo thức | |
| (thêm nếu có) | |

## 9. Bảng đối chiếu Requirement Coverage

| Requirement ID | Mô tả ngắn | Đề cập ở mục | Ghi chú |
|---|---|---|---|
| P2-M1 | Driver nút nhấn + cơ chế Smart Config Soft AP | | |
| P2-M2 | Driver buzzer | | |
| P2-M3 | Đồng bộ giờ qua NTP | | |
| P2-M4 | Màn hình đồng hồ, giây nhảy đúng | | |
| P2-M5 | Màn hình thời tiết, refresh đúng nhịp | | |
| P2-M6 | Nhấn nút chuyển màn hình | | |
| P2-M7 | Webserver cấu hình báo thức | | |
| P2-M8 | Buzzer kêu đúng giờ | | |
| P2-M9 | Tắt buzzer bằng nút nhấn | | |

## 10. Rủi ro em thấy khó nhất trong project này

```
(1-3 câu ngắn)
```

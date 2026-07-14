# DESIGN.md — Project 4: Infrastructure Health Monitoring Agent

**Học viên:** ___________ | **Ngày nộp:** ___________

> File này nộp **trước khi mở PR code**. Trọng tâm review: **protocol agent↔server**, cách phát hiện OFFLINE, và thiết kế dashboard 2 phía (agent + server).

---

## 1. Kiến trúc tổng thể

Sơ đồ luồng: agent (thu thập → gửi → tự hiển thị dashboard riêng) và server (epoll nhận nhiều agent → ghi log → hiển thị dashboard tổng hợp → nhận lệnh `/command` từ admin).

```
(vẽ sơ đồ ở đây)
```

## 2. Danh sách Process/Thread (bắt buộc, liệt kê từng cái cụ thể — cho cả Agent và Server)

**Phía Agent:**

| Tên | Loại | Vai trò | Tạo lúc nào | Kết thúc lúc nào | Giao tiếp với ai — qua kênh gì |
|---|---|---|---|---|---|
| main | process | | khởi động agent | | |
| collector_thread | thread | đọc `/proc`, gửi dữ liệu định kỳ về server | | | |
| dashboard_thread | thread | tự vẽ dashboard CLI riêng của agent | | | |
| (thêm nếu có) | | | | | |

**Phía Server:**

| Tên | Loại | Vai trò | Tạo lúc nào | Kết thúc lúc nào | Giao tiếp với ai — qua kênh gì |
|---|---|---|---|---|---|
| main / epoll_loop | process (1 thread duy nhất hoặc số ít) | nhận kết nối agent, đọc dữ liệu, ghi log | khởi động server | | |
| dashboard_thread | thread | tự refresh dashboard tổng hợp mỗi 2s | | | |
| command_thread | thread (hoặc gộp vào epoll loop qua STDIN_FILENO) | đọc lệnh `/command` từ admin | | | |
| (thêm nếu có) | | | | | |

## 3. Định nghĩa Protocol Agent ↔ Server

| Chiều | Loại message | Định dạng | Ví dụ |
|---|---|---|---|
| Agent→Server | Dữ liệu định kỳ (CPU/RAM/DISK...) | | |
| Agent→Server | Heartbeat (PING) | | |
| Server→Agent | Config mới (`/config`) | | |
| Server→Agent | (khác nếu có) | | |

## 4. agent_id — sinh & lưu trữ

- Cách sinh agent_id lần đầu (thuật toán/format).
- File lưu agent_id đặt ở đâu, đọc lại khi nào.

## 5. Cơ chế đọc chỉ số hệ thống

- Đọc `/proc/stat` để tính % CPU: công thức tính cụ thể (đọc 2 lần cách nhau bao lâu, công thức delta).
- Đọc `/proc/meminfo` để tính % RAM.
- Đọc disk usage bằng cách nào (`statvfs`? đọc `/proc`?).

## 6. Cơ chế Keepalive 2 lớp & Phát hiện OFFLINE

- Chu kỳ heartbeat (agent gửi PING mỗi bao nhiêu giây).
- Cấu hình `SO_KEEPALIVE` (giá trị `TCP_KEEPIDLE`/`TCP_KEEPINTVL`/`TCP_KEEPCNT` dự kiến).
- Ngưỡng timeout để coi là OFFLINE — cách tránh false positive khi chỉ mất gói tạm thời.

## 7. Thiết kế Dashboard (Agent + Server)

- Hàm dùng chung vẽ thanh load + tô màu theo ngưỡng (input/output của hàm này).
- Cách server quản lý danh sách agent để in đúng số dòng (cấu trúc dữ liệu: map agent_id → trạng thái mới nhất).
- Cách dashboard tự refresh mà không xung đột với việc đọc lệnh `/command` từ stdin.

## 8. Thiết kế lệnh `/command` trên Server

| Lệnh | Cú pháp | Hành vi |
|---|---|---|
| `/config` | `/config <agent_id> <key>=<value>` | |
| `/history` | `/history <agent_id> [--last N]` | |
| `/help` | | |

## 9. Thiết kế Logging

- 3 loại log tách biệt: định dạng file/dòng log cho mỗi loại (dữ liệu định kỳ, ALERT, connect/disconnect).
- Log có thể parse lại được bằng cách nào (dùng cho lệnh `/history`).

## 10. Ngưỡng cảnh báo & màu sắc

Xác nhận đã áp dụng đúng bảng ngưỡng chuẩn (CPU 70/90%, RAM 75/90%, DISK 80/95%) hay có điều chỉnh gì khác — nêu rõ nếu có.

## 11. Edge Case / Failure Handling dự kiến

| Tình huống | Cách xử lý dự kiến |
|---|---|
| Agent mất kết nối tạm thời rồi kết nối lại | |
| Server nhận `/config` cho agent_id không tồn tại | |
| Agent restart, agent_id trùng với 1 agent đang ONLINE khác | |
| Chương trình chạy nhiều giờ — điểm nào dễ leak nhất | |
| (thêm nếu có) | |

## 12. Bảng đối chiếu Requirement Coverage

| Requirement ID | Mô tả ngắn | Đề cập ở mục | Ghi chú |
|---|---|---|---|
| P4-M1 | Agent đọc /proc, gửi định kỳ | | |
| P4-M2 | agent_id cố định, lưu file | | |
| P4-M3 | Agent tự hiển thị dashboard riêng | | |
| P4-M4 | Server dùng epoll | | |
| P4-M5 | 3 loại log tách biệt | | |
| P4-M6 | Keepalive 2 lớp | | |
| P4-M7 | Phát hiện OFFLINE | | |
| P4-M8 | systemd + không leak (Valgrind/Heaptrack) | | |
| P4-M9 | Dashboard server động theo số agent | | |
| P4-M10 | Lệnh `/config` | | |
| P4-M11 | Lệnh `/history` | | |

## 13. Rủi ro em thấy khó nhất trong project này

```
(1-3 câu ngắn)
```

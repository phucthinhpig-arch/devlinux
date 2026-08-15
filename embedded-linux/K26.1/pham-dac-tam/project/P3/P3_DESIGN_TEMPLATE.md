# DESIGN.md — Project 3: Multi-user CLI Chat Server/Client

**Học viên:** ___________ | **Ngày nộp:** ___________

> File này nộp **trước khi mở PR code**. Trọng tâm review ở đây là **protocol** và **cách xử lý epoll/non-blocking I/O** — đây là phần dễ sai nhất và khó sửa nhất nếu phát hiện muộn.

---

## 1. Kiến trúc tổng thể

Sơ đồ luồng xử lý của server (event loop epoll, các loại sự kiện được lắng nghe: `accept`, dữ liệu từ client, stdin admin nếu có...).

```
(vẽ sơ đồ ở đây)
```

## 2. Danh sách Process/Thread (bắt buộc, liệt kê từng cái cụ thể)

| Tên | Loại (process/thread) | Vai trò | Tạo lúc nào | Kết thúc lúc nào | Giao tiếp với ai — qua kênh gì |
|---|---|---|---|---|---|
| main / epoll_loop | process (1 thread duy nhất hoặc số ít) | vòng lặp epoll chính, xử lý accept + đọc/ghi client | khởi động server | | |
| (nếu có thread phụ nào, liệt kê ở đây — nêu rõ vì sao cần, vì bài yêu cầu hạn chế thread-per-client) | | | | | |

> Lưu ý: bài yêu cầu **không được** dùng 1 thread/1 client. Nếu thiết kế có thêm thread ngoài vòng lặp epoll chính, phải giải thích rõ lý do (ví dụ: thread ghi log riêng để không chặn epoll loop).

## 3. Định nghĩa Protocol (bắt buộc — đây là phần quan trọng nhất)

Chọn 1 kiểu và mô tả chi tiết:
- [ ] Text-based dạng dòng lệnh (giống IRC), kết thúc mỗi message bằng `\n`
- [ ] JSON-line (mỗi message là 1 dòng JSON)
- [ ] Khác: ___________

Liệt kê toàn bộ loại message client ↔ server, ví dụ:

| Chiều | Loại message | Định dạng | Ví dụ |
|---|---|---|---|
| C→S | Đăng nhập | | |
| C→S | Đăng ký | | |
| C→S | Gửi tin nhắn chat | | |
| S→C | Broadcast tin nhắn | | |
| S→C | Lịch sử tin nhắn (khi vừa join) | | |
| S→C | Thông báo lỗi đăng nhập | | |
| S→C | Danh sách user online (`/who`) | | |

## 4. Xử lý Non-blocking Socket & Partial Read/Write

- Mỗi client có buffer riêng như thế nào (struct gì, cấp phát ở đâu, giải phóng khi nào)?
- Cách xác định 1 message đã nhận đủ hay chưa (delimiter `\n`? length-prefix?).
- Cách xử lý khi `send()` không ghi hết dữ liệu trong 1 lần gọi.

```
(mô tả ở đây)
```

## 5. Quản lý State

- Cấu trúc dữ liệu lưu danh sách user đang online, danh sách kết nối (map fd → thông tin client).
- Cách lưu/đọc tài khoản (file gì, hash bằng thuật toán nào).
- Cách lưu lịch sử tin nhắn (append-only file? có dùng `flock`/`fcntl` không?).

## 6. Xử lý client ngắt kết nối đột ngột

Mô tả cách phát hiện (giá trị trả về của `read`/`recv` khi client đóng kết nối) và cách dọn dẹp tài nguyên (đóng fd, xoá khỏi epoll, giải phóng buffer, gỡ khỏi danh sách online) mà không làm crash server.

## 7. Edge Case / Failure Handling dự kiến

| Tình huống | Cách xử lý dự kiến |
|---|---|
| Client gửi message không đúng protocol | |
| Client kill đột ngột giữa lúc đang gõ dở | |
| Nhập sai password 3 lần liên tiếp | |
| File tài khoản/lịch sử bị khoá bởi tiến trình khác | |
| (thêm nếu có) | |

## 8. Bảng đối chiếu Requirement Coverage

| Requirement ID | Mô tả ngắn | Đề cập ở mục | Ghi chú |
|---|---|---|---|
| P3-M1 | Đăng ký/đăng nhập, mật khẩu hash | | |
| P3-M2 | Broadcast 1 phòng chung | | |
| P3-M3 | epoll, không thread-per-client | | |
| P3-M4 | Xử lý disconnect đột ngột | | |
| P3-M5 | Danh sách user online | | |
| P3-M6 | Xử lý sai username/password 3 lần | | |
| P3-M7 | Lưu & gửi lại lịch sử tin nhắn khi join | | |

## 9. Rủi ro em thấy khó nhất trong project này

```
(1-3 câu ngắn)
```

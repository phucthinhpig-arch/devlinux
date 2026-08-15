# TEST CASES — Project 3: Multi-user CLI Chat Server/Client (bản dành cho học viên)

Bắt buộc **tự chạy đủ** các test case dưới đây trên máy Linux của mình, ghi kết quả vào `docs/test_report.md`, kèm log terminal (copy/paste) cho từng case.

**Cách ghi báo cáo:** dán log terminal thật của lần chạy, không tóm tắt lại bằng lời — log thật là bằng chứng đáng tin nhất và khó giả mạo nhất.

| Test ID | Requirement ID | Mô tả | Các bước thực hiện |
|---|---|---|---|
| TC-P3-01 | P3-M1 | Đăng ký + đăng nhập | Đăng ký tài khoản mới, thoát, đăng nhập lại bằng đúng tài khoản đó |
| TC-P3-02 | P3-M2 | Broadcast tới toàn bộ user | Mở ≥3 client, đăng nhập cả 3, 1 client gửi tin nhắn |
| TC-P3-03 | P3-M3 | Không dùng thread-per-client | (Không chạy được bằng thao tác thủ công) — tự kiểm tra code của mình và ghi lại trong report |
| TC-P3-04 | P3-M4 | Client ngắt kết nối đột ngột | Mở 2 client, 1 client `kill -9` giữa chừng, lặp lại vài lần, kiểm tra `lsof`/`ps` trước và sau |
| TC-P3-05 | P3-M5 | Danh sách user online | Đăng nhập 3 user, gõ `/who` |
| TC-P3-06 | P3-M6 | Sai username/password | Nhập sai password 3 lần liên tiếp |
| TC-P3-07 | P3-M7 | Lịch sử tin nhắn khi join | Gửi vài tin nhắn từ user A, sau đó user B mới đăng nhập/join |
| TC-P3-08 | — (edge case) | Gửi message không đúng protocol | Dùng `nc`/telnet gửi dữ liệu tuỳ ý không đúng định dạng |
| TC-P3-09 | — (đồng thời) | Nhiều client cùng gửi tin nhắn đồng thời | ≥5 client gửi tin nhắn gần như cùng lúc (script tự động) |

**Về bằng chứng:** dán log terminal trực tiếp vào `docs/test_report.md` (không cần ảnh chụp màn hình terminal, copy text là đủ và tốt hơn).

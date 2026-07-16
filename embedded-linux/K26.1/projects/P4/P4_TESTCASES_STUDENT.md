# TEST CASES — Project 4: Infrastructure Health Monitoring Agent (bản dành cho học viên)

Bắt buộc **tự chạy đủ** các test case dưới đây, ghi kết quả vào `docs/test_report.md`, kèm log thật cho từng case.

**Cách ghi báo cáo:** dán log/output thật của lần chạy, không tóm tắt bằng lời. Với TC-P4-08, bắt buộc đính kèm báo cáo Valgrind/Heaptrack đầy đủ (log gốc, không chỉ 1 dòng kết luận).

| Test ID | Requirement ID | Mô tả | Các bước thực hiện |
|---|---|---|---|
| TC-P4-01 | P4-M1 | Agent đọc & gửi dữ liệu thật | Chạy 1 agent, quan sát dữ liệu server nhận được, đối chiếu `top`/`free`/`df` |
| TC-P4-02 | P4-M2 | agent_id bền vững qua restart | Chạy agent, ghi lại agent_id, restart agent |
| TC-P4-03 | P4-M3 | Dashboard riêng của agent | Quan sát output của agent khi đang chạy |
| TC-P4-04 | P4-M4 | Nhiều agent kết nối đồng thời | Chạy ≥3 agent cùng lúc kết nối 1 server |
| TC-P4-05 | P4-M5 | 3 loại log tách biệt | Chạy hệ thống 1 lúc, tạo 1 sự kiện ALERT (đẩy CPU cao), ngắt 1 agent |
| TC-P4-06 | P4-M6 | Keepalive 2 lớp | Kiểm tra socket bằng `ss -o` xem `SO_KEEPALIVE` có bật; theo dõi heartbeat |
| TC-P4-07 | P4-M7 | Phát hiện OFFLINE | `kill -9` 1 agent đang chạy |
| TC-P4-08 | P4-M8 | Không leak (Valgrind/Heaptrack) | Chạy server + agent liên tục dưới Valgrind/Heaptrack tối thiểu 30 phút |
| TC-P4-09 | P4-M9 | Dashboard server động theo agent | Thêm 1 agent mới giữa lúc server đang chạy, sau đó tắt 1 agent |
| TC-P4-10 | P4-M10 | Lệnh `/config` đẩy config | Gõ `/config <agent_id> interval=5` |
| TC-P4-11 | P4-M11 | Lệnh `/history` | Gõ `/history <agent_id> --last 5` |
| TC-P4-12 | — (edge case) | `/config` cho agent_id không tồn tại | Gõ `/config fake-id interval=5` |

**Về bằng chứng:** dán log/output trực tiếp vào `docs/test_report.md`.

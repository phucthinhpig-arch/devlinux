# QUY ĐỊNH CHUNG — 4 Project Cuối Khoá Embedded Linux (DevLinux)

Áp dụng chung cho cả 4 project. Xem đề bài chi tiết từng project trong file riêng: `DE_BAI_P1_IoT_Gateway.md`, `DE_BAI_P2_Weather_Clock.md`, `DE_BAI_P3_Chat_Server.md`, `DE_BAI_P4_Monitoring_Agent.md`.

**Thời lượng:** 4 tuần | **Độ khó:** Trung bình | **Nộp bài:** GitHub PR (hệ thống chấm điểm tự động `devlinux-reviewer`, ngưỡng ≥80 Pass / 60–79 Pass with revisions / <60 Fail)

Mỗi project mô phỏng một sản phẩm IoT/Embedded có thật ngoài thị trường, huy động một cụm kiến thức khác nhau trong syllabus.

## Phân loại
- **Project 1–2:** chạy trên thiết bị thật (Raspberry Pi Zero W) — bắt buộc viết driver, thao tác GPIO/I2C thật.
- **Project 3–4:** chỉ cần máy Linux (VM/container/máy thật) — không yêu cầu phần cứng, không viết driver, tập trung systems programming (process/thread/socket/epoll).

## Quy ước ID requirement
Mỗi Must-have/Nice-to-have được gán 1 ID cố định (`P1-M1`, `P1-N1`, `P2-M1`...) để dùng chung giữa đề bài, file `DESIGN.md` học viên nộp, và rubric chấm điểm tự động — tránh việc diễn giải yêu cầu lệch nhau giữa các bước review.

## Bằng chứng test — nguyên tắc chung cho cả 4 project

Reviewer tự động (Claude) **không tự chạy code, không xem ảnh, không xem video** — chỉ đọc text. Với mỗi project:
1. Học viên tự chạy toàn bộ Test Case bắt buộc (`TESTCASES_Px_student.md` trong repo học viên) trên máy/thiết bị của mình.
2. Ghi kết quả vào `docs/test_report.md` (theo `TEST_REPORT_TEMPLATE.md`) gồm **mô tả bằng chữ + log chương trình dán nguyên văn** cho từng case (log là output thật: terminal, `journalctl`, hoặc debug print học viên tự thêm vào code).
3. Hệ thống chỉ dùng **10 dòng đầu tiên** của mỗi khối log để chấm điểm — học viên phải tự đặt đoạn quan trọng nhất lên đầu mỗi khối.
4. Ảnh/video (nếu có, đặt trong `docs/demo/`) chỉ dành cho **mentor con người** xem đối chứng khi cần, Claude không đọc phần này.

Claude đối chiếu log/mô tả với chính source code (log có khớp với các câu lệnh in log thật trong code không) — log không khớp với bất kỳ đoạn code nào bị coi là bằng chứng không hợp lệ.

## Quy tắc đặt tên & cấu trúc chung
- `docs/test_report.md` là file **bắt buộc** với cả 4 project.
- File video/ảnh demo (Project 1–2) đặt trong `docs/demo/`; nếu là link (Youtube/Drive) thay vì file trực tiếp, ghi vào `docs/demo/demo_links.txt`, mỗi dòng 1 link kèm chú thích ngắn.
- Không đặt code driver lẫn trong thư mục `app/` hoặc ngược lại — tách rõ để reviewer phân biệt được phần kernel-space và user-space khi chấm mục "Đúng kỹ thuật bắt buộc".
- README phải có 1 mục "Cấu trúc thư mục" liệt kê ngắn gọn nơi đặt từng phần, kể cả khi đã theo đúng chuẩn (giúp reviewer đối chiếu nhanh, và mentor con người đọc PR dễ hơn).
- Cấu trúc thư mục chi tiết riêng từng project xem trong file đề bài tương ứng.

## Lộ trình chung (4 tuần)

| Tuần | Nội dung |
|---|---|
| 1 | Thiết kế kiến trúc, dựng khung project, thiết lập môi trường (driver skeleton / socket skeleton) |
| 2 | Cài đặt chức năng must-have chính |
| 3 | Hoàn thiện must-have, bắt đầu debug (GDB/Helgrind/Valgrind theo yêu cầu từng project) |
| 4 | Nice-to-have (tuỳ chọn), viết README, chạy đủ Test Case, chuẩn bị PR |

*(Xem gợi ý lộ trình chi tiết theo tuần, riêng cho từng project, trong mục "Gợi ý thiết kế cho học viên" ở mỗi file đề bài.)*

## Rubric chấm điểm chung

| Tiêu chí | Trọng số |
|---|---|
| Đúng chức năng (functional correctness) | 40% |
| Chất lượng code & xử lý lỗi | 25% |
| Áp dụng đúng kỹ thuật đã học (bắt buộc, không được né — vd: phải dùng epoll, phải viết driver, không dùng lib có sẵn) | 20% |
| Bằng chứng debug (GDB/Valgrind/Helgrind/Heaptrack) + tài liệu | 15% |

**Ngưỡng:** ≥80 Pass | 60–79 Pass with revisions | <60 Fail

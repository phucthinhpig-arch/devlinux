# Assignment — session-07
**Deadline: 2026-06-21 23:59:00**

---

## Exercise_1 [build]

### Problem Statement

Viết một chương trình C mô phỏng một **sensor daemon** chạy nền (giả lập, không cần đọc cảm biến thật) thực hiện một vòng lặp vô hạn, mỗi giây in ra một dòng log dạng:

```
[INFO] Sensor reading #<n>: temperature=<giá_trị>
```

(giá trị nhiệt độ có thể random hoặc tăng dần, không bắt buộc chính xác).

Chương trình phải xử lý các signal sau bằng cách thay đổi default action thông qua `signal()`:

- **SIGINT** (Ctrl+C): không cho thoát ngay. Khi nhận được, in ra dòng `[WARN] Received SIGINT, ignoring...` và tiếp tục chạy bình thường.
- **SIGTERM**: đây là signal "tắt máy đúng cách". Khi nhận được, in ra dòng `[INFO] Received SIGTERM, shutting down gracefully...`, sau đó kết thúc chương trình bằng `exit(0)`.
- **SIGUSR1**: dùng để giả lập lệnh "in báo cáo trạng thái" gửi tới daemon. Khi nhận được, in ra số lượng reading đã thực hiện được tính từ lúc chương trình khởi động, dạng: `[REPORT] Total readings so far: <n>`.

### Design Hints (optional)

```c
volatile sig_atomic_t reading_count = 0;

void handle_sigint(int sig);
void handle_sigterm(int sig);
void handle_sigusr1(int sig);
```

> Gợi ý: biến đếm số lần đọc nên được khai báo `volatile sig_atomic_t` vì nó được cập nhật ở main loop và đọc trong signal handler.

### Suggested Approach (optional)

```
1. Đăng ký 3 signal handler bằng signal() cho SIGINT, SIGTERM, SIGUSR1
2. Main loop: mỗi giây in 1 dòng log, tăng biến đếm reading_count, sleep(1)
3. Handler SIGINT: chỉ in cảnh báo, không làm gì khác
4. Handler SIGTERM: in log rồi exit(0)
5. Handler SIGUSR1: in tổng số reading hiện tại (đọc từ reading_count)
```

### Expected Output (optional)

Khi chạy chương trình, sau đó từ terminal khác gửi:

```
kill -USR1 <PID>
```

Chương trình đang chạy phải in thêm dòng `[REPORT] Total readings so far: <n>` mà **không bị dừng vòng lặp chính**.

Khi gửi `kill -INT <PID>` (Ctrl+C), chương trình in `[WARN] Received SIGINT, ignoring...` và **vẫn tiếp tục chạy**.

Khi gửi `kill -TERM <PID>` (hoặc `kill <PID>`), chương trình in dòng shutdown rồi thoát với exit code `0`.

### Submission

```
Exercise_1/
├── main.c        (required)
├── Makefile      (required — targets: all, clean)
└── *.h           (if any)
```

---

## Exercise_2 [build]

### Problem Statement

Viết chương trình C gồm **2 tiến trình cha-con** mô phỏng một **IoT gateway** điều khiển một **worker process** xử lý dữ liệu.

- Tiến trình cha (gateway):
  - Tạo tiến trình con bằng `fork()`.
  - In ra PID của con vừa tạo.
  - Sau khi tạo con, **block signal SIGUSR1** bằng `sigprocmask(SIG_BLOCK, ...)` trong vòng 5 giây (dùng `sleep(5)`) để mô phỏng "đang trong giai đoạn khởi tạo hệ thống, không muốn bị làm phiền bởi tín hiệu từ worker".
  - Trong 5 giây này, nếu tiến trình con gửi SIGUSR1 lên cho cha, tín hiệu phải bị giữ ở trạng thái **pending** (không được xử lý ngay).
  - Sau 5 giây, gọi `sigprocmask(SIG_UNBLOCK, ...)` để gỡ block. Lúc này nếu SIGUSR1 đang pending thì handler phải được gọi ngay.
  - Khi handler SIGUSR1 được gọi, in ra: `[GATEWAY] Worker reported READY signal received`.
  - Sau đó cha chờ con kết thúc bằng `wait()`, rồi in ra trạng thái thoát của con (mã exit code, dùng `WIFEXITED`/`WEXITSTATUS`).

- Tiến trình con (worker):
  - Ngủ 2 giây (mô phỏng thời gian khởi tạo worker).
  - Gửi signal **SIGUSR1** lên cho tiến trình cha bằng `kill(getppid(), SIGUSR1)` để báo "tôi đã sẵn sàng".
  - In ra dòng `[WORKER] Sent READY signal to gateway`.
  - Kết thúc bằng `exit(7)` (mã exit code tuỳ ý để cha kiểm tra).

### Design Hints (optional)

```c
void handle_sigusr1(int sig);

sigset_t block_set;
```

### Suggested Approach (optional)

```
1. Cha: thiết lập handler cho SIGUSR1 bằng signal()
2. Cha: fork()
3. Cha (sau fork): block SIGUSR1, sleep(5), unblock SIGUSR1
4. Cha: wait() lấy exit code của con, in ra kết quả
5. Con (sau fork): sleep(2), kill(getppid(), SIGUSR1), in log, exit(7)
```

### Expected Output (optional)

Output minh hoạ (thứ tự một số dòng có thể xen kẽ do tiến trình song song):

```
[GATEWAY] Worker PID = 12345
[WORKER] Sent READY signal to gateway
[GATEWAY] Worker reported READY signal received
[GATEWAY] Worker exited with code 7
```

Lưu ý quan trọng: vì cha block SIGUSR1 trong 5 giây đầu, mà con gửi tín hiệu ở giây thứ 2, nên dòng `[GATEWAY] Worker reported READY signal received` **chỉ được in ra sau khi cha unblock** (tức là khoảng giây thứ 5), không in ngay khi con gửi.

### Submission

```
Exercise_2/
├── main.c        (required)
├── Makefile      (required — targets: all, clean)
└── *.h           (if any)
```

---

## Exercise_3 [build]

### Problem Statement

Viết chương trình C mô phỏng tình huống **transaction logger** ghi log giao dịch (chỉ cần `printf` ra màn hình, không cần ghi file thật) — minh hoạ việc bảo vệ một đoạn code quan trọng khỏi bị gián đoạn bởi signal, đồng thời chứng minh hành vi "default action" của SIGINT khi không block.

Chương trình hoạt động theo 2 giai đoạn lặp lại liên tục (vòng lặp `for` hoặc `while` chạy 5 lần):

**Giai đoạn 1 — "Vùng an toàn" (critical section):**
- Trước khi vào vùng này, block signal **SIGINT** bằng `sigprocmask()`.
- In ra: `[SAFE] Writing transaction #<n> ...`
- Giả lập việc ghi log mất thời gian bằng `sleep(3)`.
- In ra: `[SAFE] Transaction #<n> committed.`
- Sau khi xong, unblock lại SIGINT bằng `sigprocmask()`.

**Giai đoạn 2 — "Vùng nghỉ" (idle):**
- In ra: `[IDLE] Waiting for next transaction...`
- `sleep(3)` — trong giai đoạn này SIGINT **không bị block**, nên nếu người dùng nhấn Ctrl+C lúc này, chương trình phải dừng theo **default action** (không cần viết handler riêng cho phần này).

Yêu cầu thêm: dùng tham số thứ 3 của `sigprocmask()` (**oldmask**) để lưu lại trạng thái mask trước khi block, và dùng nó để **restore** lại đúng trạng thái cũ thay vì unblock một cách "cứng" — đúng theo nguyên tắc đã học (không tự ý phá trạng thái mình không tạo ra).

### Design Hints (optional)

```c
sigset_t block_set, old_set;
```

### Suggested Approach (optional)

```
1. Khởi tạo sigset_t chứa SIGINT
2. Lặp 5 lần:
   a. sigprocmask(SIG_BLOCK, &block_set, &old_set)
   b. In log "SAFE", sleep(3), in log "committed"
   c. sigprocmask(SIG_SETMASK, &old_set, NULL)
   d. In log "IDLE", sleep(3)
3. Kết thúc chương trình bình thường (exit(0)) sau 5 lần lặp nếu không bị Ctrl+C giữa đường
```

### Expected Output (optional)

Nếu không nhấn Ctrl+C, chương trình lặp 5 vòng rồi tự kết thúc với exit code `0`, output dạng:

```
[SAFE] Writing transaction #1 ...
[SAFE] Transaction #1 committed.
[IDLE] Waiting for next transaction...
[SAFE] Writing transaction #2 ...
...
```

Nếu nhấn Ctrl+C **trong lúc đang ở giai đoạn [SAFE]**, chương trình **không bị dừng ngay** — phải chạy hết câu lệnh `sleep(3)` và in xong dòng `committed` rồi mới được xử lý SIGINT (vì đang bị block, signal ở trạng thái pending).

Nếu nhấn Ctrl+C trong lúc đang ở giai đoạn `[IDLE]`, chương trình phải **kết thúc ngay theo default action** của SIGINT.

### Submission

```
Exercise_3/
├── main.c        (required)
├── Makefile      (required — targets: all, clean)
└── *.h           (if any)
```

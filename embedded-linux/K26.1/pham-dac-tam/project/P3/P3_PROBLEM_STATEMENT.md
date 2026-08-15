# PROJECT 3: Multi-user CLI Chat Server/Client

**Thời lượng:** 4 tuần | **Độ khó:** Trung bình | **Nộp bài:** GitHub PR (hệ thống chấm điểm tự động `devlinux-reviewer`, ngưỡng ≥80 Pass / 60–79 Pass with revisions / <60 Fail)

**Thiết bị:** chỉ cần máy Linux (VM/container/máy thật), không cần phần cứng.

**Mô phỏng sản phẩm thật:** hệ thống chat dạng IRC/Slack rút gọn.

**Kiến thức áp dụng:** Socket TCP, POSIX pthread, File I/O (lưu tài khoản), băm mật khẩu, quản lý phiên đồng thời, thread synchronization (mutex).

> Quy ước ID requirement (`P3-M1`, `P3-N1`...) dùng chung giữa đề bài này, `DESIGN.md` học viên nộp, và rubric chấm điểm — xem thêm quy định chung ở `00_Chung/QUY_DINH_CHUNG.md`.

---

## Problem Statement
Xây dựng server chat CLI đa người dùng qua TCP, hỗ trợ đăng ký/đăng nhập và chat chung trong **1 phòng broadcast duy nhất**, toàn bộ xử lý đồng thời nhiều client bằng **1 thread phục vụ 1 client** (thread-per-client model) với synchronization sử dụng mutex cho shared state.

## Functional Requirements

**Must-have:**
- **P3-M1.** Đăng ký/đăng nhập tài khoản, mật khẩu lưu ở dạng **hash** (không lưu plaintext), lưu trong file.
- **P3-M2.** Sau khi đăng nhập, mọi tin nhắn gửi lên đều **broadcast tới toàn bộ user đang online** (1 phòng chung duy nhất, không cần tạo/quản lý nhiều phòng).
- **P3-M3.** Server dùng **thread-per-client model**: main thread chấp nhận kết nối, mỗi client được xử lý bởi 1 pthread riêng với blocking I/O. Dùng **mutex để synchronization** khi access shared state (clients array, broadcast).
- **P3-M4.** Xử lý client ngắt kết nối đột ngột (crash/kill) mà không làm sập server hay rò rỉ tài nguyên.
- **P3-M5.** Danh sách user đang online.
- **P3-M6.** Nhập sai username/password: server trả về lỗi rõ ràng, **không** cho biết cụ thể là sai username hay sai password (tránh lộ thông tin tài khoản nào tồn tại), client cho phép nhập lại tối đa 3 lần trước khi tự ngắt kết nối.
- **P3-M7.** Server lưu **toàn bộ lịch sử tin nhắn** ra file; khi một user đăng nhập/join vào phòng chung, server gửi lại **toàn bộ lịch sử tin nhắn** đã có từ trước cho client đó xem trước khi vào chat realtime.

**Nice-to-have:**
- **P3-N1.** Nhiều phòng chat (tạo/tham gia/rời phòng), broadcast chỉ trong phạm vi phòng thay vì toàn server.
- **P3-N2.** Tin nhắn riêng (private message) giữa 2 user.
- **P3-N3.** Giới hạn số lượng tin nhắn lịch sử hiển thị (ví dụ chỉ N tin gần nhất) thay vì toàn bộ, kèm lệnh `/history` xem thêm.
- **P3-N4.** Lệnh admin (kick user).
- **P3-N5.** Giới hạn rate gửi tin nhắn để chống spam.

## Design Hints (lưu ý kỹ thuật, tránh bẫy thường gặp)
- Thiết kế protocol đơn giản dạng text-based (giống IRC) hoặc JSON-line, phải document rõ ràng trong README.
- **Thread-per-client architecture:** Main thread chỉ lắng nghe & accept kết nối. Mỗi client được xử lý bởi 1 pthread riêng gọi `recv()` và `send()` blocking (đơn giản hơn non-blocking socket).
- **Shared state synchronization:** Dùng `pthread_mutex_t` để bảo vệ:
  - Mảng `clients[]` (thêm/xóa client, access username & fd)
  - Broadcast: khi gửi tin nhắn, lock mutex trước khi duyệt `clients[]`
  - Message history file: dùng `flock()` khi đọc/ghi
- Lịch sử tin nhắn nên ghi ra file (append-only) mỗi khi có tin nhắn mới, và đọc lại toàn bộ file này để gửi cho client vừa đăng nhập — chú ý dùng `flock()` để ngăn race condition khi nhiều thread ghi đồng thời.
- Gửi lịch sử tin nhắn cho client mới nên tách biệt rõ với luồng broadcast realtime để tránh lẫn lộn thứ tự (client cần nhận đủ và đúng thứ tự lịch sử trước khi nhận tin nhắn mới).

## Gợi ý thiết kế cho học viên

**Gợi ý kiến trúc tổng thể (Thread-per-client):** 
- 1 main thread lắng nghe & accept kết nối mới vào loop: `accept() → pthread_create() → client thread xử lý`
- 1 mảng/map `clients[] → struct client_info` (fd, username, authenticated, mutex-protected)
- Mỗi client thread gọi `recv()` blocking tới khi nhận đủ 1 message (delimiter `\n`), xử lý message, gọi `broadcast_to_all()` nếu cần gửi
- Broadcast duyệt qua `clients[]` với lock mutex, gửi `send()` tới từng authenticated client
- Client thread clean exit khi client disconnect, cập nhật `clients[]` với lock mutex

### Chi tiết kỹ thuật — Thread-per-client & Blocking Socket

**Khung kiến trúc gợi ý:**
```c
typedef struct {
    int fd;
    char username[64];
    int authenticated;
} client_t;

client_t clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void *client_handler(void *arg) {
    client_t *client = (client_t *)arg;
    char buf[4096];
    int n;
    
    while ((n = recv(client->fd, buf, sizeof(buf), 0)) > 0) {
        // Parse message (delimited by \n)
        buf[n] = '\0';
        
        // Process command (login, register, msg, etc.)
        process_input(client, buf);
        
        // If message, broadcast to all authenticated clients
        if (is_chat_message(buf)) {
            pthread_mutex_lock(&clients_mutex);
            broadcast_message(client->username, message_text);
            pthread_mutex_unlock(&clients_mutex);
        }
    }
    
    // Client disconnected
    pthread_mutex_lock(&clients_mutex);
    client->fd = -1;
    pthread_mutex_unlock(&clients_mutex);
    return NULL;
}

int main() {
    int listen_fd = socket(...);
    bind(...);
    listen(...);
    
    while (1) {
        int client_fd = accept(listen_fd, ...);
        pthread_t thread;
        // Find empty slot in clients[] and create thread
        pthread_create(&thread, NULL, client_handler, &clients[i]);
        pthread_detach(thread);
    }
}
```

**Cách xử lý message với blocking socket:**
- Mỗi lần `recv()` có thể nhận chưa đủ 1 message hoặc nhiều hơn 1 message
- Nếu dùng protocol text kết thúc `\n`: buffer incomplete messages, quét tìm `\n` để tách message
- `recv()` trả về `0` → client đã close; `-1` → error, close connection
- Vì dùng blocking `recv()`, mỗi thread chỉ xử lý client của nó, không cần non-blocking trickery

**Synchronization với mutex:**
```c
// Trước khi access/modify clients[]
pthread_mutex_lock(&clients_mutex);
// ... critical section
pthread_mutex_unlock(&clients_mutex);

// Broadcast example
void broadcast_message(const char *username, const char *text) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].fd >= 0 && clients[i].authenticated) {
            send(clients[i].fd, message, len, 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    save_to_history(username, text);  // Save with flock
}
```

### Chi tiết kỹ thuật — Hash password

Dùng `crypt()` (glibc, hỗ trợ SHA-256/512 qua tiền tố `$5$`/`$6$`) là đủ cho bài, không cần thư viện ngoài:
```c
#include <crypt.h>
char *hashed = crypt(password, "$6$somesalt$");   // lưu chuỗi `hashed` vào file, không lưu password gốc
// khi đăng nhập: so sánh crypt(input_password, hashed) == hashed
```
Nhớ link `-lcrypt` khi biên dịch.

**Gợi ý thứ tự nên làm:**
1. **Tuần 1:** viết xong khung thread-per-client server cơ bản — main thread accept kết nối, spawn pthread cho mỗi client, xử lý echo đơn giản blocking I/O (chưa cần auth/broadcast gì), test bằng `nc` hoặc simple client.
2. **Tuần 2:** thêm đăng ký/đăng nhập + hash password (P3-M1, M6), rồi broadcast thật (P3-M2) với mutex protection. Đây là lúc nên tự thiết kế xong protocol (mục 2 trong DESIGN.md) trước khi code tiếp.
3. **Tuần 3:** thêm lưu & gửi lịch sử khi join (P3-M7), xử lý disconnect đột ngột sạch sẽ (P3-M4). Dùng `flock()` khi đọc/ghi message history, và `pthread_mutex` cho clients array.
4. **Tuần 4:** hoàn thiện `/who` (P3-M5), viết client CLI theo đúng hành vi mẫu, viết README định nghĩa protocol, chạy đủ Test Case.

**Gợi ý test thủ công khi code:** dùng `nc localhost <port>` hoặc `telnet localhost <port>` để giả lập client thô khi debug protocol, không cần chờ viết xong client CLI hoàn chỉnh mới test được server. Test đăng ký, đăng nhập, broadcast, history trong quá trình development.

## Client CLI Behavior (mẫu tham khảo)

**Lúc mới mở client:**
```
=== DevLinux Chat Client ===
Server: 127.0.0.1:9000
Đăng nhập hay Đăng ký? [login/register]: login
Username: viet
Password: ********
```

**Nhập sai username hoặc password:**
```
[!] Sai username hoặc password. Vui lòng thử lại (2 lần thử còn lại).
Username: viet
Password: ********
```
Sau 3 lần nhập sai liên tiếp, client tự ngắt kết nối và in ra:
```
[!] Đã nhập sai quá số lần cho phép. Kết nối đã bị đóng.
```

**Sau khi đăng nhập thành công — server gửi lại toàn bộ lịch sử tin nhắn trước, rồi vào màn hình chat chính:**
```
[Connected as viet — 3 users online]
--- Lịch sử tin nhắn ---
[09:10:02] minh: có ai đang test server không?
[09:15:44] lan: em đang test đây anh
[10:32:01] minh: chào mọi người
[10:32:15] lan: hi hi
--------------------------------------
[10:32:40] *** khoa has joined ***
> _
```
Người dùng gõ tin nhắn rồi Enter để gửi broadcast. Ngoài ra hỗ trợ một số lệnh bắt đầu bằng `/` để không lẫn với nội dung chat thường:
```
/who        -> liệt kê user đang online
/quit       -> thoát chương trình
/help       -> xem danh sách lệnh
```

**Khi có tin nhắn mới tới trong lúc đang gõ dở**, tin nhắn phải in phía trên dòng nhập hiện tại, không được đè lên hoặc xoá mất nội dung người dùng đang gõ dở.

**Khi mất kết nối server:**
```
[!] Mất kết nối tới server. Đang thử kết nối lại...
```

## Expected Output
- Demo nhiều terminal client kết nối đồng thời, đăng nhập, gửi tin nhắn thấy broadcast tới toàn bộ client khác, một client bị kill đột ngột không ảnh hưởng client khác.
- Demo riêng luồng nhập sai username/password 3 lần liên tiếp, thấy client tự ngắt kết nối đúng như mô tả.

## Submission
- GitHub PR, tuân thủ đúng **Cấu trúc bàn giao chuẩn** bên dưới. Định nghĩa protocol phải nêu rõ trong README. Kết quả tự chạy Test Case đính kèm trong `docs/test_report.md`.

## Cấu trúc bàn giao chuẩn — Project 3
```
<student-repo>/
├── DESIGN.md
├── README.md                      # bắt buộc có mục định nghĩa Protocol
├── src/
│   ├── server/
│   │   ├── main.c                 # thread-per-client main loop (P3-M3), accept & pthread_create
│   │   ├── auth.c                 # đăng ký/đăng nhập, hash password (P3-M1, P3-M6)
│   │   ├── broadcast.c            # broadcast tin nhắn với mutex protection (P3-M2)
│   │   └── history.c              # lưu & gửi lịch sử tin nhắn khi join, dùng flock (P3-M7)
│   └── client/
│       └── main.c                 # client CLI (theo đúng Client CLI Behavior đã định nghĩa)
└── docs/
    └── test_report.md             # kết quả tự chạy TESTCASES_P3_student.md (bắt buộc, kèm log terminal)
```

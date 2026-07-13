# DESIGN.md — Project 3: Multi-user CLI Chat Server/Client

**Học viên:** nguyen-quoc-viet | **Ngày nộp:** 2026-07-10

> File này mô tả kiến trúc **thread-per-client** được lựa chọn thay vì epoll. Mỗi client được xử lý bởi 1 pthread riêng với blocking I/O, dùng mutex để synchronize shared state. Đơn giản hơn, dễ debug hơn, và vẫn xử lý đủ concurrent clients.

---

## 1. Kiến trúc tổng thể (Thread-per-client)

Sơ đồ luồng xử lý của server:

```
┌─────────────────────────────────────────────────┐
│          Server Startup                         │
│  - Create listening socket on port 5000         │
│  - Initialize mutex for clients[] synchronization │
│  - Bind & listen for connections                │
└──────────────┬──────────────────────────────────┘
               │
        ┌──────▼──────────────────────┐
        │  Main Accept Loop           │
        │  (in main thread)           │
        │  - accept() new connection  │
        │  - store client in clients[]│
        │  - pthread_create()         │
        └──────┬──────────────────────┘
               │
        ┌──────▼────────────────────────────┐
        │  Client Thread (for each client)  │
        │  - recv() blocking                │
        │  - parse message (delimiter \n)   │
        │  - route by command type          │
        │  - send() response (blocking)     │
        │  - broadcast to all authenticated │
        │    clients (with mutex lock)      │
        └──────┬────────────────────────────┘
               │
        ┌──────▼──────────────────────┐
        │  Synchronization Points     │
        │  - mutex lock before:       │
        │    * access clients[]       │
        │    * broadcast_message()    │
        │    * update user list       │
        │  - flock() for history file │
        └──────┬──────────────────────┘
               │
        ┌──────▼──────────────────────┐
        │  Shutdown (on SIGTERM)      │
        │  - Stop accepting new       │
        │  - Close all client threads │
        │  - Wait for thread cleanup  │
        │  - Free all resources       │
        └─────────────────────────────┘
```

## 2. Danh sách Process/Thread (bắt buộc, liệt kê từng cái cụ thể)

| Tên | Loại | Vai trò | Tạo lúc nào | Kết thúc lúc nào | Giao tiếp với ai — qua kênh gì |
|---|---|---|---|---|---|
| main | process | Thread chính: lắng nghe & accept kết nối | khởi động server | nhận SIGTERM hoặc EOF | listening socket, tất cả client threads qua mutex |
| client_handler × N | thread | Xử lý 1 client: recv/send blocking, parse protocol, route commands | mỗi khi accept() thành công | client disconnect (read()=0 hoặc error) | 1 client socket, shared clients[] array (mutex-protected), message history file (flock) |

**Lý do chọn thread-per-client thay vì epoll:**
- ✅ Đơn giản: blocking I/O, không cần non-blocking socket complexity
- ✅ Dễ debug: mỗi thread độc lập, không có state machine complexity
- ✅ Đủ scalable: xử lý ~100 concurrent clients (bài yêu cầu MAX_CLIENTS=100)
- ✅ Thread overhead (~1-2MB per thread) chấp nhận được cho bài này
- ✅ Mutex synchronization đơn giản hơn epoll state management

## 3. Định nghĩa Protocol (bắt buộc — đây là phần quan trọng nhất)

**Chọn:** [X] Text-based dạng dòng lệnh (giống IRC), kết thúc mỗi message bằng `\n`

Liệt kê toàn bộ loại message client ↔ server:

| Chiều | Loại message | Định dạng | Ví dụ |
|---|---|---|---|
| C→S | Đăng ký | `REGISTER:username:password\n` | `REGISTER:alice:pass123\n` |
| C→S | Đăng nhập | `LOGIN:username:password\n` | `LOGIN:bob:mypass\n` |
| C→S | Gửi tin nhắn chat | `MSG:message_text\n` | `MSG:Hello everyone!\n` |
| C→S | Yêu cầu danh sách user | `USERLIST\n` | `USERLIST\n` |
| C→S | Xem lịch sử | `HISTORY\n` | `HISTORY\n` |
| C→S | Đăng xuất | `LOGOUT\n` | `LOGOUT\n` |
| S→C | Register success | `OK:REGISTER\n` | `OK:REGISTER\n` |
| S→C | Login success | `OK:LOGIN\n` | `OK:LOGIN\n` |
| S→C | Auth failed | `ERR:INVALID_CREDENTIALS\n` | `ERR:INVALID_CREDENTIALS\n` |
| S→C | Broadcast tin nhắn | `FROM:username:message\n` | `FROM:alice:Hello everyone!\n` |
| S→C | Danh sách user online | `USERS:user1,user2,user3\n` | `USERS:alice,bob,charlie\n` |
| S→C | Lịch sử tin nhắn | `HISTORY:username:message\n` | `HISTORY:alice:hello\nHISTORY:bob:hi\n` |
| S→C | Lỗi protocol | `ERR:INVALID_FORMAT\n` | `ERR:INVALID_FORMAT\n` |

**Quy tắc:**
- Mỗi message kết thúc bằng `\n` (newline)
- Format: `COMMAND:PARAM1:PARAM2:...\n` (colon-delimited)
- Không hỗ trợ `:` trong message nội dung (sẽ làm sai delimiter)
- Server không phân biệt hoa/thường cho COMMAND
- Delimiter là newline, tìm bằng `strchr(buf, '\n')`

## 4. Cấu trúc dữ liệu & Synchronization

**Client structure:**
```c
typedef struct {
    int fd;
    char username[64];
    int authenticated;
} client_t;

// Global shared state
client_t clients[MAX_CLIENTS];          // 100 slots
pthread_mutex_t clients_mutex;          // Protect access to clients[]
int client_count;                       // Number of authenticated users
```

**Khi nào cần lock mutex:**
1. Thêm/xóa client từ `clients[]`
2. Cập nhật `authenticated` status
3. Broadcast message (duyệt `clients[]`)
4. Gửi user list (duyệt `clients[]`)
5. Đếm online users

**File storage:**
- **accounts.db:** mỗi dòng = `username:password_hash:timestamp\n`
  - Hash algorithm: SHA256 (OpenSSL) with salt "devlinux_salt_2026"
  - Read-only access: không cần lock (chỉ authentication đọc)
  - Write (register): dùng `flock(LOCK_EX)` tạm thời

- **messages.log:** append-only, mỗi dòng = `username:timestamp:message\n`
  - Save: mỗi khi có client gửi message
  - Read (HISTORY): gửi lại toàn bộ file khi client vừa login
  - Lock: `flock(LOCK_EX)` khi ghi, `flock(LOCK_SH)` khi đọc

## 5. Xử lý Message & Parsing

**Blocking socket with line-buffering:**
```c
void *client_handler(void *arg) {
    client_t *client = (client_t *)arg;
    char buf[BUFFER_SIZE];
    char line[BUFFER_SIZE];
    int pos = 0;
    int n;
    
    while (running && client->fd >= 0) {
        n = recv(client->fd, buf, sizeof(buf), 0);
        
        if (n <= 0) {
            close_client(client);  // Client disconnected
            break;
        }
        
        // Process each complete line (newline-delimited)
        for (int i = 0; i < n; i++) {
            line[pos++] = buf[i];
            
            if (buf[i] == '\n') {
                line[pos - 1] = '\0';  // Remove newline
                route_message(client, line);
                pos = 0;
            }
        }
    }
    return NULL;
}
```

**Route & Handle:**
```c
void route_message(client_t *client, const char *message) {
    if (strncmp(message, "LOGIN:", 6) == 0) {
        handle_login(client, message + 6);
    } else if (strncmp(message, "REGISTER:", 9) == 0) {
        handle_register(client, message + 9);
    } else if (strncmp(message, "MSG:", 4) == 0) {
        handle_message(client, message + 4);
    } else if (strcmp(message, "LOGOUT") == 0) {
        close_client(client);
    } else if (strcmp(message, "USERLIST") == 0) {
        handle_userlist(client);
    } else {
        send_error(client->fd, "INVALID_FORMAT");
    }
}
```

**Broadcast (with mutex):**
```c
void broadcast_message(const char *username, const char *text) {
    char buf[512];
    snprintf(buf, sizeof(buf), "FROM:%s:%s\n", username, text);
    
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].fd >= 0 && clients[i].authenticated) {
            send(clients[i].fd, buf, strlen(buf), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    
    save_message_log(username, text);  // Persistent storage
}
```

## 6. Xử lý client ngắt kết nối đột ngột

**Phát hiện:**
- `recv()` trả về `0` → client đã close kết nối chủ động (EOF)
- `recv()` trả về `-1` → error (EPIPE, connection reset)
- Thread exit nếu `running` flag = 0 hoặc `client->fd < 0`

**Dọn dẹp tài nguyên:**
```c
void close_client(client_t *client) {
    if (client->fd < 0)
        return;  // Already closed
    
    close(client->fd);
    
    pthread_mutex_lock(&clients_mutex);
    if (client->authenticated)
        client_count--;
    client->fd = -1;
    client->authenticated = 0;
    pthread_mutex_unlock(&clients_mutex);
}
```

Đảm bảo **không rò rỉ file descriptor** bằng:
- Close socket trước khi unlock mutex
- Set `fd = -1` sau khi close để avoid double-close
- Client thread tự exit khi fd < 0

## 7. Edge Case / Failure Handling dự kiến

| Tình huống | Cách xử lý |
|---|---|
| Client gửi message không đúng protocol | Send `ERR:INVALID_FORMAT\n`, không close (cho retry) |
| Client kill đột ngột | Phát hiện `recv()` = 0 hoặc error, `close_client()` cleanup, thread exit |
| Nhập sai password lần đầu | Server gửi `ERR:INVALID_CREDENTIALS\n`, client cho phép retry |
| File tài khoản bị khóa bởi thread khác | `flock()` block tạm thời (milliseconds), không ảnh hưởng |
| Buffer input overflow (>4096 bytes một lần) | Quét `\n` từng `recv()`, nếu không tìm → để cho line-buffering xử lý |
| Server nhận SIGTERM | Set `running = 0`, main thread stop accept, wait tất cả client threads clean up, exit |
| Multiple threads write messages.log | `flock(LOCK_EX)` prevent race condition |

## 8. Bảng đối chiếu Requirement Coverage

| Requirement ID | Mô tả ngắn | Đề cập ở mục | Trạng thái |
|---|---|---|---|
| P3-M1 | Đăng ký/đăng nhập, mật khẩu hash | Mục 4, 5 | ✅ SHA256 + salt |
| P3-M2 | Broadcast 1 phòng chung | Mục 5 | ✅ `broadcast_message()` with mutex |
| P3-M3 | Thread-per-client (thay epoll) | Mục 1, 2 | ✅ 1 pthread per client |
| P3-M4 | Xử lý disconnect đột ngột | Mục 6 | ✅ `close_client()` no leak |
| P3-M5 | Danh sách user online | Mục 5 | ✅ USERLIST command |
| P3-M6 | Xử lý sai password retry | Mục 7, client | ✅ Client retry logic |
| P3-M7 | Lưu & gửi lại lịch sử | Mục 4, 5 | ✅ messages.log + HISTORY |

## 9. Client CLI Behavior

**Quy trình login:**
1. Client kết nối tới server
2. Hỏi `Login or Register? [login/register]:`
3. Nhập username, password
4. Nếu sai: in `[!] Login failed: Invalid username or password.`
5. Hỏi `Try again? [yes/no]:`
6. Nếu chọn yes: lặp lại từ bước 2
7. Nếu chọn no hoặc quá số lần: exit

**Trong chat room:**
- Gõ message rồi Enter → gửi `MSG:...\n` → server broadcast
- `/who` → gửi `USERLIST\n` → nhận danh sách users
- `/help` → hiển thị danh sách lệnh
- `/quit` → gửi `LOGOUT\n` → exit

**Lịch sử:**
- Khi login xong, server tự động gửi `HISTORY:...` messages
- Client display với `[history]` prefix

## 10. Rủi ro / Phần khó nhất

**Phần khó nhất: Thread synchronization**
- `pthread_mutex` protect `clients[]` array → phải lock khi:
  - Thêm/xóa client
  - Duyệt clients (broadcast)
  - Update user count
- Deadlock risk: luôn lock trong cùng thứ tự, giải phóng ngay
- Data race: nếu không lock khi đọc `clients[i].authenticated` → có thể miss update

**Testing:**
- Test với 50+ concurrent clients
- Test kill client đột ngột → không leak fd
- Test broadcast với multiple senders cùng lúc
- Test message history consistency với concurrent logins

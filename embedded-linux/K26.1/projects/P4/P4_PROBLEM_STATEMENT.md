# PROJECT 4: Infrastructure Health Monitoring Agent

**Thời lượng:** 4 tuần | **Độ khó:** Trung bình | **Nộp bài:** GitHub PR (hệ thống chấm điểm tự động `devlinux-reviewer`, ngưỡng ≥80 Pass / 60–79 Pass with revisions / <60 Fail)

**Thiết bị:** chỉ cần máy Linux, không cần phần cứng/driver.

**Mô phỏng sản phẩm thật:** hệ thống giám sát hạ tầng dạng Zabbix/Nagios rút gọn — nhiều agent gửi dữ liệu về 1 server trung tâm.

**Kiến thức áp dụng:** Socket TCP, epoll, đọc `/proc` và `/sys`, Signal, File I/O (log có cấu trúc), systemd, Valgrind/Heaptrack (kiểm tra leak dài hạn).

> Quy ước ID requirement (`P4-M1`, `P4-N1`...) dùng chung giữa đề bài này, `DESIGN.md` học viên nộp, và rubric chấm điểm — xem thêm quy định chung ở `00_Chung/QUY_DINH_CHUNG.md`.

---

## Problem Statement
Xây dựng hệ thống gồm nhiều **agent (client)** chạy trên các máy khác nhau, đọc chỉ số hệ thống trực tiếp từ `/proc` và `/sys` (CPU, RAM, disk, network), gửi định kỳ về 1 **server giám sát trung tâm** qua TCP. Mỗi agent có 1 **agent_id cố định** (lưu trong file, giữ nguyên qua các lần khởi động lại) và tự hiển thị dashboard CLI của riêng nó (chỉ số + màu + thanh load) song song với việc gửi dữ liệu về server. Server dùng epoll xử lý nhiều agent đồng thời, ghi log có cấu trúc, phát hiện agent bị mất kết nối (OFFLINE) để cảnh báo, và hiển thị dashboard tổng hợp **động theo đúng số lượng client đang kết nối** (client kết nối tới đâu, dashboard tự thêm dòng hiển thị tới đó; client ngắt kết nối/OFFLINE thì dòng tương ứng chuyển trạng thái, không bị xoá mất).

## Functional Requirements

**Must-have:**
- **P4-M1.** Agent (client) đọc trực tiếp `/proc/stat`, `/proc/meminfo`, `/proc/net/dev`, v.v. (không dùng thư viện có sẵn như `sysstat`), gửi dữ liệu định kỳ về server qua TCP.
- **P4-M2.** Agent có **agent_id** cố định, lưu trong 1 file cấu hình (ví dụ `agent.id`) — nếu file đã tồn tại thì đọc lại đúng agent_id cũ, nếu chưa có thì tự sinh 1 lần rồi lưu ra file để dùng lại cho các lần chạy sau.
- **P4-M3.** Agent tự hiển thị **dashboard CLI của chính nó**: hiển thị agent_id, và các chỉ số (CPU/RAM/DISK...) theo dạng **thanh load tỉ lệ** (progress bar) + **màu sắc theo mức độ** (bình thường/cảnh báo/nguy hiểm), dùng chung quy tắc màu và ngưỡng như dashboard server.
- **P4-M4.** Server dùng epoll quản lý nhiều kết nối agent đồng thời.
- **P4-M5.** Server ghi log theo 3 loại tách biệt: log dữ liệu định kỳ, log **ALERT** (khi chỉ số vượt ngưỡng), log sự kiện kết nối (connect/disconnect).
- **P4-M6.** Cơ chế keepalive 2 lớp: heartbeat tầng ứng dụng (agent gửi PING mỗi N giây) **và** `SO_KEEPALIVE` ở tầng TCP.
- **P4-M7.** Server phát hiện agent OFFLINE (không nhận heartbeat quá timeout) và ghi cảnh báo riêng.
- **P4-M8.** Chạy dài hạn dưới dạng systemd service; phải được kiểm tra không rò rỉ bộ nhớ qua Valgrind/Heaptrack khi chạy nhiều giờ.
- **P4-M9.** **Server hiển thị dashboard CLI**, mỗi client đang kết nối (hoặc đã từng kết nối và hiện OFFLINE) tương ứng với **1 khối/dòng riêng trên dashboard**, hiển thị đúng agent_id của từng client — có bao nhiêu client thì hiển thị bấy nhiêu, thêm/bớt động theo thời gian thực (client mới connect → tự thêm khối mới; client OFFLINE → khối chuyển trạng thái, không biến mất). Mỗi khối hiển thị chỉ số theo dạng thanh load tỉ lệ + màu sắc theo mức độ, giống hệt quy tắc hiển thị bên agent.
- **P4-M10.** Trên CLI của server, hỗ trợ lệnh dạng `/command` để **đẩy config mới xuống agent** (thay vì agent tự đọc config tĩnh), ví dụ `/config <agent_id> interval=5` để đổi chu kỳ gửi dữ liệu của agent đó. Server gửi config này xuống đúng agent qua kết nối TCP đang có, agent áp dụng ngay không cần restart.
- **P4-M11.** Trên CLI của server, hỗ trợ lệnh dạng `/command` để **truy vấn lịch sử dữ liệu đã log**, ví dụ `/history <agent_id> [--last N]` hiển thị lại các bản ghi dữ liệu định kỳ/ALERT đã log của agent đó.

**Nice-to-have:**
- **P4-N1.** 2 mức cảnh báo WARNING/CRITICAL theo ngưỡng khác nhau (dùng chung ngưỡng với phần tô màu ở mục P4-M9).
- **P4-N2.** Gửi thông báo ra ngoài qua Telegram Bot khi có ALERT/OFFLINE.
- **P4-N3.** Theo dõi uptime/SLA từng agent.
- **P4-N4.** Xác thực agent bằng token khi kết nối.

## Quy định màu sắc & ngưỡng cảnh báo (bắt buộc áp dụng thống nhất ở cả Agent và Server)

| Mức | Điều kiện | Màu hiển thị | Mã ANSI |
|---|---|---|---|
| **Bình thường** | dưới ngưỡng WARNING | Xanh lá | `\033[32m` |
| **Cảnh báo (WARNING)** | từ ngưỡng WARNING đến dưới ngưỡng CRITICAL | Vàng | `\033[33m` |
| **Nguy hiểm (CRITICAL)** | từ ngưỡng CRITICAL trở lên | Đỏ | `\033[31m` |
| Reset màu | — | — | `\033[0m` |

**Ngưỡng mặc định theo từng chỉ số** (phải cho phép cấu hình lại qua file config hoặc `/config`, không hardcode):

| Chỉ số | WARNING | CRITICAL |
|---|---|---|
| CPU | ≥ 70% | ≥ 90% |
| RAM | ≥ 75% | ≥ 90% |
| DISK | ≥ 80% | ≥ 95% |

**Quy tắc áp dụng:**
- So sánh giá trị chỉ số với ngưỡng CRITICAL trước — nếu đạt/vượt thì tô đỏ ngay, không cần so tiếp ngưỡng WARNING.
- Thanh load (`█`/`░`) tô cùng màu với chữ % và nhãn trạng thái phía sau nó — không để thanh 1 màu còn chữ 1 màu khác.
- Trạng thái `OFFLINE` của agent trên dashboard server: hiển thị đỏ (dùng `\033[31m`) bất kể chỉ số cuối cùng ghi nhận là gì, vì đây là cảnh báo mất kết nối chứ không phải mức tải.
- Khi vượt ngưỡng CRITICAL, đây chính là điều kiện để server ghi log **ALERT**; ngưỡng WARNING không bắt buộc phải ghi ALERT riêng trừ khi làm thêm phần nice-to-have "2 mức cảnh báo".

## Design Hints (lưu ý kỹ thuật, tránh bẫy thường gặp)
- Đây là project cố ý có bug rò rỉ bộ nhớ (memory leak) để sinh viên bắt buộc phải dùng Valgrind/Heaptrack phát hiện và fix.
- Thiết kế log dạng có thể parse được (ví dụ JSON line hoặc format cố định), không log tự do.
- Timeout phát hiện OFFLINE nên tách biệt rõ giữa "mất gói tạm thời" và "agent thực sự chết", tránh false positive.
- Thanh load có thể vẽ bằng ký tự đặc (`█`) và ký tự rỗng (`░`), độ dài thanh cố định (ví dụ 20 ký tự) tương ứng tỉ lệ % của chỉ số.
- Dashboard nên tự refresh theo chu kỳ (ví dụ mỗi 2 giây) bằng cách xoá màn hình (`\033[2J\033[H`) rồi vẽ lại toàn bộ, tránh in chồng dòng lên nhau.
- Server nên giữ 1 danh sách (map từ client id → dữ liệu mới nhất + trạng thái) để mỗi lần refresh dashboard chỉ cần duyệt qua danh sách này và in ra đúng số dòng tương ứng — không hardcode số lượng client hiển thị.
- Phần đọc lệnh `/command` từ stdin của admin cần chạy song song (cùng epoll loop, thêm `STDIN_FILENO` vào tập theo dõi) với việc dashboard tự refresh và nhận dữ liệu từ agent — không được block lẫn nhau.
- Lệnh `/config` nên định nghĩa 1 message type riêng trong protocol server↔agent để agent phân biệt được đây là lệnh đổi config chứ không phải dữ liệu thông thường.
- agent_id nên sinh 1 lần (ví dụ UUID rút gọn, hoặc hostname + số ngẫu nhiên) và ghi ra file cấu hình cục bộ của agent; các lần chạy sau agent đọc file này trước, chỉ sinh mới nếu file chưa tồn tại — tránh agent đổi "danh tính" mỗi lần restart khiến server hiểu nhầm là agent mới.
- Nên tách phần logic vẽ thanh load + tô màu theo ngưỡng thành 1 hàm dùng chung (nhận vào giá trị % + loại chỉ số, trả về chuỗi đã tô màu), để cả agent và server đều gọi lại đúng 1 chỗ, tránh lệch quy tắc màu giữa 2 phía.

## Gợi ý thiết kế cho học viên

**Gợi ý kiến trúc tổng thể:** đây là project có 2 chương trình độc lập (agent và server), nên thiết kế tách bạch từ đầu, không dùng chung code base lẫn lộn:
- **Agent** gồm 2 thread: 1 thread thu thập `/proc` + gửi TCP định kỳ, 1 thread tự vẽ dashboard riêng đọc cùng 1 struct dữ liệu (có mutex bảo vệ nếu 2 thread cùng đụng).
- **Server** nên có 1 struct `agent_entry { agent_id, last_data, status, last_heartbeat_time }` lưu trong 1 map (agent_id → agent_entry), **không dùng mảng cố định**. Mọi thứ khác (dashboard, log, `/history`) đều duyệt qua map này.
- Vòng lặp chính của server nên gộp cả 3 việc vào cùng 1 `epoll_wait`: (1) fd lắng nghe agent mới, (2) fd từng agent gửi dữ liệu, (3) `STDIN_FILENO` cho lệnh admin, (4) 1 `timerfd` để tự refresh dashboard mỗi 2 giây và check timeout OFFLINE — tránh dùng nhiều thread không cần thiết.

### Chi tiết kỹ thuật — Đọc chỉ số từ `/proc`

**CPU (`/proc/stat`):** dòng đầu tiên (`cpu ...`) có các cột `user nice system idle iowait irq softirq`. Vì đây là **số lũy kế từ lúc boot**, phải đọc 2 lần cách nhau 1 khoảng (ví dụ 1 giây) rồi tính hiệu số mới ra được % tức thời:
```
idle_time  = idle + iowait
total_time = user + nice + system + idle + iowait + irq + softirq
% CPU sử dụng = (1 - (delta_idle / delta_total)) * 100
```
**RAM (`/proc/meminfo`):** lấy `MemTotal` và `MemAvailable` (không dùng `MemFree` vì không tính cache/buffer có thể giải phóng được):
```
% RAM sử dụng = (1 - MemAvailable / MemTotal) * 100
```
**Disk:** dùng `statvfs("/", &st)` (hàm thư viện chuẩn, không phải đọc `/proc`), tính `(f_blocks - f_bfree) / f_blocks * 100`.

### Chi tiết kỹ thuật — Protocol Agent ↔ Server

Gợi ý dùng **JSON-line** (mỗi message là 1 dòng JSON kết thúc `\n`) — dễ debug bằng mắt hơn binary protocol, và dễ mở rộng thêm field sau này:
```
{"type":"data","agent_id":"web-01","cpu":52,"ram":81,"disk":30}
{"type":"heartbeat","agent_id":"web-01"}
{"type":"config","key":"interval","value":5}
```
Field `"type"` giúp phân biệt: agent gửi lên `data`/`heartbeat`, server gửi xuống `config`. Không cần thư viện JSON đầy đủ — vì format cố định và đơn giản, có thể parse bằng `sscanf`/tách chuỗi thủ công thay vì tích hợp thư viện ngoài (nhưng nếu muốn chắc chắn không lỗi parse, dùng thư viện nhỏ gọn như `cJSON` cũng được, không bắt buộc).

### Chi tiết kỹ thuật — epoll với `timerfd` + `STDIN_FILENO`

```c
int epfd = epoll_create1(0);
epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &(struct epoll_event){.events=EPOLLIN, .data.fd=listen_fd});
epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &(struct epoll_event){.events=EPOLLIN, .data.fd=STDIN_FILENO});

int tfd = timerfd_create(CLOCK_MONOTONIC, 0);
struct itimerspec its = { .it_value = {2, 0}, .it_interval = {2, 0} }; // refresh mỗi 2s
timerfd_settime(tfd, 0, &its, NULL);
epoll_ctl(epfd, EPOLL_CTL_ADD, tfd, &(struct epoll_event){.events=EPOLLIN, .data.fd=tfd});

while (1) {
    int n = epoll_wait(epfd, events, MAX_EVENTS, -1);
    for (int i = 0; i < n; i++) {
        int fd = events[i].data.fd;
        if (fd == listen_fd) { /* accept agent mới */ }
        else if (fd == STDIN_FILENO) { /* đọc dòng lệnh /config, /history từ admin */ }
        else if (fd == tfd) { /* đọc để "xả" timer, rồi vẽ lại dashboard + check timeout OFFLINE */ }
        else { /* dữ liệu từ 1 agent cụ thể */ }
    }
}
```
Cách này gộp toàn bộ vào **1 vòng lặp duy nhất**, không cần thread riêng cho dashboard hay cho đọc lệnh admin — đúng tinh thần bài học về I/O multiplexing.

### Chi tiết kỹ thuật — Phát hiện OFFLINE

Với mỗi agent, lưu `last_heartbeat_time` (cập nhật mỗi khi nhận được `data` hoặc `heartbeat`). Mỗi lần `timerfd` bắn (mỗi 2s), duyệt qua map, so `now - last_heartbeat_time` với ngưỡng timeout (ví dụ 3× chu kỳ heartbeat, để tránh false positive do mất gói ngẫu nhiên 1 lần) — vượt ngưỡng thì đổi status sang `OFFLINE` và ghi log.

**Gợi ý thứ tự nên làm:**
1. **Tuần 1:** viết agent đọc `/proc` in ra console trước (chưa cần gửi mạng gì), rồi mới nối TCP gửi lên 1 server "echo" tạm để test đường truyền.
2. **Tuần 2:** xây dựng đúng cấu trúc map agent_id → agent_entry ở server, hoàn thiện epoll nhận nhiều agent (P4-M4), viết 3 loại log (P4-M5).
3. **Tuần 3:** làm keepalive + OFFLINE detection (P4-M6, M7) — test bằng cách tắt agent đột ngột nhiều lần; làm dashboard 2 phía (P4-M3, M9) tái sử dụng cùng hàm vẽ màu.
4. **Tuần 4:** thêm `/config` và `/history` (P4-M10, M11), chạy Valgrind/Heaptrack tối thiểu 30 phút cho P4-M8, viết README, hoàn thiện `docs/test_report.md`.

**Gợi ý test thủ công khi code:** dùng lệnh `stress --cpu 2` (hoặc tương đương) để chủ động đẩy CPU lên cao, kiểm tra dashboard đổi màu đúng theo ngưỡng mà không cần đợi máy tự nóng lên.

## CLI Dashboard — phía Agent (mẫu tham khảo)
```
=== DevLinux Health Agent — id: web-01 ===                   [refresh: 2s]
Server: 192.168.1.10:9000   Status: CONNECTED

  CPU   [██████████░░░░░░░░░░]  52%   (bình thường)
  RAM   [████████████████░░░░]  81%   (cảnh báo)
  DISK  [██████░░░░░░░░░░░░░░]  30%   (bình thường)
```
`web-01` ở đây chính là agent_id đọc từ file (ví dụ `agent.id`), không đổi qua các lần khởi động lại. Agent tự vẽ dashboard này cho chính nó, song song với việc gửi dữ liệu về server — không cần chờ lệnh gì từ server để hiển thị.

## CLI Dashboard — phía Server (mẫu tham khảo)
```
=== Infra Health Monitor — 3 agents ===                      [refresh: 2s]

[web-01]     ONLINE
  CPU   [██████████░░░░░░░░░░]  52%   (bình thường)
  RAM   [████████████████░░░░]  81%   (cảnh báo)
  DISK  [██████░░░░░░░░░░░░░░]  30%   (bình thường)

[db-02]      ONLINE
  CPU   [███████████████████░]  94%   (nguy hiểm)
  RAM   [██████████████░░░░░░]  70%   (cảnh báo)
  DISK  [████░░░░░░░░░░░░░░░░]  22%   (bình thường)

[cache-03]   OFFLINE (mất kết nối 45s trước)

> /_
```
Dòng `> /_` ở cuối là nơi admin gõ lệnh điều khiển, không làm gián đoạn việc dashboard tự refresh phía trên. Ví dụ:
```
> /config db-02 interval=5
[OK] Đã gửi config mới xuống db-02: interval=5s

> /history web-01 --last 5
[10:40:02] web-01 CPU=45% RAM=78% DISK=30%
[10:40:04] web-01 CPU=48% RAM=79% DISK=30%
[10:40:06] web-01 CPU=52% RAM=81% DISK=30% [ALERT] RAM vượt ngưỡng cảnh báo
[10:40:08] web-01 CPU=50% RAM=80% DISK=30%
[10:40:10] web-01 CPU=52% RAM=81% DISK=30%

> /help
/config <agent_id> <key>=<value>   -> đẩy config mới xuống agent
/history <agent_id> [--last N]     -> xem lại lịch sử dữ liệu đã log
/help                               -> xem danh sách lệnh
```
Trong terminal thật, dòng `CPU 94% (nguy hiểm)` sẽ hiển thị **màu đỏ**, `RAM 81%/70% (cảnh báo)` màu **vàng**, các chỉ số còn lại màu **xanh**; dòng `OFFLINE` nên hiển thị nổi bật (ví dụ đỏ hoặc in đậm) để dễ nhận biết ngay.

## Expected Output
- Demo nhiều agent (có thể chạy trong nhiều container/terminal) gửi dữ liệu về server, mỗi agent tự hiển thị dashboard riêng của nó kèm đúng agent_id.
- Tắt đột ngột 1 agent để trigger OFFLINE alert, xem 3 loại log tách biệt trên server.
- Restart lại 1 agent, thấy agent_id hiển thị vẫn giữ nguyên như trước (đọc lại từ file), server nhận diện đúng là cùng 1 agent cũ chứ không tạo agent mới.
- Demo dashboard trên server tự thêm dòng khi có agent mới kết nối, và giữ nguyên dòng (chuyển sang OFFLINE) khi agent ngắt kết nối — số dòng hiển thị đúng bằng số agent đã từng/đang kết nối.
- Demo dashboard CLI (cả bên agent và bên server) hiển thị đúng màu theo ngưỡng khi tăng tải (ví dụ chạy `stress` để đẩy CPU lên cao và quan sát màu chuyển từ xanh → vàng → đỏ ở cả 2 phía).
- Demo dùng `/config <agent_id> interval=...` thấy agent tương ứng đổi chu kỳ gửi dữ liệu ngay không cần restart.
- Demo dùng `/history <agent_id>` xem lại đúng dữ liệu đã log trước đó của agent đó.
- Báo cáo Heaptrack/Valgrind cho thấy không còn leak sau khi fix.

## Submission
- GitHub PR, tuân thủ đúng **Cấu trúc bàn giao chuẩn** bên dưới. Kết quả tự chạy Test Case + báo cáo Valgrind/Heaptrack (trước/sau khi fix leak) đính kèm trong `docs/test_report.md`.

## Cấu trúc bàn giao chuẩn — Project 4
```
<student-repo>/
├── DESIGN.md
├── README.md                      # bắt buộc có mục định nghĩa Protocol agent↔server
├── src/
│   ├── server/
│   │   ├── main.c                 # epoll loop chính (P4-M4)
│   │   ├── logger.c               # 3 loại log tách biệt (P4-M5)
│   │   ├── keepalive.c            # keepalive + phát hiện OFFLINE (P4-M6, P4-M7)
│   │   ├── dashboard.c            # dashboard CLI server (P4-M9)
│   │   └── command.c              # lệnh /config, /history (P4-M10, P4-M11)
│   └── agent/
│       ├── main.c                 # thu thập /proc + gửi định kỳ (P4-M1)
│       ├── agent_id.c             # sinh/đọc agent_id từ file (P4-M2)
│       └── dashboard.c            # dashboard CLI riêng của agent (P4-M3)
└── docs/
    └── test_report.md             # kết quả tự chạy TESTCASES_P4_student.md (bắt buộc), báo cáo Valgrind/Heaptrack
```

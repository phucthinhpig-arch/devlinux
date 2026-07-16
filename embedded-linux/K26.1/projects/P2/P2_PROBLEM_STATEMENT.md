# PROJECT 2: Smart Weather Alarm Clock

**Thời lượng:** 4 tuần | **Độ khó:** Trung bình | **Nộp bài:** GitHub PR (hệ thống chấm điểm tự động `devlinux-reviewer`, ngưỡng ≥80 Pass / 60–79 Pass with revisions / <60 Fail)

**Thiết bị:** Raspberry Pi Zero W thật.

**Mô phỏng sản phẩm thật:** đồng hồ báo thức thông minh tích hợp thời tiết (kiểu Xiaomi/Lenovo smart clock).

**Kiến thức áp dụng:** Character Device Driver (GPIO input ngắt cho nút nhấn, GPIO output cho buzzer), Device Tree overlay, I2C (OLED SSD1306), Thread + đồng bộ hoá, Socket/HTTP (webserver cục bộ + HTTP client), quản lý wifi (hostapd + wpa_supplicant), Signal/Timer, File I/O (lưu cấu hình báo thức), systemd.

> Quy ước ID requirement (`P2-M1`, `P2-N1`...) dùng chung giữa đề bài này, `DESIGN.md` học viên nộp, và rubric chấm điểm — xem thêm quy định chung ở `00_Chung/QUY_DINH_CHUNG.md`.

---

## Problem Statement
Xây dựng đồng hồ báo thức thông minh chạy trên Pi Zero W, có khả năng tự cấu hình wifi lần đầu qua Soft AP, hiển thị giờ hệ thống + xem thời tiết trên màn hình OLED, và cấu hình báo thức qua giao diện web.

**Yêu cầu tổng quan:**
- **Nguồn thời gian:** dùng **giờ hệ thống** (đồng bộ qua NTP một khi đã có wifi), không cần module RTC riêng.
- **Nút nhấn vật lý + màn hình SSD1306 (I2C):** 1 nút bấm dùng cho nhiều mục đích (chuyển màn hình, vào chế độ cấu hình wifi, tắt báo thức), 1 màn hình OLED hiển thị thông tin.
- **Chế độ Smart Config (cấu hình wifi lần đầu qua Soft AP):**
  - Giữ nút trên **5 giây** sẽ đưa đồng hồ vào **chế độ Soft AP**: đồng hồ tắt kết nối wifi hiện tại (nếu có) và tự phát ra một mạng wifi riêng.
  - Điện thoại/laptop kết nối vào wifi này, truy cập được **giao diện web cục bộ** hiển thị 2 ô nhập (username/password wifi nhà) và nút OK.
  - Khi bấm OK, đồng hồ nhận thông tin, thoát Soft AP, thử kết nối vào wifi vừa nhập; nếu sai password, tự quay lại Soft AP để thử lại thay vì treo máy.
- **Sau khi kết nối wifi thành công**, màn hình hỗ trợ **2 chế độ hiển thị**, chuyển qua lại bằng cách nhấn nút (nhấn ngắn):
  1. **Màn hình đồng hồ:** hiển thị trạng thái kết nối wifi + giờ hiện tại dạng `HH:MM:SS`, số giây phải nhảy đúng theo từng giây thực (cập nhật realtime, không đứng hình).
  2. **Màn hình thời tiết:** hiển thị dữ liệu thời tiết. Lấy dữ liệu thời tiết mới **ngay khi người dùng chuyển sang màn hình này**; nếu người dùng ở lại màn hình này lâu, tự động lấy lại dữ liệu **mỗi 5 phút**.
- **Webserver cấu hình báo thức:** cung cấp giao diện web để người dùng đặt giờ báo thức. Chỉ hỗ trợ **1 lịch báo thức duy nhất** — đặt lại sẽ **ghi đè** lịch cũ.
- **Báo thức:** đến đúng giờ đã đặt, kêu chuông qua **buzzer nối GPIO**. Trong lúc chuông đang kêu, **nhấn nút vật lý sẽ tắt chuông ngay**.

## Functional Requirements

**Must-have:**
- **P2-M1.** Driver char device tự viết cho nút nhấn (GPIO interrupt, không polling), phân biệt được nhấn ngắn và nhấn giữ ≥5 giây; kết hợp cơ chế chuyển đổi Soft AP ↔ Station mode (`hostapd`/`wpa_supplicant`) để cấu hình wifi lần đầu qua giao diện web khi giữ nút đủ 5 giây.
- **P2-M2.** Driver char device tự viết điều khiển buzzer (GPIO output), bật/tắt được từ chương trình chính.
- **P2-M3.** Đồng bộ giờ hệ thống qua NTP sau khi kết nối wifi thành công (dùng `systemd-timesyncd` hoặc tương đương).
- **P2-M4.** Màn hình đồng hồ: hiển thị `HH:MM:SS` cập nhật đúng từng giây (không lệch, không giật hình khi refresh) và trạng thái kết nối wifi.
- **P2-M5.** Màn hình thời tiết: gọi lấy dữ liệu thời tiết ngay khi vừa chuyển vào màn hình này; nếu người dùng vẫn đứng ở màn hình này, tự làm mới dữ liệu mỗi 5 phút; xử lý được trường hợp mất kết nối khi gọi API/mock server mà không crash hay treo màn hình.
- **P2-M6.** Nhấn nút (nhấn ngắn) khi đang ở 1 trong 2 màn hình sẽ chuyển sang màn hình còn lại.
- **P2-M7.** Webserver cục bộ cho phép nhập giờ báo thức (giờ:phút) và lưu; lưu đè lên cấu hình báo thức cũ (chỉ 1 lịch duy nhất); cấu hình được lưu ra file để giữ nguyên qua các lần khởi động lại.
- **P2-M8.** Khi thời gian hệ thống khớp giờ báo thức đã lưu: bật buzzer kêu liên tục.
- **P2-M9.** Trong lúc buzzer đang kêu, nhấn nút vật lý (nhấn ngắn) sẽ tắt buzzer ngay lập tức; sau khi tắt, quay lại đúng luồng nút nhấn bình thường (chuyển màn hình) cho các lần nhấn tiếp theo.

**Nice-to-have:**
- **P2-N1.** Cho phép chọn lặp lại báo thức theo ngày trong tuần (dù vẫn chỉ 1 lịch).
- **P2-N2.** Snooze báo thức (nhấn giữ thay vì nhấn ngắn để snooze thay vì tắt hẳn).
- **P2-N3.** Hiển thị thêm biểu tượng nhỏ trên màn hình khi báo thức đang được bật/đã cấu hình.

## Design Hints (lưu ý kỹ thuật, tránh bẫy thường gặp)
- Vì dùng chung nút nhấn cho 3 mục đích (chuyển màn hình / vào Smart Config / tắt báo thức), cần phân biệt rõ ràng theo ngữ cảnh: khi buzzer đang kêu → nhấn = tắt báo thức (ưu tiên cao nhất); khi không kêu → nhấn ngắn = chuyển màn hình, nhấn giữ ≥5s = Smart Config. Logic phân loại này nên đặt ở tầng userspace, driver chỉ báo sự kiện thô (press/release + timestamp).
- Việc hiển thị giây nhảy đúng từng giây nên dùng timer riêng (`timerfd` hoặc thread với `nanosleep`/`clock_nanosleep`) thay vì `sleep(1)` để tránh trôi giờ theo thời gian.
- Việc lấy dữ liệu thời tiết mỗi 5 phút khi đứng ở màn hình thời tiết nên dùng cơ chế non-blocking (thread riêng hoặc `epoll` với `timerfd`), không được chặn (block) việc cập nhật giờ ở màn hình đồng hồ hay phản hồi nút nhấn.
- Việc chuyển đổi giữa `hostapd` và `wpa_supplicant` cần đảm bảo không xung đột (phải dừng hẳn service này trước khi bật service kia), và cần xử lý được trường hợp kết nối wifi thất bại (sai password) để quay lại Soft AP thử lại thay vì treo chương trình.
- Không hardcode số chân GPIO cho nút và buzzer — khai báo qua Device Tree overlay.

## Gợi ý thiết kế cho học viên

**Gợi ý kiến trúc tổng thể:** hình dung chương trình gồm 1 tiến trình chính, bên trong có nhiều thread chạy song song, tất cả chia sẻ 1 "trạng thái hệ thống" chung (struct chứa: đang ở Soft AP hay Station, SSID hiện tại, IP hiện tại, `current_screen` là CLOCK hay WEATHER, `alarm_ringing` là bool) được bảo vệ bởi mutex:
- **Thread Button:** lắng nghe sự kiện thô (press/release + timestamp) từ driver, tự phân loại theo ngữ cảnh: nếu `alarm_ringing == true` → nhấn ngắn = tắt báo thức (ưu tiên cao nhất, kiểm tra điều kiện này trước); nếu không → nhấn ngắn = đổi `current_screen`, nhấn giữ ≥5s = chuyển sang Soft AP.
- **Thread Clock:** dùng `timerfd` bắn tín hiệu mỗi giây, mỗi lần tín hiệu tới cập nhật lại `HH:MM:SS` và so sánh với giờ báo thức đã lưu để quyết định bật `alarm_ringing` hay không — gộp việc tính giờ và kiểm tra báo thức vào cùng 1 thread để tránh phải đồng bộ giờ ở 2 nơi.
- **Thread Weather:** chỉ "thức dậy" khi `current_screen == WEATHER`, gọi API rồi ngủ tiếp; không chạy nền liên tục kể cả khi đang ở màn hình đồng hồ.
- **Thread Buzzer:** đơn giản, chỉ đọc `alarm_ringing` và bật/tắt GPIO buzzer tương ứng.
- **Thread Webserver:** vòng lặp `accept` chính; khi nhận request (submit wifi, đặt báo thức), nó sửa "trạng thái hệ thống" hoặc file cấu hình rồi trả response — không tự thực hiện việc kết nối wifi ngay trong thread này.

### Chi tiết kỹ thuật — Soft AP (Smart Config)

Về bản chất, "phát wifi" trên Linux là chuyển `wlan0` từ chế độ **station** (client) sang chế độ **AP**, cần 3 thành phần phối hợp:

| Thành phần | Vai trò |
|---|---|
| `hostapd` | Biến `wlan0` thành access point, phát ra SSID |
| `dnsmasq` | Cấp DHCP cho điện thoại khi kết nối vào (nếu không có, điện thoại không tự có IP để truy cập webserver) |
| `wpa_supplicant` | Dùng khi ở chế độ station, kết nối `wlan0` ra wifi nhà |

**Luồng chuyển đổi gợi ý (Station → Soft AP), thực hiện bằng lệnh gọi qua `fork()+execvp()` từ chương trình chính:**
```
1. systemctl stop wpa_supplicant@wlan0     # ngắt kết nối station hiện tại (nếu có)
2. ip addr add 192.168.4.1/24 dev wlan0    # gán IP tĩnh cho wlan0 khi đóng vai AP
3. systemctl start hostapd                 # bắt đầu phát SSID (đọc cấu hình từ /etc/hostapd/hostapd.conf)
4. systemctl start dnsmasq                 # cấp DHCP cho client kết nối vào
```
Mẫu tối thiểu `/etc/hostapd/hostapd.conf` (SSID nên sinh động theo device ID để tránh trùng giữa các thiết bị của các học viên khác nhau):
```
interface=wlan0
driver=nl80211
ssid=DevLinux_Clock_<device_id>
hw_mode=g
channel=6
auth_algs=1
wpa=0
```
Mẫu tối thiểu `/etc/dnsmasq.conf`:
```
interface=wlan0
dhcp-range=192.168.4.2,192.168.4.20,255.255.255.0,24h
```

**Luồng chuyển ngược lại (Soft AP → Station) sau khi nhận được SSID/password wifi nhà từ web form:**
```
1. systemctl stop hostapd
2. systemctl stop dnsmasq
3. Ghi lại /etc/wpa_supplicant/wpa_supplicant.conf với network={ ssid="..." psk="..." }
4. systemctl start wpa_supplicant@wlan0
5. dhclient wlan0   (hoặc udhcpc)          # xin IP từ router nhà qua DHCP
6. Kiểm tra kết nối thành công: đọc `wpa_cli status` (tìm dòng wpa_state=COMPLETED) hoặc `iw wlan0 link`
```
Nếu bước 6 không thành công trong X giây (ví dụ 15-20s) → coi là sai password, tự động quay lại luồng Soft AP (bước "Station → Soft AP" ở trên).

**Lưu ý quan trọng:** các lệnh `systemctl`/`ip`/`dhclient` gọi từ code C nên dùng `fork()` + `execvp()` thay vì `system()` để kiểm soát được exit code chính xác hơn.

### Chi tiết kỹ thuật — Webserver (cấu hình wifi + đặt báo thức)

Vì chỉ có 1 người dùng thao tác tại 1 thời điểm, **không cần epoll/non-blocking phức tạp** — 1 vòng lặp `accept()` chặn (blocking), xử lý tuần tự từng kết nối là đủ.

**Khung chương trình gợi ý:**
```c
int server_fd = socket(AF_INET, SOCK_STREAM, 0);
bind(server_fd, ...);   // bind port 8080
listen(server_fd, 5);

while (1) {
    int client_fd = accept(server_fd, ...);
    char buf[4096];
    int n = read(client_fd, buf, sizeof(buf) - 1);
    // parse dòng đầu: "GET /path HTTP/1.1" hoặc "POST /path HTTP/1.1"
    // nếu POST: đọc header Content-Length, đọc tiếp đủ số byte đó làm body
    // routing:
    //   GET  /               -> trả HTML form nhập wifi (nếu đang Soft AP) hoặc trang đặt báo thức (nếu Station)
    //   POST /wifi-config    -> parse body "ssid=...&password=...", cập nhật trạng thái hệ thống
    //   POST /alarm-config   -> parse body "hour=...&minute=...", ghi đè file cấu hình báo thức
    send(client_fd, response, strlen(response), 0);
    close(client_fd);
}
```
**Parse HTTP tối thiểu cần có:**
- Dòng đầu: tách theo khoảng trắng để lấy `METHOD` và `PATH`.
- Với `POST`: tìm header `Content-Length: N`, đọc đúng N byte tiếp theo làm body (có thể cần đọc nhiều lần `read()` nếu dữ liệu chưa về hết trong 1 lần gọi).
- Parse dữ liệu form `application/x-www-form-urlencoded`: tách theo `&`, mỗi phần tách theo `=`, decode `%XX` thành ký tự tương ứng, decode `+` thành khoảng trắng.
- HTML trang web có thể để dạng **hằng chuỗi C** nhúng thẳng trong code — đơn giản và đủ dùng cho 2 trang (form nhập wifi, trang đặt báo thức).
- Response luôn trả đủ 3 phần: status line, header tối thiểu (`Content-Type`, `Content-Length`, `Connection: close`), rồi tới body.

**Kỹ thuật nên dùng / không nên dùng:**
- ✅ Nên: tự viết parser HTTP thô bằng `read`/`write` socket thuần.
- ✅ Nên: dùng `fork()+execvp()` khi cần gọi lệnh hệ thống để kiểm soát exit code.
- ❌ Không nên: dùng framework web nặng (Flask, Express...).
- ❌ Không cần: TLS/HTTPS.
- ❌ Không nên: dùng `libgpiod`/sysfs GPIO thay cho driver tự viết.

### Chi tiết kỹ thuật — Timer hiển thị giây chính xác

Đây là điểm hay bị làm sai nhất: dùng `sleep(1)` rồi `i++` để đếm giây sẽ **trôi dần theo thời gian** (vì `sleep(1)` không đảm bảo đúng 1000ms tuyệt đối, còn cộng dồn cả thời gian xử lý vẽ màn hình). Cách đúng:
```c
int timer_fd = timerfd_create(CLOCK_MONOTONIC, 0);
struct itimerspec its = { .it_value = {1, 0}, .it_interval = {1, 0} }; // bắn mỗi 1 giây
timerfd_settime(timer_fd, 0, &its, NULL);

while (1) {
    uint64_t expirations;
    read(timer_fd, &expirations, sizeof(expirations)); // block tới khi tick tiếp theo
    time_t now = time(NULL);           // lấy giờ THẬT từ hệ thống mỗi lần tick
    struct tm *t = localtime(&now);
    // vẽ lại HH:MM:SS từ t, KHÔNG tự cộng dồn biến đếm riêng
}
```
Điểm mấu chốt: **luôn lấy giờ thật từ `time()`/`clock_gettime()` mỗi lần vẽ**, không tự đếm cộng dồn — như vậy dù thread có bị trễ đôi chút do xử lý khác, giá trị hiển thị vẫn luôn đúng với đồng hồ hệ thống, không bị trôi dần.

### Chi tiết kỹ thuật — Gọi API thời tiết (HTTP Client)

Khác với webserver (đồng hồ đóng vai trò server), phần này đồng hồ đóng vai trò **client** gọi ra 1 mock weather server (do học viên tự dựng, có thể là 1 script Python/Node đơn giản trả JSON). Khung gợi ý:
```c
int sock = socket(AF_INET, SOCK_STREAM, 0);
connect(sock, (mock server address), ...);
char *request = "GET /weather HTTP/1.1\r\nHost: mockserver\r\nConnection: close\r\n\r\n";
send(sock, request, strlen(request), 0);
char response[4096];
int n = recv(sock, response, sizeof(response) - 1, 0);
// tìm "\r\n\r\n" để tách header và body, body chính là JSON trả về
// parse JSON tối thiểu bằng tay (ví dụ tìm chuỗi "temp":) nếu không muốn dùng thư viện JSON ngoài
```
**Kỹ thuật nên dùng:** đặt `SO_RCVTIMEO`/`SO_SNDTIMEO` trên socket này (ví dụ timeout 5 giây) để tránh treo màn hình vô thời hạn nếu mock server không phản hồi — đúng yêu cầu P2-M5 "không crash hay treo màn hình" khi mất kết nối.

**Gợi ý thứ tự nên làm:**
1. **Tuần 1:** viết xong driver nút nhấn (đã phân loại nhấn ngắn/giữ) và driver buzzer, test độc lập bằng script gọi `ioctl` tay. Bắt đầu luồng Soft AP (P2-M1): giữ nút 5s → phát wifi → nhập form → kết nối.
2. **Tuần 2:** làm màn hình đồng hồ chạy đúng giây bằng `timerfd` (P2-M3, M4) — đây là phần dễ kiểm chứng nhất bằng mắt thường, nên làm sớm để tự tin phần nền tảng ổn.
3. **Tuần 3:** thêm màn hình thời tiết (viết mock weather server đơn giản trước, rồi mới viết HTTP client gọi tới) + chuyển màn hình bằng nút (P2-M5, M6), rồi tới webserver báo thức + buzzer (P2-M7, M8, M9) — để phần khó nhất (logic phân loại nút nhấn 3 ngữ cảnh) vào cuối khi đã quen code base.
4. **Tuần 4:** test kỹ chuỗi "buzzer kêu → tắt → nhấn lại vẫn chuyển màn hình" (đây là case hay bị lỗi nhất), viết README, hoàn thiện `docs/test_report.md`.

**Gợi ý test thủ công khi code:** dùng `date -s` để chỉnh giờ hệ thống gần sát giờ báo thức, không cần chờ thật lâu để test buzzer kêu. Mock weather server có thể viết bằng 5-10 dòng Python (`http.server` + trả JSON cứng) để không tốn công, trọng tâm bài không nằm ở chỗ này.

## Expected Output
- Video demo: giữ nút 5 giây → phát wifi → nhập wifi nhà qua giao diện web → kết nối thành công; ở màn hình đồng hồ thấy giây nhảy đều; nhấn nút chuyển sang màn hình thời tiết, thấy dữ liệu load ngay lúc chuyển vào; đặt báo thức qua web; đến giờ đã đặt buzzer kêu; nhấn nút tắt được chuông ngay.
- Tắt nguồn, bật lại: đồng hồ vẫn giữ đúng cấu hình wifi (không cần Smart Config lại) và đúng giờ báo thức đã lưu.

## Submission
- GitHub PR theo nhánh `embedded-linux/<course>/<student>/session-XX`, tuân thủ đúng **Cấu trúc bàn giao chuẩn** bên dưới. README mô tả rõ luồng xử lý nút nhấn theo từng ngữ cảnh (chuyển màn hình / Smart Config / tắt báo thức).
- Video demo đặt trong `docs/demo/` (hoặc link trong `docs/demo/demo_links.txt`) — chỉ để mentor xem, không dùng để chấm tự động.

## Cấu trúc bàn giao chuẩn — Project 2
```
<student-repo>/
├── DESIGN.md
├── README.md
├── src/
│   ├── driver/
│   │   ├── btn_driver.c           # driver nút nhấn đa chức năng (P2-M1)
│   │   └── buzzer_driver.c        # driver buzzer (P2-M2)
│   ├── app/
│   │   ├── smartconfig.*          # logic Soft AP ↔ Station (P2-M1)
│   │   ├── clock_screen.*         # màn hình đồng hồ (P2-M4)
│   │   ├── weather_screen.*       # màn hình thời tiết (P2-M5)
│   │   ├── webserver.*            # cấu hình wifi + cấu hình báo thức (P2-M1, P2-M7)
│   │   └── alarm_manager.*        # so giờ + kích buzzer + tắt bằng nút (P2-M8, P2-M9)
│   └── Makefile
├── devicetree/                    # overlay GPIO nút/buzzer + địa chỉ I2C OLED
├── systemd/
└── docs/
    ├── test_report.md             # kết quả tự chạy TESTCASES_P2_student.md (bắt buộc, có log)
    └── demo/                      # video: giây nhảy đúng, chuyển màn hình, báo thức kêu/tắt (chỉ mentor xem)
```

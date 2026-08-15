# PROJECT 1: IoT Gateway SmartHome (Local Smart Home Gateway)

**Thời lượng:** 4 tuần | **Độ khó:** Trung bình | **Nộp bài:** GitHub PR (hệ thống chấm điểm tự động `devlinux-reviewer`, ngưỡng ≥80 Pass / 60–79 Pass with revisions / <60 Fail)

**Thiết bị:** Raspberry Pi Zero W thật.

**Mô phỏng sản phẩm thật:** gateway nhà thông minh dạng Sonoff/Tuya Gateway — có cơ chế Smart Config (soft AP) để cấu hình wifi lần đầu, đèn báo trạng thái, màn hình hiển thị, và webserver điều khiển thiết bị.

**Kiến thức áp dụng:** Character Device Driver (GPIO input ngắt cho nút nhấn, GPIO output cho LED và 4 relay), Device Tree overlay, I2C (OLED SSD1306), Thread + đồng bộ hoá, Socket/HTTP (webserver cục bộ), quản lý wifi (hostapd + wpa_supplicant), systemd.

> Quy ước ID requirement (`P1-M1`, `P1-N1`...) dùng chung giữa đề bài này, `DESIGN.md` học viên nộp, và rubric chấm điểm — xem thêm quy định chung ở `00_Chung/QUY_DINH_CHUNG.md`.

---

## Problem Statement
Xây dựng một gateway nhà thông minh chạy trên Pi Zero W với 4 nhóm tính năng:

**1. Chế độ Smart Config (cấu hình wifi qua soft AP):**
- Có 1 nút nhấn vật lý. Giữ nút trên **5 giây** sẽ đưa gateway vào **chế độ Soft AP**: gateway tắt kết nối wifi client hiện tại (nếu có) và tự phát ra một mạng wifi riêng (SSID cố định hoặc theo device ID).
- Điện thoại/laptop kết nối vào wifi này, sau đó truy cập được một **giao diện web cục bộ** (chạy trên gateway) hiển thị 2 ô nhập (username/password của wifi nhà) và 1 nút OK.
- Khi bấm OK, gateway nhận thông tin, thoát chế độ soft AP, và thử kết nối vào wifi vừa nhập.

**2. LED báo trạng thái kết nối wifi:**
- Đang trong quá trình kết nối wifi: LED **nháy chu kỳ 500ms**.
- Kết nối thành công: LED **sáng liên tục**.
- Không có kết nối: LED **tắt**.
- Khi khởi động gateway: LED sáng ngay nếu đã có sẵn kết nối wifi đã lưu, tắt nếu chưa/không kết nối được.

**3. Màn hình SSD1306 (I2C):**
- Khi đã kết nối wifi: hiển thị tên wifi đang kết nối (SSID) và địa chỉ IP hiện tại của gateway.
- Khi đang ở chế độ Soft AP: hiển thị tên wifi mà gateway đang phát ra để người dùng biết cần kết nối vào mạng nào.

**4. Điều khiển thiết bị qua webserver:**
- Khi gateway đã kết nối wifi thành công, chạy 1 webserver cục bộ cung cấp giao diện điều khiển gồm **4 nút bấm trên web**, mỗi nút điều khiển bật/tắt (ON/OFF) 1 trong 4 GPIO (relay) vật lý trên gateway.

## Functional Requirements

**Must-have:**
- **P1-M1.** Driver char device tự viết cho nút nhấn Smart Config, dùng **GPIO interrupt** (không polling) để phát hiện nhấn giữ ≥5 giây.
- **P1-M2.** Driver char device tự viết điều khiển LED trạng thái (GPIO output), hỗ trợ 3 trạng thái: nháy 500ms / sáng liên tục / tắt.
- **P1-M3.** Driver char device tự viết điều khiển 4 GPIO relay độc lập (ON/OFF từng cái qua `ioctl` hoặc `write`).
- **P1-M4.** Cơ chế chuyển đổi Soft AP ↔ Station mode: dùng `hostapd` (phát AP) khi vào Smart Config, và `wpa_supplicant` (kết nối client) sau khi có thông tin wifi — chương trình điều khiển việc bật/tắt 2 service này.
- **P1-M5.** Webserver cục bộ nhỏ (tự viết bằng socket TCP, không bắt buộc dùng framework) phục vụ:
   - Giao diện nhập username/password wifi khi ở chế độ Soft AP.
   - Giao diện 4 nút ON/OFF điều khiển relay khi đã ở chế độ Station (đã kết nối wifi nhà).
- **P1-M6.** Lưu thông tin wifi (SSID/password) vào file cấu hình, khi khởi động lại tự kết nối lại wifi đã lưu mà không cần vào lại Smart Config.
- **P1-M7.** Cập nhật màn hình SSD1306 đúng theo 2 trạng thái mô tả ở trên (Station: SSID + IP | Soft AP: SSID đang phát).
- **P1-M8.** LED phản ánh đúng 3 trạng thái theo thời gian thực trong suốt vòng đời chạy của gateway.
- **P1-M9.** Chạy dưới dạng systemd service, tự khởi động cùng hệ thống.

**Nice-to-have:**
- **P1-N1.** Timeout tự thoát Soft AP nếu không ai cấu hình sau X phút, quay lại trạng thái cũ.
- **P1-N2.** Xác thực đơn giản (mật khẩu) cho trang web điều khiển relay.
- **P1-N3.** Hiển thị thêm trạng thái từng relay (ON/OFF) ngay trên màn hình OLED.
- **P1-N4.** Lưu lịch sử bật/tắt relay ra file log.

## Design Hints (lưu ý kỹ thuật, tránh bẫy thường gặp)
- Việc phát hiện "giữ nút ≥5 giây" nên xử lý trong driver hoặc trong userspace dựa trên timestamp giữa sự kiện ngắt cạnh lên/cạnh xuống (rising/falling edge) của GPIO, không dùng `sleep`/polling để đo thời gian giữ.
- Không hardcode số chân GPIO cho nút, LED, hay 4 relay — khai báo qua Device Tree overlay.
- Việc chuyển đổi giữa `hostapd` và `wpa_supplicant` cần đảm bảo không xung đột (phải dừng hẳn service này trước khi bật service kia), và cần xử lý được trường hợp kết nối wifi thất bại (sai password) để quay lại Soft AP thử lại thay vì treo chương trình.
- Webserver không cần framework phức tạp — 1 vòng lặp `accept`/`recv`/`send` xử lý HTTP request thô (parse GET/POST đơn giản) là đủ, trọng tâm chấm điểm là driver + quản lý trạng thái, không phải web framework.
- Đồng bộ giữa thread xử lý ngắt nút, thread điều khiển LED, và thread/tiến trình webserver cần dùng mutex/condition variable rõ ràng, tránh race condition khi đọc/ghi trạng thái dùng chung.

## Gợi ý thiết kế cho học viên

**Gợi ý kiến trúc tổng thể:** hình dung chương trình gồm 1 tiến trình chính, bên trong có 3-4 thread chạy song song, tất cả chia sẻ 1 "trạng thái hệ thống" chung (struct chứa: đang ở Soft AP hay Station, SSID hiện tại, IP hiện tại, trạng thái từng relay) được bảo vệ bởi mutex:
- **Thread 1 — Button:** chỉ làm 1 việc: lắng nghe ngắt GPIO của nút, đo thời gian giữ, khi đủ 5s thì đổi "trạng thái hệ thống" sang Soft AP.
- **Thread 2 — LED:** đọc "trạng thái hệ thống" liên tục, tự quyết định nháy/sáng/tắt tương ứng — không cần ai gọi nó, nó tự polling trạng thái mỗi 100-200ms là đủ mượt.
- **Thread 3 — Display (OLED):** tương tự thread LED, tự đọc trạng thái và vẽ lại màn hình mỗi khi có gì thay đổi (hoặc định kỳ vài trăm ms).
- **Thread 4 — Webserver:** vòng lặp `accept` chính; khi nhận request thay đổi (submit wifi, bấm relay), nó **sửa "trạng thái hệ thống"** rồi trả response — không tự thực hiện việc kết nối wifi ngay trong thread này (tránh block cả webserver khi wifi đang connect chậm).

### Chi tiết kỹ thuật — Soft AP (Smart Config)

Về bản chất, "phát wifi" trên Linux là chuyển `wlan0` từ chế độ **station** (client) sang chế độ **AP**, cần 3 thành phần phối hợp:

| Thành phần | Vai trò |
|---|---|
| `hostapd` | Biến `wlan0` thành access point, phát ra SSID |
| `dnsmasq` | Cấp DHCP cho điện thoại khi kết nối vào (nếu không có, điện thoại không tự có IP để truy cập webserver) |
| `wpa_supplicant` | Dùng khi ở chế độ station, kết nối `wlan0` ra wifi nhà |

**Luồng chuyển đổi gợi ý (Station → Soft AP), thực hiện bằng lệnh gọi qua `system()`/`fork+exec` từ chương trình chính:**
```
1. systemctl stop wpa_supplicant@wlan0     # ngắt kết nối station hiện tại (nếu có)
2. ip addr add 192.168.4.1/24 dev wlan0    # gán IP tĩnh cho wlan0 khi đóng vai AP
3. systemctl start hostapd                 # bắt đầu phát SSID (đọc cấu hình từ /etc/hostapd/hostapd.conf)
4. systemctl start dnsmasq                 # cấp DHCP cho client kết nối vào
```
Mẫu tối thiểu `/etc/hostapd/hostapd.conf` (SSID nên sinh động theo device ID để tránh trùng giữa các gateway của các học viên khác nhau):
```
interface=wlan0
driver=nl80211
ssid=DevLinux_GW_<device_id>
hw_mode=g
channel=6
auth_algs=1
wpa=0
```
Mẫu tối thiểu `/etc/dnsmasq.conf` (chỉ định phạm vi cấp IP):
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

**Lưu ý quan trọng:** các lệnh `systemctl`/`ip`/`dhclient` gọi từ code C nên dùng `fork()` + `execvp()` thay vì `system()` để kiểm soát được exit code chính xác hơn (biết chắc lệnh có chạy thành công hay không), tránh dùng `system()` khi cần biết kết quả để quyết định bước tiếp theo.

### Chi tiết kỹ thuật — Webserver

Vì chỉ có 1 người dùng thao tác tại 1 thời điểm (người đang cấu hình gateway), **không cần epoll/non-blocking phức tạp** — 1 vòng lặp `accept()` chặn (blocking), xử lý tuần tự từng kết nối là đủ, và cũng đúng trọng tâm bài (trọng tâm chấm điểm là driver + quản lý trạng thái, không phải kỹ thuật I/O multiplexing).

**Khung chương trình gợi ý:**
```c
int server_fd = socket(AF_INET, SOCK_STREAM, 0);
bind(server_fd, ...);   // bind port 8080 (không cần port 80, tránh cần quyền root)
listen(server_fd, 5);

while (1) {
    int client_fd = accept(server_fd, ...);
    char buf[4096];
    int n = read(client_fd, buf, sizeof(buf) - 1);
    // parse dòng đầu tiên: "GET /path HTTP/1.1" hoặc "POST /path HTTP/1.1"
    // nếu POST: đọc header Content-Length, đọc tiếp đủ số byte đó làm body
    // routing đơn giản theo path, ví dụ:
    //   GET  /            -> trả HTML form nhập wifi (nếu đang Soft AP) hoặc trang điều khiển relay (nếu Station)
    //   POST /wifi-config -> parse body "ssid=...&password=...", cập nhật trạng thái hệ thống
    //   POST /relay?id=1&state=on -> đổi trạng thái relay tương ứng
    send(client_fd, response, strlen(response), 0);
    close(client_fd);
}
```
**Parse HTTP tối thiểu cần có:**
- Dòng đầu: tách theo khoảng trắng để lấy `METHOD` và `PATH`.
- Với `POST`: tìm header `Content-Length: N`, sau đó đọc đúng N byte tiếp theo làm body (chú ý: có thể cần đọc nhiều lần `read()` nếu dữ liệu chưa về hết trong 1 lần gọi).
- Parse dữ liệu form `application/x-www-form-urlencoded`: tách theo `&`, mỗi phần tách theo `=`, decode `%XX` thành ký tự tương ứng (ví dụ `%40` → `@`), decode `+` thành khoảng trắng.
- HTML trang web có thể để dạng **hằng chuỗi C** nhúng thẳng trong code (không cần file `.html` riêng, không cần template engine) — đơn giản và đủ dùng cho 2 trang (form nhập wifi, trang điều khiển relay).
- Response luôn trả đủ 3 phần: status line (`HTTP/1.1 200 OK`), header tối thiểu (`Content-Type: text/html`, `Content-Length: <n>`, `Connection: close`), rồi tới body — thiếu `Content-Length` nhiều trình duyệt sẽ chờ mãi không hiển thị.

**Kỹ thuật nên dùng / không nên dùng:**
- ✅ Nên: tự viết parser HTTP thô bằng `read`/`write` socket thuần — đúng trọng tâm học của bài (Socket/HTTP cơ bản).
- ✅ Nên: dùng `fork()+execvp()` khi cần gọi lệnh hệ thống (`systemctl`, `ip`, `dhclient`) để kiểm soát được exit code.
- ❌ Không nên: dùng framework web nặng (Flask, Express...) — sai tinh thần bài (bài yêu cầu C/hệ thống, không phải web dev).
- ❌ Không cần: TLS/HTTPS — mạng Soft AP là mạng riêng tạm thời, không cần mã hoá.
- ❌ Không nên: dùng `libgpiod`/sysfs GPIO thay cho driver tự viết (nhắc lại vì đây là lỗi hay gặp nhất khi học viên muốn "làm nhanh" phần webserver rồi quên mất yêu cầu cốt lõi vẫn là driver).

**Gợi ý thứ tự nên làm (khớp với lộ trình 4 tuần):**
1. **Tuần 1:** viết xong 3 driver (`btn`, `led`, `relay`) trước, test độc lập bằng script nhỏ gọi `ioctl` tay — chưa cần webserver/wifi gì cả. Đây là phần "chắc điểm" nhất vì không phụ thuộc phần nào khác.
2. **Tuần 2:** làm luồng Soft AP → nhập wifi → kết nối Station (P1-M4, M5, M6) — đây là phần khó nhất, nên làm sớm để còn thời gian debug. Test riêng phần chuyển đổi `hostapd`/`wpa_supplicant` bằng tay qua terminal trước khi gọi từ code.
3. **Tuần 3:** ghép LED + OLED phản ánh đúng trạng thái (P1-M7, M8), thêm webserver điều khiển relay (phần còn lại của M5), viết `systemd` service.
4. **Tuần 4:** làm Nice-to-have nếu còn thời gian, viết README, chạy đủ Test Case, hoàn thiện `docs/test_report.md`.

**Gợi ý công cụ/thư viện có thể dùng:** `libgpiod` chỉ dùng để **test nhanh khi phát triển driver**, bài chấm điểm vẫn yêu cầu driver char device tự viết cho sản phẩm cuối — không nộp bản dùng `libgpiod` thay driver. `hostapd`/`wpa_supplicant`/`dnsmasq` đã có sẵn trên hầu hết bản Yocto/Raspbian, chỉ cần cấu hình file `.conf` đúng và thêm vào image nếu build Yocto (kiểm tra `IMAGE_INSTALL` đã có các package này chưa).

## Expected Output
- Video demo: giữ nút 5 giây → gateway phát wifi → điện thoại kết nối, nhập wifi nhà qua giao diện web → gateway kết nối thành công → LED chuyển từ nháy sang sáng → màn hình hiển thị đúng SSID/IP → vào trang web điều khiển 4 relay, bật/tắt từng cái thấy relay phản hồi đúng.
- Tắt nguồn, bật lại: gateway tự kết nối lại đúng wifi đã lưu mà không cần cấu hình lại.

## Submission
- GitHub PR theo nhánh `embedded-linux/<course>/<student>/session-XX`, tuân thủ đúng **Cấu trúc bàn giao chuẩn** bên dưới.
- Video demo đầy đủ cả 2 luồng (Soft AP config lần đầu, và khởi động lại tự kết nối) đặt trong `docs/demo/` (hoặc link trong `docs/demo/demo_links.txt`) trên phần cứng thật — chỉ để mentor xem, không dùng để chấm tự động.

## Cấu trúc bàn giao chuẩn — Project 1
```
<student-repo>/
├── DESIGN.md                      # nộp ở Design Gate, trước khi có code
├── README.md                      # hướng dẫn build & flash + mục "Cấu trúc thư mục"
├── src/
│   ├── driver/
│   │   ├── btn_driver.c           # driver nút nhấn Smart Config (P1-M1)
│   │   ├── led_driver.c           # driver LED trạng thái (P1-M2)
│   │   └── relay_driver.c         # driver 4 relay (P1-M3)
│   ├── app/
│   │   ├── smartconfig.c/.py/...  # logic Soft AP ↔ Station (P1-M4)
│   │   ├── webserver.*            # webserver config wifi + điều khiển relay (P1-M5)
│   │   └── oled_display.*         # cập nhật OLED (P1-M7)
│   └── Makefile
├── devicetree/                    # overlay khai báo GPIO nút/LED/relay + địa chỉ I2C OLED
├── systemd/                       # vd: devlinux-gateway.service (P1-M9)
└── docs/
    ├── test_report.md             # kết quả tự chạy TESTCASES_P1_student.md (bắt buộc, có log)
    └── demo/                      # video/ảnh demo (chỉ mentor xem), hoặc demo_links.txt
```

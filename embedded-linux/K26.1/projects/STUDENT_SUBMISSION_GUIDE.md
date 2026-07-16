# Hướng Dẫn Nộp Project — DevLinux 2-Gate Review System

**Tài liệu này hướng dẫn học viên cách nộp 4 project cuối khoá (P1-P4) trong hệ thống DevLinux.**

---

## Overview — 2-Gate Review Là Gì?

Mỗi project cuối khoá phải qua **2 cổng review độc lập**:

```
┌──────────────────────────┐
│  Gate 1: DESIGN REVIEW   │
│  (Tuần 1-2)              │
│  Branch: .../design      │
│  Claude review design    │
│  Teacher bảo sửa → ≥80   │
│  → Approved ✅           │
└────────────┬─────────────┘
             │
             ↓ Student creates code branch from design
             │
┌──────────────────────────┐
│  Gate 2: CODE REVIEW     │
│  (Tuần 3-8)              │
│  Branch: .../code        │
│  (từ design branch)      │
│  Claude review code      │
│  Teacher review tests    │
│  → PASS ✅               │
└──────────────────────────┘
```

### Điều kiện bắt buộc:
- ✅ **Phải đạt design ≥80 điểm** (do teacher phê duyệt) trước khi code
- ✅ **Code branch được tạo TỪ design branch** (chứa DESIGN.md context)
- ✅ Gate 2 là final review của code + test results (độc lập, không check design score)

---

## Gate 1: Design Review (Tuần 1-2)

### Khi nộp:
- **Thời điểm:** Tuần 1-2 (đầu project)
- **Branch name:** `embedded-linux/K{COURSE}/project/P{N}/{YOUR_USERNAME}/design`
  - Ví dụ: `embedded-linux/K27.1/project/P1/nguyen-van-a/design`
- **File nộp:** 1 file duy nhất
  ```
  embedded-linux/K27.1/
  └── nguyen-van-a/project/submission/design/
      └── DESIGN.md        ← File thiết kế
  ```

### Nội dung DESIGN.md cần có:

Xem `P{N}_DESIGN_TEMPLATE.md` để biết template chính xác. Tối thiểu phải cover:

**P1-P4 chung:**
1. **Tóm tắt:** Project là gì, giải quyết bài toán nào
2. **Kiến trúc:** Cấu trúc thư mục, module, luồng dữ liệu
3. **Phân tích yêu cầu:** Mỗi M1-M9 (hoặc M1-M11 cho P4) sẽ implement thế nào
4. **Kỹ thuật chính:** 
   - P1: GPIO interrupt, device tree overlay, systemd service
   - P2: Timing precision (timerfd/nanosleep, NOT sleep), non-blocking API, thread layout
   - P3: epoll architecture, non-blocking socket, file locking, password hashing
   - P4: /proc parsing, epoll server, agent-server model, memory leak handling
5. **Edge case:** Xử lý lỗi, timeout, disconnect, resource cleanup
6. **Quy trình test:** Cách test từng requirement

### Yêu cầu để đạt Gate 1:

**Claude (tự động review):**
- ✅ Design logic rõ ràng, có thể convert thành code
- ✅ Cover tất cả M1-M9 (hoặc M1-M11 cho P4)
- ✅ Kỹ thuật bắt buộc được nêu rõ (không vòng vèo)
- ✅ Edge case được xem xét
- ✅ Test plan realistic

**Teacher (review thủ công):**
- ✅ Kiến trúc hợp lý, có thể code được
- ✅ Không quá phức tạp, không quá đơn giản

### Score & Verdict:

| Score | Verdict | Được code? | Hành động |
|-------|---------|-----------|-----------|
| **≥80** | **PASS** | ✅ **CÓ** | Teacher phê duyệt → Bắt đầu code |
| **60-79** | **PASS_WITH_REVISIONS** | ⏳ **CHỜ** | Sửa design cho đến ≥80 → Teacher phê duyệt |
| **<60** | **FAIL** | ❌ **KHÔNG** | Viết lại design → nộp lại |

### Nếu bị Request Changes:

1. **Đọc Claude feedback** trên PR comment
2. **Sửa DESIGN.md** tại thư mục local
3. **Commit + push vào CÙNG branch** (không tạo PR mới)
   ```bash
   git add embedded-linux/K27.1/nguyen-van-a/project/submission/design/DESIGN.md
   git commit -m "Fix design: [brief reason]"
   git push origin embedded-linux/K27.1/project/P1/nguyen-van-a/design
   ```
4. **GitHub tự động:**
   - Update PR (add commit mới)
   - Re-run CI → Claude review lại
   - Post comment mới với score mới
5. **Lặp lại cho đến khi score ≥60** → teacher approve → merge

---

## Gate 2: Code Review (Tuần 3-8)

### Điều kiện tiên quyết:
- ✅ **Design đã đạt ≥80 điểm** (teacher phê duyệt)
- ✅ **Code branch tạo TỪ design branch**
- ✅ **Code xong + test xong** trước khi nộp

### Khi nộp:
- **Thời điểm:** Tuần 3-8 (sau khi design ≥80)
- **Branch name:** `embedded-linux/K{COURSE}/project/P{N}/{YOUR_USERNAME}/code`
  - Ví dụ: `embedded-linux/K27.1/project/P1/nguyen-van-a/code`
  - ⚠️ **Tạo từ design branch:** `git checkout -b .../code origin/embedded-linux/K27.1/project/P1/nguyen-van-a/design`

### File nộp:

```
embedded-linux/K27.1/
└── nguyen-van-a/project/submission/
    └── source-code/                    ← Tất cả code ở đây
        ├── README.md                   ← Hướng dẫn build/flash
        ├── src/                        ← Source code files
        ├── devicetree/                 ← Device tree overlays (P1, P2)
        ├── systemd/                    ← Systemd service files
        └── docs/
            └── test_report.md          ← 🔴 CRITICAL: Test results
```

### Nội dung README.md:

- **Cách build:** `make`, `cmake`, công cụ nào
- **Cách flash:** Command flash vào Pi Zero
- **Dependencies:** Library, tool cần cài
- **Cách chạy:** Lệnh chạy từng module/daemon
- **Test:** Cách test từng M1-M9 (hoặc M1-M11)
- **Debug:** Cách xem log, cách debug (nếu có)

**Ví dụ structure:**
```markdown
# P1 — IoT Gateway

## Build
```bash
cd src && make
```

## Flash
```bash
sudo dd if=image.bin of=/dev/sdX
```

## Run
```bash
sudo systemctl start p1-gateway
journalctl -u p1-gateway -f   # View logs
```

## Test
- M1 (Button driver): `tests/test_button.sh`
- M2 (LED driver): `tests/test_led.sh`
- ...
```

### Nội dung test_report.md — 🔴 VÔ CÙNG QUAN TRỌNG:

**test_report.md là bằng chứng duy nhất chứng minh code của bạn hoạt động.**

**Cấu trúc bắt buộc:**

```markdown
# Test Report — P1 Project

## M1: Button Driver

### TC-P1-M1-01: Button press → raw event
**Expected:** Raw GPIO event to stdin
**Log Output:**
```
[GPIO event at 2026-07-08 14:30:45.123456]
GPIO pin 17 state: 1 (pressed)
```
**Result:** ✅ PASS

## M2: LED Driver

### TC-P1-M2-01: LED on/off
**Expected:** LED blinks on command
**Log Output:**
```
[2026-07-08 14:31:00] LED ON
[2026-07-08 14:31:01] LED OFF
```
**Result:** ✅ PASS

...
```

### ⚠️ LƯU Ý QUAN TRỌNG VỀ test_report.md:

1. **Phải có concrete output/log**, không chỉ "PASS suông"
   - ❌ Sai: "Button test: PASS"
   - ✅ Đúng: "Button test: [GPIO event output ghi cụ thể]"

2. **Logs phải match code** — nếu code không in log gì, test report không nên có log từ hư cấu
   - Teacher sẽ kiểm tra xem log có khớp với code không
   - Nếu log "lạ" không tìm thấy ở code → không tín

3. **Test cả happy path + error cases** (không chỉ "mọi thứ hoạt động")
   - P1: Test button short/long/release, LED on/off, relay, OLED update
   - P2: Test timer precision (ghi cụ thể HH:MM:SS tại 2 mốc để tính độ lệch), weather API fail, button context
   - P3: Test auth, broadcast, disconnect, message history, /who command, error message (generic, not "user not found")
   - P4: Test CPU/RAM/disk parsing (cụ thể số liệu), agent connect/disconnect, heartbeat timeout, dashboard, /config, /history, memory leak (Valgrind output)

4. **Ghi cụ thể test sequence:**
   - Thời gian test
   - Lệnh chạy
   - Kết quả cụ thể (có số liệu, không chung chung)
   - Nếu test fail → ghi lý do, cách fix

5. **test_report.md không được gửi tới Claude** — chỉ teacher review
   - Claude review code + design + logic
   - Teacher review test_report để verify bạn thực sự test, không bịa đặt

### Yêu cầu để đạt Gate 2:

**Claude (tự động review code):**
- ✅ Code match DESIGN.md (không có design drift)
- ✅ Tất cả M1-M9 (hoặc M1-M11) được implement
- ✅ Xử lý lỗi đầy đủ (error handling, resource cleanup)
- ✅ Kỹ thuật bắt buộc đúng:
  - P1: GPIO interrupt, not polling
  - P2: timerfd/nanosleep, not sleep; non-blocking weather fetch
  - P3: epoll, NOT thread-per-client; password hash, NOT plaintext
  - P4: /proc parsing correct; epoll server; Valgrind zero leaks

**Teacher (review thủ công):**
- ✅ test_report.md có & cụ thể
- ✅ Logs match code (không bịa)
- ✅ Edge cases tested
- ✅ Code quality tốt (naming, structure, comments)

### Score & Verdict:

| Score | Verdict | Hành động |
|-------|---------|-----------|
| **≥80** | **PASS** | Merge → Project done ✅ |
| **60-79** | **PASS_WITH_REVISIONS** | Merge nhưng cần fix (follow-up sau) |
| **<60** | **FAIL** | Không merge → fix code → new PR |

### Nếu bị Request Changes:

1. **Fix code hoặc test_report** tại local
2. **Re-test** (rất quan trọng!) — nếu fix code, phải test lại và update test_report.md
3. **Commit + push vào CÙNG branch**
   ```bash
   git add embedded-linux/K27.1/nguyen-van-a/project/submission/
   git commit -m "Fix: [specific issue]"
   git push origin embedded-linux/K27.1/project/P1/nguyen-van-a/code
   ```
4. **GitHub tự động:**
   - Update PR
   - Re-run CI → Claude review lại
   - Post new review
5. **Lặp lại cho đến khi score ≥60 + teacher approve** → merge

---

## Lưu Ý Quan Trọng

### 1️⃣ GitHub Account Phải Khớp

- **class.json** có mapping: `"student-name" → "github-username"`
- **PR phải được tạo từ account đó**
  - ✅ Nếu student name là `nguyen-van-a` với GitHub `nguyenvana` → PR phải tạo từ account `nguyenvana`
  - ❌ Nếu từ account khác (vd `someone-else`) → PR bị reject

**Cách kiểm tra:**
```bash
git config user.name    # Xem account hiện tại
git config user.email
```

### 2️⃣ Branch Name Phải Đúng Format

**Design PR:**
```
embedded-linux/{COURSE}/project/P{N}/{STUDENT}/design
```

**Code PR:**
```
embedded-linux/{COURSE}/project/P{N}/{STUDENT}/code
```

- ❌ Sai: `P1-design`, `nguyen-van-a/P1`, `project/P1`, `embedded-linux/.../project` (cũ)
- ✅ Đúng: `embedded-linux/K27.1/project/P1/nguyen-van-a/design`
- ✅ Đúng: `embedded-linux/K27.1/project/P1/nguyen-van-a/code`

### 3️⃣ Một PR Duy Nhất (Không Tạo PR Mới)

- Khi bị **Request changes** → **không close PR cũ + tạo PR mới**
- **Push commit mới vào CÙNG branch** → GitHub tự động update PR

### 4️⃣ Tạo Code Branch Từ Design Branch

```bash
# Fetch latest changes từ remote
git fetch origin

# Tạo code branch TỪ design branch
git checkout -b embedded-linux/K27.1/project/P1/nguyen-van-a/code \
  origin/embedded-linux/K27.1/project/P1/nguyen-van-a/design

# Kiểm tra DESIGN.md có trong local không
cat embedded-linux/K27.1/nguyen-van-a/project/submission/design/DESIGN.md
```

**⚠️ Quan trọng:** Code branch PHẢI derived từ design branch (để có DESIGN.md context)

### 5️⃣ Test Kỹ Trước Khi Nộp

- Đừng nộp code chưa test
- Không có bằng chứng test → không thể chấm được
- test_report.md là **điểm duy nhất** chứng minh code hoạt động

### 6️⃣ Design Phải ≥80 Trước Khi Code

- Nếu design 60-79 → **chưa được code**, phải sửa cho đến ≥80
- Teacher phê duyệt design ≥80 → mới được phép tạo code branch
- **Lợi ích:** Code design không bị drift, DESIGN.md ổn định từ đầu

---

## Quy Trình Chi Tiết — Step by Step

### Design Gate (Tuần 1-2):

```
1. Viết DESIGN.md (template có sẵn)
2. git add/commit/push vào branch: embedded-linux/K27.1/project/P1/nguyen-van-a/design
3. Open PR trên GitHub
4. CI tự động run review_pr.py → Claude review
5. Xem Claude comment → score + feedback
6. Nếu score <80:
   a. Fix DESIGN.md
   b. git add/commit/push (cùng branch)
   c. PR tự động update → CI re-run → new score
   d. Lặp đến khi score ≥80
7. Nếu score ≥80:
   a. Teacher phê duyệt
   b. Có thể bắt đầu code (tạo code branch)
```

### Code Gate (Tuần 3-8):

```
1. Design đã ≥80 + teacher phê duyệt ✅
2. Tạo code branch TỪ design branch:
   git checkout -b .../code origin/.../design
3. Code theo DESIGN.md
4. Test toàn bộ M1-M9 (hoặc M1-M11)
5. Ghi test_report.md chi tiết (log + result cụ thể)
6. git add/commit/push vào branch: embedded-linux/K27.1/project/P1/nguyen-van-a/code
7. Open PR trên GitHub
8. Claude review code (có DESIGN.md context từ design branch)
9. Xem Claude comment → score + code quality feedback
10. Nếu score ≥60 + teacher ok:
    a. Teacher approve
    b. Merge PR → code final ✅
11. Nếu score <60 hoặc có issue:
    a. Fix code / update test_report.md
    b. Re-test (quan trọng!)
    c. git add/commit/push (cùng branch)
    d. PR tự động update + CI re-run → new score
    e. Lặp đến khi score ≥60 + teacher approve
```

---

## FAQ

**Q: Tạo PR từ account khác được không?**
A: Không. GitHub account phải khớp với class.json → PR bị reject.

**Q: Design score 70 (Pass with revisions) được code chưa?**
A: Chưa. Phải đạt ≥80 điểm + teacher phê duyệt mới được code.

**Q: Code PR được tạo trước design ≥80 được không?**
A: Không. Phải đợi design ≥80 + teacher approve → code branch derive từ design.

**Q: Tạo PR mới khi bị Request changes được không?**
A: Không nên. Dùng cùng PR, push commit mới → GitHub tự động update.

**Q: test_report.md phải ghi bằng tay toàn bộ hay có template?**
A: Có template: `TEST_REPORT_TEMPLATE.md` trong projects folder. Tham khảo template.

**Q: Nếu quên sync fork, code mà design chưa merge thì sao?**
A: Code PR sẽ bị reject. Phải sync fork, verify SCORE.json, rồi tạo PR mới.

**Q: Cần upload ảnh/video demo không?**
A: Không bắt buộc. Nếu có → tạo thư mục `docs/demo/` (teacher xem, Claude không).

**Q: Code bao nhiêu dòng là đủ?**
A: Không fix cứng. P1-P4 từ 200-500 dòng (tùy yêu cầu). Focus vào logic, không số dòng.

---

## Contact & Support

- **Thắc mắc về Gate 1 (Design):** Hỏi teacher → teacher có thể review design offline
- **Thắc mắc về Gate 2 (Code):** Hỏi teacher → teacher review code + test_report.md
- **Lỗi hệ thống (PR không open, CI hang):** Report tại Discord/Slack → admin check

---

## Checklist Trước Khi Nộp

### Design Gate:
- [ ] DESIGN.md cover tất cả M1-M9 (hoặc M1-M11 cho P4)
- [ ] Kiến trúc rõ ràng, có thể convert thành code
- [ ] Kỹ thuật bắt buộc nêu cụ thể (P1: GPIO interrupt; P2: timerfd; P3: epoll; P4: /proc)
- [ ] Edge case được xem xét
- [ ] Test plan realistic
- [ ] GitHub account khớp với class.json
- [ ] Branch name đúng format: `embedded-linux/K{COURSE}/project/P{N}/{STUDENT}/design`
- [ ] **Score ≥80 + teacher phê duyệt**

### Code Gate:
- [ ] Design đã ≥80 + teacher phê duyệt ✅
- [ ] Code branch tạo TỪ design branch (có DESIGN.md)
- [ ] Code hoàn thành + build pass
- [ ] Test toàn bộ M1-M9 (hoặc M1-M11)
- [ ] test_report.md chi tiết (log + result cụ thể)
- [ ] README.md có hướng dẫn build/flash/test
- [ ] Logs trong test_report.md match code (không bịa)
- [ ] Edge cases tested
- [ ] GitHub account khớp
- [ ] Branch name đúng: `embedded-linux/K{COURSE}/project/P{N}/{STUDENT}/code`
- [ ] Code match DESIGN.md (không design drift lớn)
- [ ] Kỹ thuật bắt buộc đúng (epoll, timerfd, hash, /proc parsing, v.v.)

---

*Last updated: 2026-07-14*
*Version: 2.0 — Design-Code Separation (No SCORE.json)*
*Design: review-only, anh quản lý approval. Code: độc lập, từ design branch.*
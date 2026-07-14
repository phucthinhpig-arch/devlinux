# Hướng Dẫn Nộp Project — DevLinux 2-Gate Review System

**Tài liệu này hướng dẫn học viên cách nộp 4 project cuối khoá (P1-P4) trong hệ thống DevLinux.**

---

## Overview — 2-Gate Review Là Gì?

Mỗi project cuối khoá phải qua **2 cổng review tuần tự**:

```
┌─────────────────────────┐
│  Gate 1: DESIGN REVIEW  │
│  (Tuần 1-2)             │
│  Claude review design   │
│  Teacher approve design │
│  → PASS ✅              │
└────────────┬────────────┘
             │
             ↓ Design merged to master + SCORE.json locked
             │
┌─────────────────────────┐
│  Gate 2: PROJECT REVIEW │
│  (Tuần 3-8)             │
│  Claude review code     │
│  Teacher review tests   │
│  Teacher approve code   │
│  → PASS ✅              │
└─────────────────────────┘
```

### Điều kiện bắt buộc:
- ✅ **Phải pass Gate 1 trước khi nộp Gate 2** (design được merge vào master)
- ✅ Gate 1 phải đạt **score ≥ 60** để được phép code
- ✅ Gate 2 là final review của code + test results

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
| **≥80** | **PASS** | ✅ **CÓ** | Merge ngay → sync fork → code |
| **60-79** | **PASS_WITH_REVISIONS** | ✅ **CÓ** | Merge → code song song với fix design |
| **<60** | **FAIL** | ❌ **KHÔNG** | Không merge → viết lại design từ đầu |

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

### Check design merged hay chưa:

```bash
# Trên local, sau khi sync fork
git pull origin master

# Check xem SCORE.json có không
cat embedded-linux/K27.1/nguyen-van-a/project/submission/design/SCORE.json
```

- **Tồn tại + score ≥60 + approved_by có reviewer:** ✅ Được code
- **Không tồn tại hoặc score <60:** ❌ Design chưa pass

---

## Gate 2: Project Review (Tuần 3-8)

### Điều kiện tiên quyết:
- ✅ **Design đã merge vào master** (SCORE.json tồn tại)
- ✅ **Đã sync fork** để lấy design + SCORE.json
- ✅ **Code xong + test xong** trước khi nộp

### Khi nộp:
- **Thời điểm:** Tuần 3-8 (sau khi design pass)
- **Branch name:** `embedded-linux/K{COURSE}/project/P{N}/{YOUR_USERNAME}/project`
  - Ví dụ: `embedded-linux/K27.1/project/P1/nguyen-van-a/project`

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
   git push origin embedded-linux/K27.1/project/P1/nguyen-van-a/project
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
embedded-linux/{COURSE}/project/P{N}/{STUDENT}/{design}
```

**Project PR:**
```
embedded-linux/{COURSE}/project/P{N}/{STUDENT}/{project}
```

- ❌ Sai: `P1-design`, `nguyen-van-a/P1`, `project/P1`
- ✅ Đúng: `embedded-linux/K27.1/project/P1/nguyen-van-a/design`

### 3️⃣ Một PR Duy Nhất (Không Tạo PR Mới)

- Khi bị **Request changes** → **không close PR cũ + tạo PR mới**
- **Push commit mới vào CÙNG branch** → GitHub tự động update PR

### 4️⃣ Sync Fork Trước Khi Code

```bash
# Sau khi design được merge
git fetch origin
git pull origin master

# Kiểm tra SCORE.json có không
cat embedded-linux/K27.1/nguyen-van-a/project/submission/design/SCORE.json

# Rồi tạo branch project
git checkout -b embedded-linux/K27.1/project/P1/nguyen-van-a/project
```

### 5️⃣ Test Kỹ Trước Khi Nộp

- Đừng nộp code chưa test
- Không có bằng chứng test → không thể chấm được
- test_report.md là **điểm duy nhất** chứng minh code hoạt động

### 6️⃣ Design Có Thể Fix Song Song Với Code

- Nếu design đạt **60-79** (Pass with revisions) → được merge + code
- Đồng thời có thể:
  - Code theo design hiện tại
  - Fix DESIGN.md trên branch riêng → open new design PR để revise
  - Teacher sẽ flag "design drift" nếu code không match design cũ
- **Nhưng:** Design revision phải hoàn thành trước final approval của code

---

## Quy Trình Chi Tiết — Step by Step

### Design Gate (Tuần 1-2):

```
1. Viết DESIGN.md (template có sẵn)
2. git add/commit/push vào branch: embedded-linux/K27.1/project/P1/nguyen-van-a/design
3. Open PR trên GitHub
4. CI tự động run review_pr.py → Claude review
5. Xem Claude comment → score + feedback
6. Nếu score ≥60:
   a. Teacher approve
   b. Merge PR → SCORE.json tự động được tạo
7. Nếu score <60:
   a. Fix DESIGN.md
   b. git add/commit/push (cùng branch)
   c. PR tự động update
   d. CI re-run → new score
   e. Lặp đến khi score ≥60
8. Sync fork: git pull origin master
   → SCORE.json sẽ có trong local
```

### Project Gate (Tuần 3-8):

```
1. Verify SCORE.json tồn tại ✅
2. Code theo DESIGN.md
3. Test toàn bộ M1-M9 (hoặc M1-M11)
4. Ghi test_report.md chi tiết (log + result cụ thể)
5. git add/commit/push vào branch: embedded-linux/K27.1/project/P1/nguyen-van-a/project
6. Open PR trên GitHub
7. CI tự động check:
   a. SCORE.json có không? (design merged?)
   b. test_report.md có không?
8. Claude review code + design alignment
9. Xem Claude comment → score + code quality feedback
10. Nếu score ≥60 + teacher ok:
    a. Teacher approve
    b. Merge PR → code final ✅
11. Nếu score <60 hoặc có issue:
    a. Fix code / update test_report.md
    b. Re-test (quan trọng!)
    c. git add/commit/push (cùng branch)
    d. PR tự động update + CI re-run
    e. Lặp đến khi score ≥60 + teacher approve
```

---

## FAQ

**Q: Tạo PR từ account khác được không?**
A: Không. GitHub account phải khớp với class.json → PR bị reject.

**Q: Có thể sửa SCORE.json sau merge không?**
A: Không. SCORE.json bị lock trong git history (immutable).

**Q: Design score 70 (Pass with revisions) mà không merge được chứ?**
A: Sai. Score 70 ≥ 60 → được merge. Có thể code song parallel với fix design.

**Q: Code PR được tạo trước design merge được không?**
A: Không. Hệ thống tự động reject nếu SCORE.json không tồn tại.

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

### Project Gate:
- [ ] Design đã merge (SCORE.json ≥60 + approved)
- [ ] Code hoàn thành + build pass
- [ ] Test toàn bộ M1-M9 (hoặc M1-M11)
- [ ] test_report.md chi tiết (log + result cụ thể)
- [ ] README.md có hướng dẫn build/flash/test
- [ ] Logs trong test_report.md match code (không bịa)
- [ ] Edge cases tested
- [ ] GitHub account khớp
- [ ] Branch name đúng: `embedded-linux/K{COURSE}/project/P{N}/{STUDENT}/project`
- [ ] Code match DESIGN.md (không design drift lớn)
- [ ] Kỹ thuật bắt buộc đúng (epoll, timerfd, hash, /proc parsing, v.v.)

---

*Last updated: 2026-07-08*
*Version: 1.0 — Hệ thống DevLinux 2-Gate Review*
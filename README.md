# DevLinux — Nộp Bài Tập

> Fork repo này để bắt đầu nộp bài.

---

## Cây thư mục

```
devlinux/
├── embedded-linux/
│   └── K26.1/
│       ├── class.json           ← danh sách học viên + GitHub usernames
│       ├── homeworks/           ← chứa deadline + báo cáo
│       │   ├── session-01.md    ← chứa deadline (dòng 2)
│       │   ├── session-02.md
│       │   └── homework_report.csv  ← báo cáo tự động (mỗi chủ nhật)
│       └── ten-hoc-vien/        ← giáo viên tạo sẵn
│           ├── session-01/      ← giáo viên tạo sẵn
│           │   ├── homework.md  ← giáo viên chuẩn bị sẵn — KHÔNG xoá
│           │   ├── Exercise_1/  ← học viên tự tạo
│           │   │   ├── main.c
│           │   │   └── Makefile
│           │   ├── Exercise_2/  ← học viên tự tạo
│           │   │   ├── main.c
│           │   │   └── Makefile
│           │   └── Exercise_3/  ← học viên tự tạo
│           │       ├── main.c
│           │       └── Makefile
│           └── session-02/      ← giáo viên tạo sẵn
├── rtos/
├── embedded-mcu/
└── c-advance/
```

---

## 📅 Xem Deadline Bài Tập

**Deadline của mỗi buổi học được giáo viên đặt trong file `homeworks/session-XX.md`**

### Cách xem deadline:

1. **Trên GitHub web:**
   - Vào thư mục `{môn}/{Khoá}/homeworks/`
   - Click file `session-XX.md`
   - Xem dòng thứ 2 chứa: `**Deadline: YYYY-MM-DD HH:MM:SS**`

2. **Trên máy (sau khi pull code):**
   ```bash
   # Ví dụ: xem deadline session-01
   cat embedded-linux/K26.1/homeworks/session-01.md | head -3
   
   # Output:
   # # Assignment — session-01
   # **Deadline: 2026-06-14 23:59:00**
   # ---
   ```

3. **Hoặc đơn giản nhất:**
   - Vào folder `{môn}/{Khoá}/homeworks/`
   - Mở file `session-XX.md` với text editor
   - Dòng thứ 2 là deadline

### Format deadline:

```
**Deadline: YYYY-MM-DD HH:MM:SS**
```

Ví dụ:
- `2026-06-14 23:59:00` = 14/6/2026 lúc 23:59
- `2026-06-21 18:00:00` = 21/6/2026 lúc 18:00

> ⏰ **Lưu ý:** Deadline tính theo **UTC timezone**. Nộp bài trước thời gian deadline = `✅ On time`, sau deadline = `⏰ Late`

---

## 📊 Xem Báo Cáo Nộp Bài (Attendance Report)

Hàng tuần, hệ thống tự động **kiểm tra xem mỗi học viên đã nộp bài hay chưa**, so sánh với deadline, và tạo file báo cáo CSV.

### Vị trí file báo cáo:

```
{môn}/{Khoá}/homeworks/homework_report.csv
```

**Ví dụ:**
- `embedded-linux/K26.1/homeworks/homework_report.csv`
- `rtos/K26.2/homeworks/homework_report.csv`

### Nội dung file CSV:

File được cập nhật định kỳ theo lịch của lớp học:

| Student | Session-01 | Session-02 | Session-03 | Session-04 | ... | Session-20 | Submission % |
|---------|-----------|-----------|-----------|-----------|-----|-----------|---|
| nguyen-quoc-viet | ✅ On time | ⏰ Late | ❌ Not yet | ⚪ N/A | ... | ⚪ N/A | 65.0% |
| tran-anh-tu | ✅ On time | ✅ On time | ⏰ Late | ✅ On time | ... | ⚪ N/A | 72.5% |
| le-van-c | ⏰ Late | ⏰ Late | ⏰ Late | ❌ Not yet | ... | ⚪ N/A | 37.5% |

**Giải thích Status cho từng buổi:**
- `✅ On time` — **nộp bài trước deadline + bài tập đạt yêu cầu**
- `⏰ Late` — **nộp bài sau deadline + bài tập đạt yêu cầu**
- `❌ Not yet` — chưa nộp bài HOẶC nộp bài nhưng chưa đạt yêu cầu
- `⚪ N/A` — không có bài tập giao cho buổi này
- Nếu GitHub username = "none" trong class.json → luôn `❌ Not yet`

### ⚠️ Điều kiện để bài tập được tính vào báo cáo

**Bài tập chỉ được tính khi:**
1. ✅ **Được nộp trước deadline** HOẶC **sau deadline** (miễn nộp trước khi báo cáo chạy)
2. **VÀ** bài tập được hệ thống chấp nhận (có label `✅ passed`)

**Nếu bài tập chưa đạt yêu cầu:**
- Dù nộp trước deadline, vẫn hiện `❌ Not yet` trên báo cáo
- **Phải sửa lại theo nhận xét của hệ thống để đạt label `✅ passed`**
- **Deadline của bài tập là deadline để NỘP + CÓ SẴN LABEL `✅ passed`**
- Nộp muộn nhưng chưa có `✅ passed` → vẫn tính là `❌ Not yet`

### Cách tính Submission %

```
Submission % = (Tổng điểm từ bài tập đạt / Tổng bài tập đã qua deadline) × 100%
```

**Scoring cho từng bài tập:**
- `✅ On time` + `✅ passed` → **1 điểm**
- `⏰ Late` + `✅ passed` → **0.5 điểm**
- `❌ Not yet` (chưa nộp hoặc chưa đạt) → **0 điểm**

**Ví dụ:**
- 10 buổi giao bài tập (chỉ tính những buổi đã qua deadline)
- 6 buổi nộp đúng hạn + đạt = 6 điểm
- 2 buổi nộp muộn + đạt = 1 điểm
- 2 buổi chưa nộp hoặc chưa đạt = 0 điểm
- **Kết quả: (6 + 1) / 10 × 100% = 70%**

### Cách xem báo cáo:

1. **Trên GitHub web:**
   - Vào thư mục `{môn}/{Khoá}/homeworks/`
   - Tìm file `homework_report.csv`
   - GitHub tự động render thành bảng (dễ nhìn)

2. **Tải về máy:**
   ```bash
   git pull
   cat embedded-linux/K26.1/homeworks/homework_report.csv
   ```

> 💡 **Lưu ý:** 
> - Báo cáo được tạo dựa trên PR của học viên, không phải merge PR. Nộp bài = tạo PR, không cần merge để được tính là đã nộp.
> - **Chỉ bài tập có label `✅ passed` từ hệ thống mới được tính vào báo cáo và submission %**
> - Nộp bài sớm nhưng chưa đạt `✅ passed` → phải sửa trước deadline để được tính vào submission %

---

## Quy trình nộp bài

### Bước 1 — Fork repo (1 lần duy nhất)

1. Vào repo của giáo viên (giáo viên sẽ gửi link)
2. Click **Fork** góc trên phải → chọn **Create a new fork**
3. Giữ nguyên tên repo `devlinux`
4. Click **Create fork**

Sau khi fork xong, GitHub tự chuyển sang repo của bạn. Kiểm tra góc trên trái thấy:

```
TÊN_BẠN / devlinux
forked from DevLinux-vn/devlinux
```

---

### Bước 2 — Tạo SSH key để push code (1 lần duy nhất)

GitHub không cho phép dùng password để push — cần SSH key.

**Trên Linux / WSL / macOS:**

```bash
ssh-keygen -t ed25519 -C "devlinux"
# Nhấn Enter liên tục (dùng đường dẫn mặc định, không đặt passphrase)

cat ~/.ssh/id_ed25519.pub
# Copy toàn bộ dòng in ra
```

**Trên Windows (PowerShell):**

```powershell
ssh-keygen -t ed25519 -C "devlinux"
# Nhấn Enter liên tục

cat ~/.ssh/id_ed25519.pub
# Copy toàn bộ dòng in ra
```

Thêm public key lên GitHub:

1. Vào **GitHub → Settings → SSH and GPG keys → New SSH key**
2. Title: đặt tên máy tính (vd: `laptop`, `wsl-desktop`)
3. Paste nội dung public key vào
4. Click **Add SSH key**

Xác nhận kết nối thành công:

```bash
ssh -T git@github.com
# Hi TÊN_BẠN! You've successfully authenticated...
```

> 💡 Nếu dùng nhiều máy, làm lại bước này trên từng máy và thêm từng key vào GitHub.

---

### Bước 3 — Clone về máy (1 lần duy nhất)

Clone bằng SSH — dùng key vừa tạo ở Bước 2, không cần nhập password:

```bash
git clone git@github.com:TÊN_BẠN/devlinux.git
cd devlinux
```

---

### Bước 4 — Sync fork và tạo branch

**Làm bước này mỗi buổi học trước khi bắt đầu làm bài để cập nhật được danh sách bài tập về nhà mới nhất.**

**Bước 4.1 — Sync fork từ repo của giáo viên (trên GitHub web)**

1. Vào fork của bạn trên GitHub
2. **⚠️ Chú ý:** Kiểm tra branch dropdown góc trên trái — **PHẢI là `master`** mới sync
3. Nhấn nút **Sync fork** → chọn **Update branch**

> 🔴 **Lỗi thường gặp:** Nếu đang trên branch session cũ (ví dụ `embedded-linux/K26.1/...`) mà nhấn Sync fork, sẽ đồng bộ hóa sai branch. Luôn chuyển sang **`master`** trước khi sync!

**Bước 4.2 — Cập nhật code cục bộ và tạo branch**

```bash
# 1. Quay về master và pull code mới nhất từ fork
git checkout master
git pull

# 2. Tạo branch mới cho buổi học và checkout sang branch đó
git checkout -b tên-môn/KXX.X/tên-của-bạn/session-XX
```

> ⚠️ **RẤT QUAN TRỌNG:** **LUÔN `git checkout master` TRƯỚC khi tạo branch session mới!**
>
> Nếu bạn quên `git checkout master` và tạo branch mới từ branch buổi cũ, PR của bạn sẽ chứa code từ **cả 2 buổi** → bị reject tự động.
>
> **Cách kiểm tra bạn đang ở master:**
> ```bash
> git branch   # phải thấy * master (có dấu *)
> git log --oneline | head -1  # phải thấy commit mới nhất từ master
> ```

**Format branch bắt buộc:** `tên-môn/KXX.X/tên-của-bạn/session-XX`

| Thành phần | Ý nghĩa | Ví dụ |
|---|---|---|
| `tên-môn` | Tên môn học (viết thường, dấu gạch ngang) | `embedded-linux` |
| `KXX.X` | Mã khoá học | `K26.1` |
| `tên-của-bạn` | Tên thư mục của bạn trong repo (giáo viên đã tạo sẵn) | `nguyen-van-a` |
| `session-XX` | Số buổi học, 2 chữ số | `session-01` |

| Môn | Tên dùng trong branch |
|---|---|
| Embedded Linux | `embedded-linux` |
| RTOS | `rtos` |
| Embedded MCU | `embedded-mcu` |
| C Advance | `c-advance` |

**Ví dụ cụ thể:**
```bash
# Học viên "nguyen-van-a", môn Embedded Linux, khoá K26.1, buổi 1
git checkout -b embedded-linux/K26.1/nguyen-van-a/session-01

# Học viên "tran-thi-b", môn RTOS, khoá K26.2, buổi 3
git checkout -b rtos/K26.2/tran-thi-b/session-03
```

> ⚠️ Sai format branch → PR sẽ bị từ chối tự động.
> ⚠️ `tên-của-bạn` phải khớp chính xác với tên thư mục giáo viên đã tạo — hỏi giáo viên nếu không chắc.

---

### Bước 5 — Làm bài

> ⚠️ **Trước khi bắt đầu, kiểm tra 2 thứ này:**
>
> ```bash
> git branch          # phải thấy branch buổi hôm nay được đánh dấu *
> pwd                 # phải đang đứng trong thư mục đúng session
> ```
>
> Ví dụ đúng — buổi 2:
> ```
> * embedded-linux/K26.1/nguyen-van-a/session-02   ← branch hôm nay
>   master
> ```
> Nếu thấy branch sai hoặc đang đứng trong `session-01/` → quay lại Bước 4.

Thư mục `session-XX/` và file `homework.md` đã được giáo viên chuẩn bị sẵn. Vào đúng thư mục session của buổi học:

```bash
cd embedded-linux/K26.1/nguyen-van-a/session-01/
```

Đọc đề bài trong `homework.md`, tạo thư mục cho từng bài:

```bash
# ⚠️ Tên thư mục bắt buộc: Exercise_1, Exercise_2, Exercise_3...
#    (chữ E in hoa, dấu gạch dưới _, không phải gạch ngang -)

mkdir -p Exercise_1
mkdir -p Exercise_2
mkdir -p Exercise_3
```

Viết code vào từng thư mục Exercise:

```
session-01/
├── homework.md        ← đề bài, giáo viên chuẩn bị sẵn — KHÔNG xoá
├── Exercise_1/
│   ├── main.c
│   └── Makefile
├── Exercise_2/
│   ├── main.c
│   └── Makefile
└── Exercise_3/
    ├── main.c
    └── Makefile
```

> ⚠️ **RẤT QUAN TRỌNG:** Chỉ được tạo/sửa file **TRONG Exercise folders**!
>
> ❌ **KHÔNG được tạo:**
> - `session-01/debug.c` (ngoài Exercise)
> - `session-01/config.h` (ngoài Exercise)
> - `session-01/test.c` (ngoài Exercise)
> - `session-01/.gitignore` (ngoài Exercise)
>
> ✅ **CHỈ được tạo trong:**
> - `session-01/Exercise_1/main.c`
> - `session-01/Exercise_1/Makefile`
> - `session-01/Exercise_2/...`
>
> Nếu cần file helper (debug, config, test), **đặt nó TRONG Exercise folder**, không ở root session.

> ⚠️ Mỗi Exercise trong đề bài có ghi rõ loại:
> - `[build]` — **bắt buộc có Makefile**, hệ thống sẽ build và chạy thử. Không có Makefile → điểm 0 toàn bài.
> - `[review-only]` — không cần Makefile, hệ thống chỉ review code tĩnh (Yocto recipe, Device Tree, v.v.).

---

### Bước 6 — Commit và push

```bash
git add .
git commit -m "embedded-linux K26.1 session-01: hoàn thành Exercise_1, Exercise_2, Exercise_3"
git push origin embedded-linux/K26.1/nguyen-van-a/session-01
```

Phần sau `origin` là tên branch — phải khớp với branch đang làm việc:

| Thành phần | Ví dụ |
|---|---|
| Tên môn | `embedded-linux` |
| Mã khoá | `K26.1` |
| Tên của bạn | `nguyen-van-a` |
| Buổi học | `session-01` |

> ⚠️ Mỗi buổi học là một branch riêng — bài buổi nào push lên branch buổi đó. Không push bài của `session-02` lên branch `session-01`.

---

### Bước 7 — Mở Pull Request

1. Vào GitHub → repo fork của bạn
2. Click **Compare & pull request**
3. Kiểm tra 4 trường như ví dụ sau:

```
base repository: DevLinux-vn/devlinux  ←  base: master
head repository: nguyen-van-a/devlinux  ←  compare: embedded-linux/K26.1/nguyen-van-a/session-01
```

   - **base repository**: repo của giáo viên
   - **base**: `master`
   - **head repository**: repo fork của bạn
   - **compare**: branch vừa push

4. Title: `[Embedded Linux K26.1 Session 01] Nguyễn Văn A`
5. Click **Create pull request**

---

## Các trường hợp PR bị từ chối tự động

Hệ thống sẽ **tự động close PR và báo lý do** nếu vi phạm một trong các điều sau:

### **1️⃣ Kiểm tra Format Branch**

| Lý do bị reject | Cách khắc phục |
|---|---|
| Branch thiếu 4 phần (môn/khoá/tên/session) | Format: `tên-môn/KXX.X/tên-của-bạn/session-XX` |
| Môn không hợp lệ | Dùng một trong: `embedded-linux`, `rtos`, `embedded-mcu`, `c-advance` |
| Thư mục môn không tồn tại | Kiểm tra tên môn đúng, liên hệ giáo viên |
| Khoá sai format (không phải KXX.X) | Ví dụ: `K26.1`, `K27.2` |
| Thư mục khoá không tồn tại | Kiểm tra tên khoá, liên hệ giáo viên |
| Session sai format (không phải session-XX) | Ví dụ: `session-01`, `session-03` (không `session-1`) |
| Thư mục session không tồn tại | Dùng đúng tên session giáo viên đã tạo |
| Tên học viên trong branch không khớp thư mục | Dùng đúng tên thư mục (hỏi giáo viên nếu không biết) |
| Học viên không trong danh sách lớp (class.json) | Liên hệ giáo viên để được thêm vào danh sách |
| Tài khoản GitHub không khớp class.json | Dùng tài khoản GitHub được đăng ký với giáo viên |

### **2️⃣ Kiểm tra File Nộp**

| Lý do bị reject | Cách khắc phục |
|---|---|
| Nộp file từ nhiều session cùng lúc (vd: session-01 + session-03) | **Nguyên nhân:** Quên `git checkout master` trước khi tạo branch session-02. Branch mới được tạo từ session-01 → kéo theo code cũ. **Cách khắc phục:** LUÔN quay về master trước tạo branch buổi mới: `git checkout master && git pull` rồi mới `git checkout -b embedded-linux/K26.1/ten-ban/session-02`. Nếu tạo sai, đóng PR cũ và tạo PR mới từ branch đúng. |
| Sửa hoặc tạo file ngoài Exercise folders | **Chỉ được tạo/sửa file TRONG `Exercise_N/` folders.** Không được tạo/sửa: `homework.md`, `debug.c`, `config.h`, `.gitignore`, v.v. ở ngoài Exercise folders (dù là bên trong session folder). Mỗi file phụ trợ phải nằm bên trong Exercise folder tương ứng. |
| Sửa file ở root repo (vd: `README.md`, `class.json`) | Chỉ được tạo/sửa file bên trong thư mục session của bạn (`{môn}/{khoá}/{tên-bạn}/{session-XX}/`). Không được sửa files ở root repo |

### **3️⃣ Kiểm tra Bài Tập**

| Lý do bị reject | Cách khắc phục |
|---|---|
| Nộp thiếu bài tập so với đề bài | Hoàn thành tất cả Exercise được định nghĩa trong `homework.md` |

---

> ⚠️ **Sau khi bị reject:**
> - **Trường hợp sai tên branch/môn/khoá:** Tạo branch mới đúng format → mở PR mới
> - **Trường hợp khác (thiếu bài, sửa file sai, v.v.):** Push thêm code lên branch cũ → **reopen PR cũ** (không tạo PR mới)

---

## Khi cần sửa bài sau khi review

```bash
# Sửa code trên cùng branch, không cần tạo PR mới
git add .
git commit -m "fix: sửa lỗi fd leak theo review"
git push origin embedded-linux/K26.1/nguyen-van-a/session-01
```

PR tự cập nhật và được review lại tự động.

---

## Buổi học tiếp theo (session-02, session-03...)

Thư mục `session-02/`, `session-03/`... đã được tạo sẵn — không cần `mkdir`.

Làm lại **Bước 4 → Bước 7** mỗi buổi:

**Bước 4.1 — Sync fork từ repo của giáo viên (trên GitHub web)**

1. Vào fork của bạn trên GitHub
2. **⚠️ Chú ý:** Kiểm tra branch dropdown góc trên trái — **PHẢI là `master`** mới sync
3. Nhấn nút **Sync fork** → chọn **Update branch**

> 🔴 **Lỗi thường gặp:** Nếu đang trên branch session cũ (ví dụ `embedded-linux/K26.1/...`) mà nhấn Sync fork, sẽ đồng bộ hóa sai branch. Luôn chuyển sang **`master`** trước khi sync!

**Bước 4.2 → Bước 7 — Tạo branch, làm bài, commit và push:**

```bash
# Cập nhật code cục bộ
git checkout master
git pull

# Tạo branch buổi mới
git checkout -b embedded-linux/K26.1/nguyen-van-a/session-02

# Kiểm tra đang đúng branch chưa
git branch    # phải thấy * embedded-linux/K26.1/nguyen-van-a/session-02

# Vào đúng thư mục session
cd embedded-linux/K26.1/nguyen-van-a/session-02
pwd           # phải kết thúc bằng /session-02
mkdir -p Exercise_1 Exercise_2 Exercise_3

# Làm bài, commit, push, mở PR như bình thường
```

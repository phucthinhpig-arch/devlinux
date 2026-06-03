# DevLinux — Nộp Bài Tập

> Fork repo này để bắt đầu nộp bài.

---

## Cây thư mục

```
devlinux/
├── embedded-linux/
│   └── K26.1/
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

## Quy trình nộp bài

### Bước 1 — Fork repo (1 lần duy nhất)

1. Vào repo của giáo viên (giáo viên sẽ gửi link)
2. Click **Fork** góc trên phải → chọn **Create a new fork**
3. Giữ nguyên tên repo `devlinux`
4. Click **Create fork**

Sau khi fork xong, GitHub tự chuyển sang repo của bạn. Kiểm tra góc trên trái thấy:

```
TÊN_BẠN / devlinux
forked from quocviet1026/devlinux
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

### Bước 4 — Pull code mới nhất và tạo branch

**Làm bước này mỗi buổi học trước khi bắt đầu làm bài.**

```bash
# 1. Quay về master
git checkout master

# 2. Pull code mới nhất từ repo giáo viên (có homework.md của buổi mới)
git pull upstream master

# 3. Tạo branch mới cho buổi học và checkout sang branch đó
git checkout -b embedded-linux/K26.1/nguyen-van-a/session-01
```

Format branch bắt buộc: `tên-môn/KXX.X/tên-của-bạn/session-XX`

| Môn | Tên dùng trong branch |
|---|---|
| Embedded Linux | `embedded-linux` |
| RTOS | `rtos` |
| Embedded MCU | `embedded-mcu` |
| C Advance | `c-advance` |

> ⚠️ Sai format branch → PR sẽ bị từ chối tự động.

---

### Bước 5 — Làm bài

Thư mục `session-XX/` và file `homework.md` đã được giáo viên chuẩn bị sẵn. Vào đúng thư mục session của buổi học:

```bash
cd embedded-linux/K26.1/nguyen-van-a/session-01/
```

Đọc đề bài trong `homework.md`, tạo thư mục cho từng bài:

```bash
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

---

### Bước 7 — Mở Pull Request

1. Vào GitHub → repo fork của bạn
2. Click **Compare & pull request**
3. Kiểm tra 4 trường như ví dụ sau:

```
base repository: quocviet1026/devlinux  ←  base: master
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

| Lý do bị reject | Cách khắc phục |
|---|---|
| Branch sai format (thiếu phần, sai tên môn, sai khoá, sai session) | Tạo lại branch đúng format `tên-môn/KXX.X/tên-của-bạn/session-XX` |
| Tên học viên trong branch không khớp thư mục giáo viên đã tạo | Dùng đúng tên thư mục — hỏi giáo viên nếu không biết |
| Tài khoản GitHub không khớp danh sách lớp | Đảm bảo đang dùng đúng tài khoản GitHub đã đăng ký với giáo viên |
| Nộp thiếu bài tập so với đề bài | Hoàn thành đủ tất cả Exercise trong `homework.md` rồi push lại, sau đó reopen PR |
| Nộp từ branch `master` | Tạo branch mới đúng format, không làm bài trực tiếp trên master |

> ⚠️ Sau khi bị reject: **không mở PR mới** — push thêm code lên branch cũ rồi **reopen PR cũ** là đủ (trừ trường hợp sai tên branch thì phải tạo branch mới).

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

```bash
# Bước 4: Pull code mới và tạo branch buổi mới
git checkout master
git pull upstream master
git checkout -b embedded-linux/K26.1/nguyen-van-a/session-02

# Bước 5: Vào thư mục session, đọc đề bài, tạo thư mục Exercise và viết code
cd embedded-linux/K26.1/nguyen-van-a/session-02
mkdir -p Exercise_1 Exercise_2 Exercise_3

# Bước 6 & 7: Commit, push, mở PR như bình thường
```

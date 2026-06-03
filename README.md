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

### Bước 4 — Tạo branch cho mỗi buổi học

Format bắt buộc: `tên-môn/KXX.X/tên-của-bạn/session-XX`

```bash
# Ví dụ: học viên "nguyen-van-a", môn Embedded Linux, khoá K26.1, buổi 1
git checkout -b embedded-linux/K26.1/nguyen-van-a/session-01
```

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

> ⚠️ Mỗi Exercise bắt buộc có Makefile — không có Makefile sẽ bị điểm 0 toàn bài.

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

```bash
# Quay về master, pull code mới nhất (bao gồm homework.md mới từ giáo viên)
git checkout master
git pull upstream master

# Tạo branch mới cho buổi mới
git checkout -b embedded-linux/K26.1/nguyen-van-a/session-02

# Vào thư mục session, tạo các thư mục Exercise
cd embedded-linux/K26.1/nguyen-van-a/session-02
mkdir -p Exercise_1 Exercise_2 Exercise_3

# Làm bài, commit, push, mở PR như bình thường
```

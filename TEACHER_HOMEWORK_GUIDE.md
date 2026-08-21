# Hướng Dẫn: Thêm/Sửa Bài Tập Về Nhà

Tài liệu này hướng dẫn giảng viên cách **thêm bài tập mới** hoặc **sửa bài tập** để phân phối cho học viên.

---

## 📋 **Overview: Quy Trình**

```
1. Mở/tạo file: {subject}/{course}/homeworks/session-XX.md
2. Dùng template: homework.template.md
3. Chỉnh nội dung + deadline
4. Commit + push
5. Tạo PR từ branch: {subject}/{course}/homework
6. GitHub Actions auto-merge + phân phối cho tất cả học viên
```

---

## 🎯 **Phần 1: Chuẩn Bị — Template & Deadline**

### Tìm template

Template file nằm ở **repo root**:

```bash
cd devlinux/
cat homework.template.md
```

**Hoặc copy nhanh:**

```bash
cp homework.template.md embedded-linux/K26.1/homeworks/session-10.md
```

### Deadline — QUAN TRỌNG ⚠️

**Bắt buộc:** Dòng thứ 2 trong file phải có **Deadline chính xác**, định dạng:

```markdown
**Deadline: YYYY-MM-DD HH:MM:SS**
```

**Ví dụ:**

```markdown
# Assignment — session-10
**Deadline: 2026-06-14 23:59:00**
```

**Lưu ý:**
- ✅ Format: `YYYY-MM-DD HH:MM:SS` (năm-tháng-ngày giờ:phút:giây)
- ✅ Timezone: UTC
- ✅ Dòng phải bắt đầu với `**Deadline:` (có dấu `**`)
- ❌ Không được: `Deadline: 14/06/2026` (sai format)
- ❌ Không được: `Due date: 2026-06-14` (thiếu giờ)

**Hệ thống sẽ dùng deadline này để:**
- Theo dõi học viên nộp bài muộn hay đúng hạn
- Tạo báo cáo attendance report (xem OPERATING_GUIDE.md Section 6)

---

## 🆕 **Phần 2: Thêm Bài Tập Mới**

### Bước 1: Tạo file session mới

```bash
cd devlinux/

# Mở folder bài tập của lớp
vim embedded-linux/K26.1/homeworks/session-10.md
```

Hoặc copy từ template:

```bash
cp homework.template.md embedded-linux/K26.1/homeworks/session-10.md
```

### Bước 2: Chỉnh nội dung theo template

**Mở file session-10.md và chỉnh:**

```markdown
# Assignment — session-10
**Deadline: 2026-06-14 23:59:00**

> ⚠️ Xóa tất cả dòng hướng dẫn (bắt đầu bằng `>`) trước khi push

---

## Exercise_1 [build]

### Problem Statement

Viết chương trình C tính...

### Design Hints (optional)

Gợi ý struct:
```c
typedef struct {
    int value;
} my_t;
```

### Suggested Approach (optional)

```
1. Đọc input
2. Xử lý dữ liệu
3. In kết quả
```

### Expected Output (optional)

Khi chạy với input `5`, output:
```
Result: 5
```

### Submission

```
Exercise_1/
├── main.c
└── Makefile
```

---

## Exercise_2 [build]

### Problem Statement

...

### Submission

```
Exercise_2/
├── main.c
└── Makefile
```
```

**Giải thích các phần:**

| Phần | Bắt buộc? | Mục đích |
|------|----------|---------|
| **Exercise_X [build]** | ✅ | Bài tập có build (Makefile), hệ thống sẽ compile |
| **Exercise_X [review-only]** | ✅ | Bài tập chỉ review code (Device Tree, MCU, etc.) |
| **Problem Statement** | ✅ | Yêu cầu bài tập chi tiết |
| **Design Hints** | ❌ | Gợi ý struct/enum (optional, xóa nếu muốn) |
| **Suggested Approach** | ❌ | Gợi ý thuật toán (optional, xóa nếu muốn) |
| **Expected Output** | ❌ | Output mong đợi (optional, xóa nếu chỉ review) |
| **Submission** | ✅ | Cấu trúc folder cần nộp |

### Bước 3: Xóa tất cả dòng `>` (hướng dẫn template)

**Trước (template):**
```markdown
> Tag `[build]`: the system will compile and run the program
> Describe the exercise requirements. Be as specific as possible.

Write a C program that ...
```

**Sau (khi sửa xong):**
```markdown
Write a C program that ...
```

### Bước 4: Kiểm tra deadline

Đảm bảo dòng 2 chính xác:

```markdown
**Deadline: 2026-06-14 23:59:00**
```

✅ Nếu đúng: Hệ thống sẽ track học viên nộp bài đúng hạn/muộn

❌ Nếu sai: Báo cáo attendance sẽ không chính xác

### Bước 5: Xem trước (tùy chọn)

```bash
cat embedded-linux/K26.1/homeworks/session-10.md
```

Kiểm tra:
- ✅ Deadline ở dòng 2, format đúng
- ✅ Không có dòng `>` (instructions)
- ✅ Các Exercise đều có `[build]` hoặc `[review-only]`
- ✅ Submission section rõ ràng

---

## ✏️ **Phần 3: Sửa Bài Tập Cũ**

### Cách 1: Sửa file hiện tại

```bash
# Mở file cũ
vim embedded-linux/K26.1/homeworks/session-10.md

# Chỉnh nội dung (thêm/xóa/sửa Exercise)
# Nhớ không sửa deadline (hoặc sửa nếu có deadline mới)

# Save & exit
```

**Ví dụ sửa:**
- ✅ Thêm Exercise mới
- ✅ Sửa Problem Statement
- ✅ Thêm Design Hints
- ✅ Cập nhật Deadline (nếu cần dời hạn nộp)
- ❌ Không xóa Deadline (bắt buộc phải có)

### Cách 2: Copy từ file cũ + chỉnh

```bash
# Copy session cũ làm template
cp embedded-linux/K26.1/homeworks/session-09.md embedded-linux/K26.1/homeworks/session-10.md

# Chỉnh: tên session, deadline, nội dung
vim embedded-linux/K26.1/homeworks/session-10.md
```

### Cập nhật Deadline (Quan Trọng)

Nếu cần **dời hạn nộp bài:**

```markdown
# Assignment — session-10
**Deadline: 2026-06-21 23:59:00**   ← cập nhật deadline mới
```

Hệ thống sẽ dùng deadline mới này để:
- ✅ Báo cáo attendance được update
- ✅ Học viên biết hạn nộp mới

---

## 📤 **Phần 4: Push & Phân Phối Tự Động**

### Bước 1: Commit thay đổi

```bash
# Stage files
git add embedded-linux/K26.1/homeworks/

# Commit (nội dung mô tả rõ ràng)
git commit -m "homework: update session 10 for embedded-linux/K26.1"
# Hoặc
git commit -m "homework: add session 12-15 for c-advance/K26.1"
```

### Bước 2: Push lên branch

```bash
# Push vào branch homework
git push origin embedded-linux/K26.1/homework
```

**Lưu ý:** Branch phải đúng format `{subject}/{course}/homework`

```bash
✅ embedded-linux/K26.1/homework
✅ c-advance/K26.1/homework
❌ update-homework (sai format → PR bị reject)
```

### Bước 3: Tạo PR

GitHub Web UI:
1. Vào repo devlinux
2. Click "Compare & pull request"
3. Kiểm tra:
   - Base: `master`
   - Compare: `embedded-linux/K26.1/homework`
4. Click "Create Pull Request"

### Bước 4: GitHub Actions Tự Động Xử Lý

```
✅ Validate branch name
✅ Validate permission (bạn có quyền edit course này không)
✅ Validate files (chỉ homeworks/ được sửa)
✅ Auto-approve PR
✅ Run: bash distribute_homework.sh embedded-linux/K26.1
   → Copy session-XX.md cho tất cả học viên
✅ Auto-merge PR
(~30 giây)
```

**Xem progress:** GitHub PR page → Checks tab

### Bước 5: Hoàn Tất ✅

Bài tập tự động phân phát:

```
✅ embedded-linux/K26.1/student-1/session-10/homework.md → updated
✅ embedded-linux/K26.1/student-2/session-10/homework.md → updated
✅ embedded-linux/K26.1/student-3/session-10/homework.md → updated
... (tất cả học viên)
```

Học viên sẽ tự động thấy bài tập mới khi pull từ master.

---

## 🔧 **Troubleshooting**

| Vấn đề | Nguyên nhân | Fix |
|---|---|---|
| **Deadline không được update** | Deadline format sai | Kiểm tra: `**Deadline: YYYY-MM-DD HH:MM:SS**` (đúng format) |
| **PR bị reject: invalid branch name** | Tên branch sai format | Tạo branch mới: `{subject}/{course}/homework` |
| **PR bị reject: no permission** | Không được quyền edit course | Contact admin → thêm vào teacher-permissions.json |
| **PR bị reject: files not in homeworks/** | Edit file ngoài homeworks/ | Chỉ edit file trong `{subject}/{course}/homeworks/` |
| **Template hướng dẫn vẫn có trong file** | Quên xóa dòng `>` | Xóa tất cả dòng bắt đầu với `>` |
| **Học viên không thấy bài tập** | PR chưa merge | Kiểm tra PR status, chờ auto-merge hoàn tất |
| **Exercise không compile** | Makefile hoặc code lỗi | Kiểm tra Exercise có tag `[build]` không, Makefile đúng không |

---

## 📌 **Checklist Trước Khi Push**

- [ ] Dòng 2 có deadline: `**Deadline: YYYY-MM-DD HH:MM:SS**`
- [ ] Format deadline đúng (kiểm tra: năm-tháng-ngày giờ:phút:giây)
- [ ] Không có dòng `>` (xóa tất cả instructions)
- [ ] Các Exercise có tag `[build]` hoặc `[review-only]`
- [ ] Problem Statement rõ ràng
- [ ] Submission section có cấu trúc folder
- [ ] Branch name đúng: `{subject}/{course}/homework`
- [ ] Commit message rõ ràng: "homework: update session X for {subject}/{course}"

---

## 💡 **Ví Dụ Đầu Đủ**

**File:** `c-advance/K26.1/homeworks/session-10.md`

```markdown
# Assignment — session-10
**Deadline: 2026-06-21 23:59:00**

---

## Exercise_1 [build]

### Problem Statement

Viết chương trình C thực hiện các thao tác:
1. Đọc dữ liệu từ file input.txt
2. Xử lý: đảo ngược từng dòng
3. Ghi kết quả vào output.txt

### Submission

```
Exercise_1/
├── main.c
├── Makefile
└── input.txt (test case)
```

---

## Exercise_2 [review-only]

### Problem Statement

Viết shell script giải nén tất cả `.zip` files trong thư mục hiện tại.

### Suggested Approach

```bash
#!/bin/bash
for file in *.zip; do
    unzip "$file"
done
```

### Submission

```
Exercise_2/
└── extract.sh
```
```

---

**Hết! Bây giờ bạn có thể thêm/sửa bài tập và phân phối tự động. 🎉**

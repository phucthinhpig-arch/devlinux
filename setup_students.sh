#!/bin/bash
# =============================================================
#  setup_students.sh — Tạo thư mục học viên và session tự động
# =============================================================
#
# CÁCH DÙNG
#   1. Setup tất cả lớp trong repo:
#        bash setup_students.sh
#
#   2. Setup 1 lớp cụ thể:
#        bash setup_students.sh <subject> <course>
#        bash setup_students.sh c-basic K26.2
#        bash setup_students.sh embedded-linux K26.1
#
# MÔ TẢ
#   Script tự động tìm tất cả file class.json trong repo
#   (hoặc 1 lớp cụ thể nếu có tham số)
#   và tạo thư mục học viên + thư mục session tương ứng.
#
#   Cấu trúc file class.json (đặt trong mỗi thư mục khoá):
#     {
#       "sessions": 20,
#       "students": {
#         "nguyen-van-a": "nguyenvana_github",
#         "tran-thi-b":   "tranthib_github"
#       }
#     }
#
#   Vị trí đặt file:
#     embedded-linux/K26.1/class.json
#     embedded-linux/K26.2/class.json
#     rtos/K26.2/class.json
#     ...
#
#   Kết quả tạo ra:
#     embedded-linux/K26.1/
#     ├── nguyen-van-a/
#     │   ├── session-01/
#     │   ├── session-02/
#     │   └── ...session-20/
#     └── tran-thi-b/
#         ├── session-01/
#         └── ...session-20/
#
# QUY TẮC
#   - Thư mục đã tồn tại → bỏ qua, không ghi đè, không xoá
#   - Thư mục chưa có    → tạo mới và thêm .gitkeep
#   - Chạy lại bao nhiêu lần cũng an toàn
#
# KHI THÊM HỌC VIÊN MỚI
#   1. Mở file class.json của khoá tương ứng
#   2. Thêm dòng: "ten-hoc-vien": "github_username"
#   3. Chạy lại script — chỉ thư mục mới được tạo
#   4. git add . && git commit -m "add: học viên mới" && git push
#
# KHI THÊM BUỔI HỌC
#   1. Tăng trường "sessions" trong class.json
#   2. Chạy lại script — chỉ thư mục session mới được tạo
#   3. git add . && git commit -m "add: buổi học mới" && git push
# =============================================================

set -e

REPO_ROOT="$(cd "$(dirname "$0")" && pwd)"
SUBJECT="${1:-}"
COURSE="${2:-}"
CREATED=0
SKIPPED=0
ERRORS=0

echo "======================================"
echo "  DevLinux — Setup Student Folders"
echo "  Root: $REPO_ROOT"
if [ -n "$SUBJECT" ] && [ -n "$COURSE" ]; then
    echo "  Mode: Single class ($SUBJECT/$COURSE)"
else
    echo "  Mode: All classes"
fi
echo "======================================"
echo ""

# Build find command based on arguments
if [ -n "$SUBJECT" ] && [ -n "$COURSE" ]; then
    # Setup 1 lớp cụ thể
    FIND_CMD="find \"$REPO_ROOT/$SUBJECT/$COURSE\" -maxdepth 1 -name \"class.json\" -not -path \"*/.git/*\""
else
    # Setup tất cả lớp
    FIND_CMD="find \"$REPO_ROOT\" -mindepth 3 -name \"class.json\" -not -path \"*/.git/*\" | sort"
fi

# Tìm class.json
while IFS= read -r class_file; do
    course_dir="$(dirname "$class_file")"
    rel_course="${course_dir#$REPO_ROOT/}"

    echo "📂 $rel_course/class.json"

    # Kiểm tra file JSON hợp lệ
    if ! python3 -c "import json; json.load(open('$class_file'))" 2>/dev/null; then
        echo "   ❌ File JSON lỗi format — bỏ qua"
        ERRORS=$((ERRORS + 1))
        echo ""
        continue
    fi

    # Đọc số buổi học và danh sách học viên
    while IFS='|' read -r student github_user sessions; do
        student_dir="$course_dir/$student"

        # Tạo thư mục học viên nếu chưa có
        if [ -d "$student_dir" ]; then
            echo "   ⏭️  $student/ — đã tồn tại"
            SKIPPED=$((SKIPPED + 1))
        else
            mkdir -p "$student_dir"
            echo "   ✅ $student/ — đã tạo (github: $github_user)"
            CREATED=$((CREATED + 1))
        fi

        # Tạo thư mục session-XX trong thư mục học viên
        for i in $(seq 1 "$sessions"); do
            session_name=$(printf "session-%02d" "$i")
            session_dir="$student_dir/$session_name"
            if [ -d "$session_dir" ]; then
                SKIPPED=$((SKIPPED + 1))
            else
                mkdir -p "$session_dir"
                cat > "$session_dir/homework.md" << EOF
# Đề Bài — $session_name

<!-- Giáo viên điền đề bài vào đây theo format trong homework.template.md -->
EOF
                echo "      ✅ $session_name/ — đã tạo"
                CREATED=$((CREATED + 1))
            fi
        done

    done < <(python3 -c "
import json
data = json.load(open('$class_file'))
sessions = data.get('sessions', 0)
for student, github in data.get('students', {}).items():
    print(f'{student}|{github}|{sessions}')
")
    echo ""

done < <(eval "$FIND_CMD")

echo "======================================"
echo "  ✅ Tạo mới : $CREATED thư mục"
echo "  ⏭️  Bỏ qua  : $SKIPPED thư mục (đã có)"
if [ "$ERRORS" -gt 0 ]; then
    echo "  ❌ Lỗi     : $ERRORS file JSON"
fi
echo "======================================"
echo ""
echo "Chạy lệnh sau để push lên GitHub:"
echo "  git add ."
echo "  git commit -m \"setup: tạo thư mục học viên\""
echo "  git push"

#!/bin/bash
# =============================================================
#  distribute_projects.sh — Phân phối tài liệu project đến học viên
# =============================================================
#
# CÁCH DÙNG
#   Chạy từ root của repo devlinux:
#     bash distribute_projects.sh <subject/course>
#
# VÍ DỤ
#     bash distribute_projects.sh embedded-linux/K26.1
#     bash distribute_projects.sh embedded-linux/K26.2
#
# MÔ TẢ
#   Script đọc file project.json để biết học viên nào đã đăng ký
#   project nào (P1, P2, P3, hay P4), rồi copy tài liệu đó vào
#   thư mục project/ của từng học viên tương ứng.
#
# CẤU TRÚC YÊU CẦU
#   subject/course/
#   ├── class.json
#   ├── project.json          ← Chứa mapping "student" : "Px" + "reviewers": [...]
#   ├── projects/
#   │   ├── RULES.md          ← Shared
#   │   ├── TEST_REPORT_TEMPLATE.md  ← Shared
#   │   ├── P1/
#   │   │   ├── P1_PROBLEM_STATEMENT.md
#   │   │   ├── P1_DESIGN_TEMPLATE.md
#   │   │   └── P1_TESTCASES_STUDENT.md
#   │   ├── P2/
#   │   ├── P3/
#   │   └── P4/
#   └── (thư mục học viên đã tạo bởi setup_students.sh)
#
# FORMAT project.json
#   {
#     "projects": {
#       "student-name-1": "P1",
#       "student-name-2": "P2"
#     },
#     "reviewers": ["teacher-1", "teacher-2"]
#   }
#
#   - "projects": mapping học viên → project ID
#   - "reviewers": danh sách GitHub usernames cần review design & project
#
# KẾT QUẢ
#   subject/course/nguyen-van-a/project/
#   ├── RULES.md
#   ├── TEST_REPORT_TEMPLATE.md
#   └── P1/                  ← Chỉ project học viên đó chọn
#       ├── P1_PROBLEM_STATEMENT.md
#       ├── P1_DESIGN_TEMPLATE.md
#       └── P1_TESTCASES_STUDENT.md
#
# LƯU Ý
#   - Ghi đè project/ đã có — chạy lại an toàn
#   - Chỉ copy shared files + project folder mà học viên chọn
#   - Học viên chưa có thư mục → bỏ qua với cảnh báo
# =============================================================

set -e

REPO_ROOT="$(cd "$(dirname "$0")" && pwd)"
COURSE_ARG="${1:-}"

if [ -z "$COURSE_ARG" ]; then
    echo "❌ Thiếu tham số. Cách dùng:"
    echo "   bash distribute_projects.sh <subject/course>"
    echo "   Ví dụ: bash distribute_projects.sh embedded-linux/K26.1"
    exit 1
fi

COURSE_DIR="$REPO_ROOT/$COURSE_ARG"
PROJECTS_DIR="$COURSE_DIR/projects"
PROJECT_JSON="$COURSE_DIR/project.json"

if [ ! -d "$COURSE_DIR" ]; then
    echo "❌ Không tìm thấy thư mục: $COURSE_DIR"
    exit 1
fi

if [ ! -d "$PROJECTS_DIR" ]; then
    echo "❌ Không tìm thấy thư mục projects/: $PROJECTS_DIR"
    echo "   Tạo cấu trúc projects/ trước."
    exit 1
fi

if [ ! -f "$PROJECT_JSON" ]; then
    echo "❌ Không tìm thấy project.json: $PROJECT_JSON"
    echo "   Tạo file này với mapping:"
    echo "   {\"projects\": {\"student-name\": \"P1\", ...}}"
    exit 1
fi

echo "======================================"
echo "  DevLinux — Distribute Projects"
echo "  Khoá : $COURSE_ARG"
echo "======================================"
echo ""

COPIED=0
SKIPPED=0

# Đọc project.json và xử lý từng học viên
python3 << PYTHON_SCRIPT
import json
import shutil
import os
import re
from pathlib import Path

PROJECT_JSON = "$PROJECT_JSON"
PROJECTS_DIR = "$PROJECTS_DIR"
COURSE_DIR = "$COURSE_DIR"
REPO_ROOT = "$REPO_ROOT"

# Load project.json
try:
    with open(PROJECT_JSON, 'r') as f:
        data = json.load(f)
    projects_map = data.get('projects', {})
    reviewers = data.get('reviewers', [])
except Exception as e:
    print(f"❌ Lỗi đọc project.json: {e}")
    exit(1)

if not projects_map:
    print("⚠️  Không có học viên nào trong project.json")
    exit(0)

COPIED = 0
SKIPPED = 0

# Duyệt từng học viên trong project.json
for student, project_id in sorted(projects_map.items()):
    student_dir = Path(COURSE_DIR) / student
    project_src = Path(PROJECTS_DIR) / project_id
    student_project_dir = student_dir / "project"

    # Kiểm tra thư mục học viên
    if not student_dir.exists():
        print(f"   ⏭️  {student}/ — thư mục chưa tồn tại, bỏ qua")
        SKIPPED += 1
        continue

    # Kiểm tra project tồn tại
    if not project_src.exists():
        print(f"   ❌ {student} → {project_id}/ không tìm thấy")
        SKIPPED += 1
        continue

    # Tạo project/ folder
    student_project_dir.mkdir(parents=True, exist_ok=True)

    # Copy shared files
    for shared_file in ["RULES.md", "TEST_REPORT_TEMPLATE.md"]:
        src = Path(PROJECTS_DIR) / shared_file
        if src.exists():
            dst = student_project_dir / shared_file
            shutil.copy2(str(src), str(dst))

    # Copy project folder
    project_dst = student_project_dir / project_id
    if project_dst.exists():
        shutil.rmtree(str(project_dst))
    shutil.copytree(str(project_src), str(project_dst))

    print(f"✅ {student} → {project_id}")
    COPIED += 1

# Generate CODEOWNERS entries if reviewers defined
if reviewers:
    codeowners_file = Path(REPO_ROOT) / ".github" / "CODEOWNERS"

    # Extract subject and course from COURSE_DIR
    course_parts = COURSE_DIR.rstrip('/').split('/')
    subject = course_parts[-2]
    course = course_parts[-1]

    reviewers_str = " ".join(f"@{r}" for r in reviewers)

    codeowners_lines = [
        f"# {subject}/{course} (auto-generated by distribute_projects.sh)",
        f"{subject}/{course}/project/P*/*/design {reviewers_str}",
        f"{subject}/{course}/project/P*/*/project {reviewers_str}",
        ""
    ]

    # Read existing content
    existing_content = ""
    if codeowners_file.exists():
        with open(codeowners_file, 'r') as f:
            existing_content = f.read()

    # Remove old entries for this course (if any)
    course_pattern = f"# {subject}/{course}"
    if course_pattern in existing_content:
        # Remove all lines until next course comment or EOF
        lines = existing_content.split('\n')
        new_lines = []
        skip_mode = False
        for line in lines:
            if line.startswith(f"# {subject}/{course}"):
                skip_mode = True
                continue
            elif line.startswith("# ") and skip_mode:
                skip_mode = False

            if not skip_mode and line.strip():
                new_lines.append(line)

        existing_content = '\n'.join(new_lines).strip() + '\n'

    # Append new entries
    with open(codeowners_file, 'w') as f:
        if existing_content:
            f.write(existing_content)
            f.write('\n')
        f.writelines('\n'.join(codeowners_lines))

    print(f"📝 Updated .github/CODEOWNERS for {subject}/{course}")

print("")
print("======================================")
print(f"  ✅ Đã copy : {COPIED} học viên")
print(f"  ⏭️  Bỏ qua  : {SKIPPED} thư mục không tồn tại")
print("======================================")
print("")
print("Chạy lệnh sau để push lên GitHub:")
print("  git add .")
print(f'  git commit -m "projects: phân phối tài liệu {os.path.basename(COURSE_DIR)}"')
print("  git push")

PYTHON_SCRIPT
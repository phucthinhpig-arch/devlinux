#!/bin/bash
# =============================================================
#  setup_new_class.sh — Tạo lớp học mới tự động
# =============================================================
#
# CÁCH DÙNG
#   ./setup_new_class.sh <subject> <course>
#
# VÍ DỤ
#   ./setup_new_class.sh embedded-linux K26.1
#   ./setup_new_class.sh c-basic K26.2
#   ./setup_new_class.sh freertos K26.1
#
# MÔ TẢ
#   Script tạo thư mục lớp mới với cấu trúc đầy đủ:
#   - Thư mục: {subject}/{course}/
#   - File class.json (template)
#   - Thư mục homeworks/
#
# =============================================================

set -e

# Kiểm tra tham số
if [ $# -ne 2 ]; then
    echo "❌ Usage: ./setup_new_class.sh <subject> <course>"
    echo ""
    echo "Subjects: embedded-linux, embedded-mcu, c-advance, c-basic, freertos"
    echo "Example: ./setup_new_class.sh embedded-linux K26.1"
    exit 1
fi

SUBJECT=$1
COURSE=$2
REPO_ROOT="$(cd "$(dirname "$0")" && pwd)"

# Validate subject
VALID_SUBJECTS="embedded-linux embedded-mcu c-advance c-basic freertos"
if ! echo "$VALID_SUBJECTS" | grep -q "\b$SUBJECT\b"; then
    echo "❌ Invalid subject: $SUBJECT"
    echo "Valid subjects: $VALID_SUBJECTS"
    exit 1
fi

# Validate course format (K##.# hoặc K##.##)
if ! echo "$COURSE" | grep -qE "^K[0-9]{2}\.[0-9]{1,2}$"; then
    echo "❌ Invalid course format: $COURSE"
    echo "Expected format: K26.1, K26.2, etc."
    exit 1
fi

COURSE_DIR="$REPO_ROOT/$SUBJECT/$COURSE"

# Check if already exists
if [ -d "$COURSE_DIR" ]; then
    echo "⚠️  Course directory already exists: $COURSE_DIR"
    exit 1
fi

# Create directories
echo "📂 Creating $SUBJECT/$COURSE/..."
mkdir -p "$COURSE_DIR/homeworks"
echo "   ✅ Created directories"

# Create class.json template
cat > "$COURSE_DIR/class.json" << 'EOF'
{
  "sessions": 20,
  "students": {
    "ten-hoc-vien-1": "github_username_1",
    "ten-hoc-vien-2": "github_username_2"
  }
}
EOF
echo "   ✅ Created class.json template"

# Create a sample homework file for session-01
cat > "$COURSE_DIR/homeworks/session-01.md" << 'EOF'
# Assignment — session-01

**Deadline: 2026-07-17 23:59:00**

---

<!-- Giáo viên điền đề bài vào đây -->

EOF
echo "   ✅ Created homeworks/session-01.md template"

echo ""
echo "======================================"
echo "✅ Class setup complete!"
echo "======================================"
echo ""
echo "📋 Next steps:"
echo "1. Edit $COURSE_DIR/class.json:"
echo "   - Update student names and GitHub usernames"
echo "   - Adjust 'sessions' if needed (default: 20)"
echo ""
echo "2. Run setup_students.sh to create student folders:"
echo "   bash setup_students.sh $SUBJECT $COURSE"
echo ""
echo "3. Edit $COURSE_DIR/homeworks/session-XX.md:"
echo "   - Add deadline and problem statement for each session"
echo ""
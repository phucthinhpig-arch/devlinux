#!/bin/bash

# Setup new class folder structure
# Usage: ./setup_new_class.sh <subject> <course>
# Example: ./setup_new_class.sh embedded-linux K26.2

set -e

# Color codes
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Get arguments
SUBJECT="${1:-}"
COURSE="${2:-}"

# Validate inputs
if [ -z "$SUBJECT" ] || [ -z "$COURSE" ]; then
    echo -e "${RED}Error: Missing arguments${NC}"
    echo "Usage: $0 <subject> <course>"
    echo "Example: $0 embedded-linux K26.2"
    exit 1
fi

# Validate subject
VALID_SUBJECTS=("embedded-linux" "c-advance" "c-basic" "embedded-mcu" "freertos" "rtos")
if [[ ! " ${VALID_SUBJECTS[@]} " =~ " ${SUBJECT} " ]]; then
    echo -e "${RED}Error: Invalid subject '${SUBJECT}'${NC}"
    echo "Valid subjects: ${VALID_SUBJECTS[*]}"
    exit 1
fi

# Validate course format (K26.1, K26.2, etc.)
if ! [[ "$COURSE" =~ ^K[0-9]+\.[0-9]+$ ]]; then
    echo -e "${RED}Error: Invalid course format '${COURSE}'${NC}"
    echo "Expected format: K26.1, K26.2, etc."
    exit 1
fi

# Set paths
CLASS_DIR="$(dirname "$0")/${SUBJECT}/${COURSE}"
HOMEWORKS_DIR="${CLASS_DIR}/homeworks"

# Check if class already exists
if [ -d "$CLASS_DIR" ]; then
    echo -e "${YELLOW}Warning: Class directory already exists: ${CLASS_DIR}${NC}"
    read -p "Continue? (y/n) " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        echo "Aborted."
        exit 1
    fi
fi

# Create directories
echo -e "${GREEN}Creating directory structure...${NC}"
mkdir -p "$CLASS_DIR"
mkdir -p "$HOMEWORKS_DIR"

# Copy class.json template if it doesn't exist
if [ ! -f "$CLASS_DIR/class.json" ]; then
    echo -e "${GREEN}Creating class.json template...${NC}"
    cat > "$CLASS_DIR/class.json" << 'EOF'
{
  "sessions": 20,
  "students": {
    "nguyen-van-a": "nguyenvana_github",
    "tran-thi-b":   "tranthib_github",
    "le-van-c":     "levanc_github"
  }
}
EOF
    echo -e "${YELLOW}Note: Please update class.json with actual student data${NC}"
else
    echo -e "${YELLOW}class.json already exists, skipping${NC}"
fi

# Create .gitkeep in homeworks folder
touch "$HOMEWORKS_DIR/.gitkeep"

echo ""
echo -e "${GREEN}✅ Setup complete!${NC}"
echo ""
echo "Created:"
echo "  📁 $CLASS_DIR"
echo "  📁 $HOMEWORKS_DIR"
echo "  📄 $CLASS_DIR/class.json (template)"
echo ""
echo -e "${YELLOW}Next steps:${NC}"
echo "  1. Edit $CLASS_DIR/class.json with actual student data"
echo "  2. Run: ./setup_students.sh (from devlinux root)"
echo ""
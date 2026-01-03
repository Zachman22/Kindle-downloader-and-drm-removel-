#!/bin/bash

# Kindle DRM Converter - Qt C++ Build Script

set -e  # Exit on error

echo "============================================================"
echo "Kindle DRM Converter - Qt C++ Build Script"
echo "============================================================"
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check for Qt installation
echo "Checking for Qt installation..."
if command -v qmake &> /dev/null; then
    QT_VERSION=$(qmake --version | grep "Qt version" | awk '{print $4}')
    echo -e "${GREEN}✓${NC} Qt ${QT_VERSION} found"
else
    echo -e "${RED}✗${NC} Qt not found. Please install Qt 5.15+ or Qt 6.x"
    echo "  Linux: sudo apt-get install qt6-base-dev qt6-webengine-dev"
    echo "  macOS: brew install qt@6"
    exit 1
fi

# Check for CMake
echo "Checking for CMake..."
if command -v cmake &> /dev/null; then
    CMAKE_VERSION=$(cmake --version | head -n1 | awk '{print $3}')
    echo -e "${GREEN}✓${NC} CMake ${CMAKE_VERSION} found"
else
    echo -e "${RED}✗${NC} CMake not found. Please install CMake 3.16+"
    exit 1
fi

# Check for Calibre
echo "Checking for Calibre..."
if command -v ebook-convert &> /dev/null; then
    CALIBRE_VERSION=$(ebook-convert --version | head -n1 | awk '{print $2}')
    echo -e "${GREEN}✓${NC} Calibre ${CALIBRE_VERSION} found"
else
    echo -e "${YELLOW}⚠${NC} Calibre not found. Install it for ebook conversion:"
    echo "  Linux: sudo apt-get install calibre"
    echo "  macOS: brew install calibre"
fi

echo ""

# Build type
BUILD_TYPE=${1:-Release}
echo "Build type: ${BUILD_TYPE}"

# Create build directory
BUILD_DIR="build-${BUILD_TYPE}"
echo "Creating build directory: ${BUILD_DIR}"
mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}

# Configure with CMake
echo ""
echo "Configuring with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=${BUILD_TYPE}

# Build
echo ""
echo "Building..."
CORES=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
make -j${CORES}

echo ""
echo -e "${GREEN}============================================================${NC}"
echo -e "${GREEN}Build Complete!${NC}"
echo -e "${GREEN}============================================================${NC}"
echo ""
echo "Executable: ${BUILD_DIR}/KindleDRMConverter"
echo ""
echo "To run:"
echo "  cd ${BUILD_DIR}"
echo "  ./KindleDRMConverter --help"
echo ""
echo "To install (optional):"
echo "  sudo make install"
echo ""

# Create config if it doesn't exist
if [ ! -f "../config.yaml" ]; then
    echo -e "${YELLOW}Configuration file not found.${NC}"
    echo "Creating config.yaml from example..."
    cp ../config.yaml.example ../config.yaml
    echo -e "${GREEN}✓${NC} Created config.yaml - please edit with your credentials"
    echo ""
fi

# Create output directories
mkdir -p downloads output temp logs
echo -e "${GREEN}✓${NC} Created output directories"

echo ""
echo "Next steps:"
echo "1. Edit config.yaml with your Amazon credentials"
echo "2. Run: cd ${BUILD_DIR} && ./KindleDRMConverter --all"
echo ""

# Building Windows EXE - Complete Guide

This guide shows you how to create a standalone Windows executable (EXE) for the Kindle DRM Converter.

## Table of Contents

- [Quick Start (Pre-built Method)](#quick-start-pre-built-method)
- [Building from Source](#building-from-source)
- [Creating Standalone EXE](#creating-standalone-exe)
- [Creating Installer](#creating-installer)
- [Cross-Compilation from Linux](#cross-compilation-from-linux)

---

## Quick Start (Pre-built Method)

If you just want to run the application without building:

1. Download the latest release from GitHub
2. Extract the ZIP file
3. Run `KindleDRMConverter.exe`

---

## Building from Source

### Prerequisites

#### 1. Install Qt Framework

**Option A: Qt Online Installer (Recommended)**
1. Download from: https://www.qt.io/download-qt-installer
2. Run the installer
3. Select Qt 6.5 or later
4. Choose components:
   - Qt WebEngine
   - Qt Network
   - MinGW 11.2.0 64-bit (or MSVC 2019)

**Option B: Qt via MSYS2** (Advanced users)
```bash
# In MSYS2 MinGW64 terminal
pacman -S mingw-w64-x86_64-qt6-base
pacman -S mingw-w64-x86_64-qt6-webengine
```

#### 2. Install CMake
- Download from: https://cmake.org/download/
- Choose "Add CMake to system PATH" during installation

#### 3. Install C++ Compiler

**Option A: MinGW (with Qt)**
- Already included if you installed Qt with MinGW

**Option B: Visual Studio**
- Download Visual Studio 2019 or later
- Install "Desktop development with C++" workload

#### 4. Install Calibre (Required)
- Download from: https://calibre-ebook.com/download
- Install to default location

### Build Steps

#### Using Command Line (MinGW)

```batch
# 1. Open Qt MinGW 64-bit console (from Start Menu)
cd path\to\Kindle-downloader-and-drm-removel-\qt-version

# 2. Run build script
build.bat

# 3. The EXE will be in: build-Release\bin\KindleDRMConverter.exe
```

#### Using Command Line (Visual Studio)

```batch
# 1. Open "Developer Command Prompt for VS 2019" (from Start Menu)
cd path\to\Kindle-downloader-and-drm-removel-\qt-version

# 2. Configure
mkdir build-vs
cd build-vs
cmake .. -G "Visual Studio 16 2019" -A x64

# 3. Build
cmake --build . --config Release

# 4. EXE location: build-vs\bin\Release\KindleDRMConverter.exe
```

#### Using Qt Creator (GUI Method)

1. Open Qt Creator
2. File → Open File or Project
3. Select `CMakeLists.txt` from `qt-version` folder
4. Configure with your Qt kit
5. Click Build (Ctrl+B)
6. Click Run (Ctrl+R)

---

## Creating Standalone EXE

After building, create a standalone package with all dependencies:

```batch
# In qt-version directory
deploy-windows.bat
```

This creates `KindleDRMConverter-Windows` folder with:
- ✅ KindleDRMConverter.exe
- ✅ All Qt DLLs (Qt6Core.dll, Qt6Network.dll, etc.)
- ✅ Configuration files
- ✅ Documentation
- ✅ Launcher script

**Distribution**: Zip the `KindleDRMConverter-Windows` folder and share!

### Manual Deployment (if script fails)

If `deploy-windows.bat` doesn't work:

```batch
# 1. Copy your EXE
mkdir Deploy
copy build-Release\bin\KindleDRMConverter.exe Deploy\

# 2. Run windeployqt manually
cd Deploy
windeployqt KindleDRMConverter.exe

# 3. Copy additional files
copy ..\config.yaml.example .
copy ..\README_QT.md README.md
```

---

## Creating Installer

To create a professional Windows installer (`.exe`):

### Prerequisites

Install NSIS (Nullsoft Scriptable Install System):
1. Download from: https://nsis.sourceforge.io/
2. Install to default location
3. Add NSIS to PATH (or note installation path)

### Build Installer

```batch
# 1. First create deployment
deploy-windows.bat

# 2. Create installer
create-installer.bat
```

This creates `KindleDRMConverter-Setup.exe` with:
- ✅ Professional installer UI
- ✅ Start Menu shortcuts
- ✅ Desktop shortcut
- ✅ Uninstaller
- ✅ Checks for Calibre
- ✅ Registry entries

### Installer Features

Users can:
- Install to Program Files
- Launch from Start Menu
- Uninstall cleanly
- Get prompted if Calibre is missing

---

## Cross-Compilation from Linux

Build Windows EXE on Linux using MXE (cross-compiler):

### Setup MXE

```bash
# Install dependencies
sudo apt-get install \
    autoconf automake autopoint bash bison bzip2 flex g++ \
    gperf intltool libtool-bin libltdl-dev libssl-dev \
    libxml-parser-perl lzip make openssl p7zip-full patch \
    perl python ruby sed unzip wget xz-utils

# Clone MXE
git clone https://github.com/mxe/mxe.git
cd mxe

# Build Qt6 for Windows (this takes several hours!)
make MXE_TARGETS='x86_64-w64-mingw32.shared' qt6-qtbase qt6-qtwebengine
```

### Cross-Compile

```bash
cd Kindle-downloader-and-drm-removel-/qt-version

# Set up MXE environment
export PATH=/path/to/mxe/usr/bin:$PATH

# Configure with MXE
mkdir build-windows
cd build-windows
x86_64-w64-mingw32.shared-cmake ..

# Build
make

# Result: KindleDRMConverter.exe
```

---

## Troubleshooting

### "Qt not found"

**Solution**: Add Qt to PATH or specify Qt directory:
```batch
set CMAKE_PREFIX_PATH=C:\Qt\6.5.0\mingw_64
cmake ..
```

### "windeployqt not found"

**Solution**: Add Qt bin to PATH:
```batch
set PATH=%PATH%;C:\Qt\6.5.0\mingw_64\bin
```

### "Missing DLL" when running EXE

**Solutions**:
1. Run `windeployqt KindleDRMConverter.exe` in the EXE directory
2. Copy missing DLL from `C:\Qt\6.5.0\mingw_64\bin\`
3. Use Dependency Walker to find missing DLLs: https://dependencywalker.com/

### Build errors with WebEngine

**Solution**: Ensure Qt WebEngine is installed:
```batch
# Verify installation
dir C:\Qt\6.5.0\mingw_64\lib\cmake\Qt6WebEngine*
```

If missing, run Qt Maintenance Tool and add WebEngine component.

### Large EXE size

**Solutions**:
1. **Strip symbols** (release builds):
   ```batch
   strip KindleDRMConverter.exe
   ```

2. **Use static linking** (edit CMakeLists.txt):
   ```cmake
   set(BUILD_STATIC ON)
   ```

3. **Compress with UPX**:
   ```batch
   upx --best KindleDRMConverter.exe
   ```

---

## File Sizes

Expected sizes after build:

| Item | Size |
|------|------|
| KindleDRMConverter.exe (alone) | ~500 KB - 2 MB |
| With Qt DLLs (deployed) | ~80-150 MB |
| Installer (compressed) | ~60-100 MB |
| After UPX compression | ~40-60 MB |

---

## Static Build (Advanced)

For a truly standalone single EXE with no DLL dependencies:

### 1. Build Qt Statically

```batch
# Download Qt source
# Configure Qt for static build
configure -static -release -prefix C:\Qt\6.5.0-static ^
    -opensource -confirm-license ^
    -nomake examples -nomake tests

# Build (takes hours!)
cmake --build . --parallel
cmake --install .
```

### 2. Build Application

```batch
set CMAKE_PREFIX_PATH=C:\Qt\6.5.0-static
cmake .. -DBUILD_STATIC=ON
cmake --build . --config Release
```

Result: Single EXE (~150-200 MB) with no dependencies!

**Note**: Qt WebEngine cannot be built statically. For static builds, you'll need to modify the downloader to use Qt Network only.

---

## Distributing Your EXE

### Option 1: ZIP Archive
```batch
# Create ZIP
cd KindleDRMConverter-Windows
"C:\Program Files\7-Zip\7z.exe" a ..\KindleDRMConverter-v1.0-Windows.zip *
```

### Option 2: Installer
```batch
# Creates professional installer
create-installer.bat
# Distribute: KindleDRMConverter-Setup.exe
```

### Option 3: Portable
```batch
# Single folder, no installation needed
# Just copy KindleDRMConverter-Windows folder
# Users can run from USB drive
```

---

## Next Steps

After building:

1. **Test the EXE** on a clean Windows machine
2. **Check dependencies** with Dependency Walker
3. **Create documentation** for users
4. **Sign the EXE** (optional, for Windows SmartScreen)
5. **Upload to GitHub** releases

---

## Resources

- Qt Documentation: https://doc.qt.io/
- CMake Tutorial: https://cmake.org/cmake/help/latest/guide/tutorial/
- NSIS Documentation: https://nsis.sourceforge.io/Docs/
- MinGW-w64: https://www.mingw-w64.org/
- Qt Forum: https://forum.qt.io/

---

## Summary Commands

```batch
# Complete build process:
cd qt-version
build.bat                    # Build EXE
deploy-windows.bat           # Create standalone package
create-installer.bat         # Create installer (optional)

# Result files:
# - build-Release\bin\KindleDRMConverter.exe  (needs Qt DLLs)
# - KindleDRMConverter-Windows\               (standalone folder)
# - KindleDRMConverter-Setup.exe              (installer)
```

Happy building! 🚀

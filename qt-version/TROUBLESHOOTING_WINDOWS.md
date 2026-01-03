# Windows Build Troubleshooting Guide

## Problem: deploy-windows.bat opens and closes immediately

This means the script found an error and exited. Here's how to fix it:

### Solution 1: Check if Build Exists

Run this diagnostic script first:

```batch
check-build.bat
```

This will show you:
- ✅ If the build exists
- ✅ What directories are present
- ✅ Where the EXE should be

### Solution 2: Build First!

The **deploy-windows.bat** script requires that you build first. You need to run:

```batch
# Step 1: Build the application
build.bat

# Step 2: THEN deploy
deploy-windows.bat
```

### Solution 3: Run from Correct Directory

Make sure you're in the `qt-version` directory:

```batch
# Check where you are
cd

# Should show: ...Kindle-downloader-and-drm-removel-\qt-version

# If not, navigate to it:
cd path\to\Kindle-downloader-and-drm-removel-\qt-version
```

### Solution 4: Run from Command Prompt

Instead of double-clicking, open Command Prompt:

```batch
# 1. Press Win + R
# 2. Type: cmd
# 3. Navigate to qt-version folder
cd C:\path\to\Kindle-downloader-and-drm-removel-\qt-version

# 4. Run the script
deploy-windows.bat
```

This way you'll see ALL the output and any error messages.

---

## Problem: build.bat fails

### Check Qt Installation

```batch
# Check if qmake is available
qmake --version

# Check if CMake is available
cmake --version

# If not found, you need to install Qt and CMake
```

### Install Qt

1. Download Qt from: https://www.qt.io/download-qt-installer
2. Install Qt 6.x with these components:
   - ✅ Qt 6.x (latest version)
   - ✅ Qt WebEngine
   - ✅ MinGW 11.2.0 64-bit
   - ✅ CMake
   - ✅ Qt Creator (optional but helpful)

3. Add Qt to PATH:
   ```batch
   set PATH=%PATH%;C:\Qt\6.5.0\mingw_64\bin
   set PATH=%PATH%;C:\Qt\Tools\mingw1120_64\bin
   set PATH=%PATH%;C:\Qt\Tools\CMake_64\bin
   ```

### Alternative: Use Qt Creator

If command line builds fail:

1. Open Qt Creator
2. File → Open File or Project
3. Select `CMakeLists.txt` from qt-version folder
4. Configure with your Qt kit
5. Build → Build Project (Ctrl+B)
6. The EXE will be in the build directory

---

## Problem: "Qt6WebEngine not found"

Qt WebEngine is not installed.

### Fix:

1. Run Qt Maintenance Tool (in Qt installation folder)
2. Select "Add or remove components"
3. Expand Qt 6.x → Select **Qt WebEngine**
4. Install

OR install via command line (if using MSYS2):
```batch
pacman -S mingw-w64-x86_64-qt6-webengine
```

---

## Problem: "windeployqt not found"

### Fix:

Add Qt's bin directory to PATH:

```batch
set PATH=%PATH%;C:\Qt\6.5.0\mingw_64\bin
```

Then retry `deploy-windows.bat`

---

## Problem: Build succeeds but EXE crashes

### Check Dependencies

Use this to see what's missing:

```batch
# Navigate to build directory
cd build-Release\bin

# List DLLs
dumpbin /dependents KindleDRMConverter.exe
```

### Common Missing DLLs:

- **Qt6Core.dll** → Run windeployqt
- **Qt6WebEngine*.dll** → Install Qt WebEngine
- **VCRUNTIME140.dll** → Install Visual C++ Redistributable

### Install VC++ Redistributable:

Download from: https://aka.ms/vs/17/release/vc_redist.x64.exe

---

## Problem: Cannot find config.yaml

### Fix:

Copy the example config:

```batch
copy config.yaml.example config.yaml
notepad config.yaml
```

Edit with your Amazon credentials.

---

## Problem: Calibre not found

### Fix:

1. Install Calibre: https://calibre-ebook.com/download
2. Add to PATH:
   ```batch
   set PATH=%PATH%;C:\Program Files\Calibre2
   ```
3. Verify:
   ```batch
   ebook-convert --version
   ```

---

## Complete Build Process (Step-by-Step)

### Prerequisites Checklist:

- [ ] Qt 6.x installed (with WebEngine)
- [ ] CMake installed
- [ ] MinGW or Visual Studio installed
- [ ] Calibre installed
- [ ] You're in `qt-version` directory

### Build Steps:

```batch
# 1. Verify you're in the right place
cd
# Should show: ...\qt-version

# 2. Check build tools
qmake --version
cmake --version

# 3. Build
build.bat
# Wait for it to complete (may take 5-10 minutes first time)

# 4. Verify build succeeded
dir build-Release\bin\KindleDRMConverter.exe
# Should show the EXE file

# 5. Deploy
deploy-windows.bat
# This bundles all Qt DLLs

# 6. Verify deployment
dir KindleDRMConverter-Windows
# Should show folder with EXE and DLLs

# 7. Test
cd KindleDRMConverter-Windows
KindleDRMConverter.exe --help
```

---

## Still Having Issues?

### Get Detailed Error Information:

```batch
# Run with error logging
build.bat > build-log.txt 2>&1
type build-log.txt
```

### Check System Info:

```batch
# Windows version
ver

# Architecture
echo %PROCESSOR_ARCHITECTURE%

# Qt installation
where qmake
where cmake

# Compiler
where g++
where cl.exe
```

---

## Common Error Messages and Fixes

### "CMake Error: Could not find CMAKE_ROOT"

**Fix**: Reinstall CMake or add to PATH

### "No CMAKE_CXX_COMPILER could be found"

**Fix**: Install MinGW or Visual Studio

### "'qmake' is not recognized"

**Fix**: Install Qt or add Qt bin to PATH

### "Qt6 not found"

**Fix**:
```batch
set CMAKE_PREFIX_PATH=C:\Qt\6.5.0\mingw_64
```

### "Permission denied"

**Fix**: Run Command Prompt as Administrator

---

## Quick Diagnostic Script

Save this as `diagnose.bat` and run it:

```batch
@echo off
echo Qt Installation:
where qmake
qmake --version
echo.

echo CMake:
where cmake
cmake --version
echo.

echo Compiler:
where g++
where cl.exe
echo.

echo Calibre:
where ebook-convert
ebook-convert --version
echo.

echo Current Directory:
cd
echo.

echo Files in current directory:
dir /b
echo.

pause
```

---

## Need More Help?

1. **Check build-log.txt** - Contains detailed error messages
2. **Run check-build.bat** - Diagnoses build issues
3. **Use Qt Creator** - Visual IDE with better error messages
4. **GitHub Issues** - Report bugs with error logs

---

## Working Example (Full Output)

When everything works, you should see:

```
C:\...\qt-version> build.bat
[Output showing Qt found, CMake configuring, compiling...]
Build complete!

C:\...\qt-version> deploy-windows.bat
[1/5] Copying executable...
[OK] Executable copied
[2/5] Deploying Qt dependencies...
[OK] Qt dependencies deployed
[3/5] Copying configuration files...
[OK] Configuration files copied
[4/5] Creating directory structure...
[OK] Directories created
[5/5] Creating launcher script...
[OK] Launcher created
============================================================
Deployment Complete!
============================================================
```

That's what success looks like! ✅

@echo off
REM Kindle DRM Converter - Qt C++ Build Script for Windows

echo ============================================================
echo Kindle DRM Converter - Qt C++ Build Script (Windows)
echo ============================================================
echo.

REM Check for CMake
where cmake >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] CMake not found. Please install CMake 3.16+
    echo Download from: https://cmake.org/download/
    exit /b 1
)

echo [OK] CMake found
cmake --version | findstr /C:"version"
echo.

REM Check for Qt (basic check)
where qmake >nul 2>nul
if %ERRORLEVEL% EQU 0 (
    echo [OK] Qt found
    qmake --version | findstr /C:"Qt version"
) else (
    echo [WARNING] qmake not found in PATH
    echo Make sure Qt is installed and CMAKE_PREFIX_PATH is set
)
echo.

REM Check for Calibre
where ebook-convert >nul 2>nul
if %ERRORLEVEL% EQU 0 (
    echo [OK] Calibre found
) else (
    echo [WARNING] Calibre not found
    echo Install from: https://calibre-ebook.com/download
)
echo.

REM Build type
set BUILD_TYPE=Release
if not "%1"=="" set BUILD_TYPE=%1

echo Build type: %BUILD_TYPE%
echo.

REM Create build directory
set BUILD_DIR=build-%BUILD_TYPE%
echo Creating build directory: %BUILD_DIR%
if not exist %BUILD_DIR% mkdir %BUILD_DIR%
cd %BUILD_DIR%

REM Configure with CMake
echo.
echo Configuring with CMake...
cmake .. -G "Visual Studio 16 2019" -DCMAKE_BUILD_TYPE=%BUILD_TYPE%
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] CMake configuration failed
    cd ..
    exit /b 1
)

REM Build
echo.
echo Building...
cmake --build . --config %BUILD_TYPE%
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Build failed
    cd ..
    exit /b 1
)

echo.
echo ============================================================
echo Build Complete!
echo ============================================================
echo.
echo Executable: %BUILD_DIR%\%BUILD_TYPE%\KindleDRMConverter.exe
echo.
echo To run:
echo   cd %BUILD_DIR%\%BUILD_TYPE%
echo   KindleDRMConverter.exe --help
echo.

REM Create config if it doesn't exist
cd ..
if not exist config.yaml (
    echo [INFO] Creating config.yaml from example...
    copy config.yaml.example config.yaml
    echo [OK] Created config.yaml - please edit with your credentials
    echo.
)

REM Create output directories
if not exist downloads mkdir downloads
if not exist output mkdir output
if not exist temp mkdir temp
if not exist logs mkdir logs
echo [OK] Created output directories
echo.

echo Next steps:
echo 1. Edit config.yaml with your Amazon credentials
echo 2. Run: cd %BUILD_DIR%\%BUILD_TYPE% ^&^& KindleDRMConverter.exe --all
echo.

pause

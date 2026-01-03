@echo off
REM Simple test script to check if the build exists

echo ============================================================
echo Kindle DRM Converter - Build Check
echo ============================================================
echo.

echo Current Directory:
cd
echo.

echo Checking for build directories...
echo.

if exist "build-Release" (
    echo [OK] build-Release directory exists
    dir build-Release
) else (
    echo [NOT FOUND] build-Release directory
)
echo.

if exist "build-Release\bin" (
    echo [OK] build-Release\bin directory exists
    dir build-Release\bin
) else (
    echo [NOT FOUND] build-Release\bin directory
)
echo.

if exist "build-Release\bin\KindleDRMConverter.exe" (
    echo [OK] KindleDRMConverter.exe found!
    dir build-Release\bin\KindleDRMConverter.exe
) else (
    echo [NOT FOUND] KindleDRMConverter.exe
)
echo.

echo Other directories in current folder:
dir /b /ad
echo.

echo ============================================================
echo Next Steps:
echo ============================================================
echo.
echo If KindleDRMConverter.exe was NOT found:
echo   1. You need to build first: run build.bat
echo   2. Make sure Qt is installed
echo   3. Make sure you're in the qt-version directory
echo.
echo If KindleDRMConverter.exe WAS found:
echo   - Run deploy-windows.bat to create standalone package
echo.

pause

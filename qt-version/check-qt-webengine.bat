@echo off
echo.
echo ============================================================
echo Qt WebEngine Installation Checker
echo ============================================================
echo.

REM Check if Qt is installed
where qmake >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [NOT FOUND] Qt is not installed
    echo.
    echo DOWNLOAD Qt from:
    echo https://www.qt.io/download-qt-installer
    echo.
    echo During installation, make sure to CHECK:
    echo   - Qt 6.x (latest version)
    echo   - Qt WebEngine (in Additional Libraries)
    echo   - MinGW 64-bit compiler
    echo   - CMake
    echo.
    goto end
)

echo [FOUND] Qt is installed
qmake --version
echo.

REM Get Qt installation path
for /f "tokens=*" %%i in ('qmake -query QT_INSTALL_PREFIX') do set QT_PATH=%%i
echo Qt Install Path: %QT_PATH%
echo.

REM Check for Qt WebEngine
echo Checking for Qt WebEngine...
echo.

set WEBENGINE_FOUND=0

if exist "%QT_PATH%\lib\cmake\Qt6WebEngine" (
    echo [FOUND] Qt6WebEngine CMake files
    dir /b "%QT_PATH%\lib\cmake\Qt6WebEngine"
    set WEBENGINE_FOUND=1
)

if exist "%QT_PATH%\bin\Qt6WebEngineCore.dll" (
    echo [FOUND] Qt6WebEngineCore.dll
    dir "%QT_PATH%\bin\Qt6WebEngineCore.dll"
    set WEBENGINE_FOUND=1
)

if exist "%QT_PATH%\include\QtWebEngine" (
    echo [FOUND] QtWebEngine headers
    set WEBENGINE_FOUND=1
)

echo.

if %WEBENGINE_FOUND%==1 (
    echo ============================================================
    echo SUCCESS: Qt WebEngine is installed!
    echo ============================================================
    echo.
    echo You can now build the Kindle DRM Converter.
    echo.
    echo Next steps:
    echo   1. Run: build.bat
    echo   2. Run: deploy-windows.bat
    echo.
    echo Or use the automated script:
    echo   start-here.bat
    echo.
) else (
    echo ============================================================
    echo ERROR: Qt WebEngine is NOT installed!
    echo ============================================================
    echo.
    echo Qt is installed, but Qt WebEngine component is missing.
    echo.
    echo To install Qt WebEngine:
    echo.
    echo METHOD 1: Using Qt Maintenance Tool
    echo   1. Run: %QT_PATH%\..\MaintenanceTool.exe
    echo   2. Select "Add or remove components"
    echo   3. Navigate to Qt ^> Additional Libraries
    echo   4. Check "Qt WebEngine"
    echo   5. Click "Update"
    echo.
    echo METHOD 2: Reinstall Qt
    echo   Download: https://www.qt.io/download-qt-installer
    echo   Make sure to check "Qt WebEngine" during installation
    echo.
    echo See INSTALL_QT_WEBENGINE.txt for detailed instructions.
    echo.
)

:end
echo ============================================================
echo.
pause

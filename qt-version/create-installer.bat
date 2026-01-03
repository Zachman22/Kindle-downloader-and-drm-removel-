@echo off
REM Kindle DRM Converter - Create Windows Installer
REM Requires NSIS (Nullsoft Scriptable Install System)
REM Download from: https://nsis.sourceforge.io/

echo ============================================================
echo Kindle DRM Converter - Create Windows Installer
echo ============================================================
echo.

REM Check if deployment exists
if not exist "KindleDRMConverter-Windows" (
    echo [ERROR] Deployment directory not found
    echo Please run deploy-windows.bat first
    pause
    exit /b 1
)

REM Check for NSIS
where makensis >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] NSIS not found
    echo.
    echo Please install NSIS from: https://nsis.sourceforge.io/
    echo.
    echo After installation, add NSIS to your PATH or run:
    echo   "C:\Program Files (x86)\NSIS\makensis.exe" installer.nsi
    echo.
    pause
    exit /b 1
)

echo [OK] NSIS found
makensis /VERSION
echo.

REM Build installer
echo Creating installer...
echo.
makensis installer.nsi

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ============================================================
    echo Installer Created Successfully!
    echo ============================================================
    echo.
    echo Installer file: KindleDRMConverter-Setup.exe
    echo.
    echo You can now distribute this installer to users.
    echo.
    echo Installer features:
    echo   - Installs application to Program Files
    echo   - Creates Start Menu shortcuts
    echo   - Creates Desktop shortcut
    echo   - Includes uninstaller
    echo   - Checks for Calibre
    echo.
    dir KindleDRMConverter-Setup.exe
) else (
    echo.
    echo [ERROR] Failed to create installer
    echo Check the output above for errors
)

echo.
pause

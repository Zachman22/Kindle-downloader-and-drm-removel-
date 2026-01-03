@echo off
REM Kindle DRM Converter - Windows Deployment Script
REM Creates a standalone EXE with all dependencies

echo ============================================================
echo Kindle DRM Converter - Windows Deployment Script
echo ============================================================
echo.

REM Check if build exists
if not exist "build-Release\bin\KindleDRMConverter.exe" (
    echo [ERROR] Build not found. Please run build.bat first.
    echo.
    echo Current directory: %CD%
    echo.
    echo Looking for: build-Release\bin\KindleDRMConverter.exe
    echo.
    pause
    exit /b 1
)

REM Create deployment directory
set DEPLOY_DIR=KindleDRMConverter-Windows
if exist %DEPLOY_DIR% rmdir /s /q %DEPLOY_DIR%
mkdir %DEPLOY_DIR%

echo [1/5] Copying executable...
copy build-Release\bin\KindleDRMConverter.exe %DEPLOY_DIR%\
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to copy executable
    exit /b 1
)
echo [OK] Executable copied

REM Run windeployqt to copy Qt dependencies
echo.
echo [2/5] Deploying Qt dependencies...
where windeployqt >nul 2>nul
if %ERRORLEVEL% EQU 0 (
    windeployqt --release --no-translations --no-system-d3d-compiler --no-opengl-sw %DEPLOY_DIR%\KindleDRMConverter.exe
    echo [OK] Qt dependencies deployed
) else (
    echo [WARNING] windeployqt not found - Qt DLLs must be copied manually
    echo Add Qt bin directory to PATH or copy required DLLs
)

REM Copy configuration and documentation
echo.
echo [3/5] Copying configuration files...
copy config.yaml.example %DEPLOY_DIR%\config.yaml.example
copy README_QT.md %DEPLOY_DIR%\README.md
if exist ..\README.md copy ..\README.md %DEPLOY_DIR%\README-Python.md
if exist ..\LICENSE copy ..\LICENSE %DEPLOY_DIR%\LICENSE.txt
echo [OK] Configuration files copied

REM Create directory structure
echo.
echo [4/5] Creating directory structure...
mkdir %DEPLOY_DIR%\downloads
mkdir %DEPLOY_DIR%\output
mkdir %DEPLOY_DIR%\temp
mkdir %DEPLOY_DIR%\logs
echo [OK] Directories created

REM Create batch file for easy running
echo.
echo [5/5] Creating launcher script...
(
echo @echo off
echo REM Kindle DRM Converter Launcher
echo.
echo if not exist config.yaml ^(
echo     echo Configuration file not found!
echo     echo Copying example config...
echo     copy config.yaml.example config.yaml
echo     echo.
echo     echo Please edit config.yaml with your Amazon credentials
echo     echo Then run this script again
echo     pause
echo     exit /b 1
echo ^)
echo.
echo KindleDRMConverter.exe %%*
) > %DEPLOY_DIR%\KindleDRMConverter.bat

echo [OK] Launcher created

REM Create README
echo.
echo Creating deployment README...
(
echo Kindle DRM Converter - Windows Standalone
echo ==========================================
echo.
echo This is a standalone Windows build with all dependencies included.
echo.
echo Quick Start:
echo 1. Edit config.yaml with your Amazon Kindle credentials
echo 2. Double-click KindleDRMConverter.bat
echo    OR
echo    Run from command line: KindleDRMConverter.exe --all
echo.
echo Requirements:
echo - Windows 10 or later
echo - Calibre ^(for ebook conversion^)
echo   Download from: https://calibre-ebook.com/download
echo.
echo Usage Examples:
echo   KindleDRMConverter.exe --all
echo   KindleDRMConverter.exe --download-only
echo   KindleDRMConverter.exe --input "C:\Books\Kindle"
echo   KindleDRMConverter.exe --formats epub,pdf,mobi
echo.
echo For full documentation, see README.md
echo.
echo Support:
echo - Report issues: https://github.com/Zachman22/Kindle-downloader-and-drm-removel-
echo.
) > %DEPLOY_DIR%\QUICKSTART.txt

echo.
echo ============================================================
echo Deployment Complete!
echo ============================================================
echo.
echo Standalone package created: %DEPLOY_DIR%\
echo.
echo Package contents:
dir /b %DEPLOY_DIR%
echo.
echo To distribute:
echo   1. Zip the %DEPLOY_DIR% folder
echo   2. Users just need to extract and run
echo.
echo To create installer:
echo   Run: create-installer.bat
echo.
pause

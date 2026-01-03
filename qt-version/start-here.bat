@echo off
echo.
echo ============================================================
echo Kindle DRM Converter - Windows Build Helper
echo ============================================================
echo.
echo This script will guide you through building the Windows EXE
echo.
echo ============================================================
echo.

echo STEP 1: Checking Prerequisites...
echo.

REM Check Qt
where qmake >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo [OK] Qt is installed
    qmake --version | findstr "Qt version"
) else (
    echo [ERROR] Qt is NOT installed!
    echo.
    echo Please install Qt from: https://www.qt.io/download-qt-installer
    echo.
    echo Make sure to install:
    echo   - Qt 6.x
    echo   - Qt WebEngine
    echo   - MinGW 64-bit
    echo.
    pause
    exit /b 1
)
echo.

REM Check CMake
where cmake >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo [OK] CMake is installed
    cmake --version | findstr "version"
) else (
    echo [ERROR] CMake is NOT installed!
    echo.
    echo Please install CMake from: https://cmake.org/download/
    echo.
    pause
    exit /b 1
)
echo.

REM Check Calibre
where ebook-convert >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo [OK] Calibre is installed
) else (
    echo [WARNING] Calibre is NOT installed
    echo.
    echo Calibre is needed for ebook conversion.
    echo Install from: https://calibre-ebook.com/download
    echo.
    echo You can continue, but conversion won't work without it.
    echo.
)
echo.

echo ============================================================
echo STEP 2: Building the Application
echo ============================================================
echo.

if exist "build-Release\bin\KindleDRMConverter.exe" (
    echo EXE already exists!
    echo.
    choice /C YN /M "Do you want to rebuild"
    if errorlevel 2 goto skip_build
)

echo Starting build... This may take 5-10 minutes.
echo.
call build.bat

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo [ERROR] Build failed!
    echo.
    echo Check the error messages above.
    echo.
    pause
    exit /b 1
)

:skip_build
echo.
echo ============================================================
echo STEP 3: Checking Build Result
echo ============================================================
echo.

if exist "build-Release\bin\KindleDRMConverter.exe" (
    echo [SUCCESS] Build completed!
    echo.
    dir "build-Release\bin\KindleDRMConverter.exe"
    echo.
) else (
    echo [ERROR] Build failed - EXE not found
    echo.
    pause
    exit /b 1
)

echo ============================================================
echo STEP 4: Creating Standalone Package
echo ============================================================
echo.

choice /C YN /M "Create standalone package with all Qt DLLs"
if errorlevel 2 goto skip_deploy

call deploy-windows.bat

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo [ERROR] Deployment failed!
    echo.
    pause
    exit /b 1
)

echo.
echo [SUCCESS] Standalone package created!
echo Location: KindleDRMConverter-Windows\
echo.

:skip_deploy
echo.
echo ============================================================
echo STEP 5: (Optional) Create Installer
echo ============================================================
echo.

where makensis >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo NSIS is not installed. Skipping installer creation.
    echo.
    echo To create an installer, install NSIS from:
    echo https://nsis.sourceforge.io/
    echo.
    goto done
)

choice /C YN /M "Create Windows installer (requires NSIS)"
if errorlevel 2 goto done

call create-installer.bat

:done
echo.
echo ============================================================
echo ALL DONE!
echo ============================================================
echo.
echo What you have now:
echo.

if exist "build-Release\bin\KindleDRMConverter.exe" (
    echo [X] KindleDRMConverter.exe ^(needs Qt DLLs^)
)

if exist "KindleDRMConverter-Windows" (
    echo [X] KindleDRMConverter-Windows\ folder ^(standalone, ready to distribute^)
)

if exist "KindleDRMConverter-Setup.exe" (
    echo [X] KindleDRMConverter-Setup.exe ^(installer^)
)

echo.
echo ============================================================
echo Next Steps:
echo ============================================================
echo.
echo 1. Copy config.yaml.example to config.yaml
echo 2. Edit config.yaml with your Amazon credentials
echo 3. Run the application!
echo.
echo To run:
echo   cd KindleDRMConverter-Windows
echo   KindleDRMConverter.exe --help
echo.
echo To distribute:
echo   - Zip the KindleDRMConverter-Windows folder, OR
echo   - Use KindleDRMConverter-Setup.exe installer
echo.

pause

@echo off
REM ONE-CLICK BUILD for Qt at C:\Qt
echo.
echo ============================================================
echo Kindle DRM Converter - ONE-CLICK BUILD
echo ============================================================
echo.
echo This script will:
echo   1. Find Qt at C:\Qt
echo   2. Set up environment
echo   3. Build the application
echo   4. Create standalone package
echo.
echo Press any key to start...
pause >nul
echo.

REM ============================================================
REM STEP 1: Find Qt
REM ============================================================

echo [STEP 1] Finding Qt installation...
echo.

set QT_VERSION=
for %%V in (6.7.2 6.7.1 6.7.0 6.6.0 6.5.0 6.4.0) do (
    if exist "C:\Qt\%%V\mingw_64" (
        set QT_VERSION=%%V
        goto qt_found
    )
)

echo [ERROR] Qt not found at C:\Qt
echo.
echo Please install Qt first or run: check-qt-at-c-drive.bat
echo.
pause
exit /b 1

:qt_found
echo [OK] Found Qt %QT_VERSION%
echo.

REM ============================================================
REM STEP 2: Check Qt WebEngine
REM ============================================================

echo [STEP 2] Checking Qt WebEngine...
echo.

if not exist "C:\Qt\%QT_VERSION%\mingw_64\lib\cmake\Qt6WebEngine" (
    echo [ERROR] Qt WebEngine NOT FOUND!
    echo.
    echo Qt is installed but Qt WebEngine component is missing.
    echo.
    echo To install Qt WebEngine:
    echo   1. Run: C:\Qt\MaintenanceTool.exe
    echo   2. Select "Add or remove components"
    echo   3. Check: Qt ^> %QT_VERSION% ^> Additional Libraries ^> Qt WebEngine
    echo   4. Click Update
    echo.
    choice /C YN /M "Do you want to open MaintenanceTool now"
    if errorlevel 1 if not errorlevel 2 start C:\Qt\MaintenanceTool.exe
    echo.
    echo After installing Qt WebEngine, run this script again.
    pause
    exit /b 1
)

echo [OK] Qt WebEngine found!
echo.

REM ============================================================
REM STEP 3: Set up environment
REM ============================================================

echo [STEP 3] Setting up build environment...
echo.

set PATH=%PATH%;C:\Qt\%QT_VERSION%\mingw_64\bin
set PATH=%PATH%;C:\Qt\Tools\mingw1120_64\bin
set PATH=%PATH%;C:\Qt\Tools\CMake_64\bin
set CMAKE_PREFIX_PATH=C:\Qt\%QT_VERSION%\mingw_64

echo Environment configured for Qt %QT_VERSION%
echo.

REM ============================================================
REM STEP 4: Build
REM ============================================================

echo [STEP 4] Building application...
echo.
echo This may take 5-10 minutes on first build.
echo Please wait...
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

echo.
echo [SUCCESS] Build completed!
echo.

REM ============================================================
REM STEP 5: Verify
REM ============================================================

echo [STEP 5] Verifying build...
echo.

if exist "build-Release\bin\KindleDRMConverter.exe" (
    echo [OK] KindleDRMConverter.exe created successfully!
    dir "build-Release\bin\KindleDRMConverter.exe"
    echo.
) else (
    echo [ERROR] EXE not found!
    pause
    exit /b 1
)

REM ============================================================
REM STEP 6: Deploy
REM ============================================================

echo [STEP 6] Creating standalone package...
echo.

call deploy-windows.bat

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo [WARNING] Deployment encountered issues.
    echo.
    echo You can still use the EXE, but you may need to copy Qt DLLs manually.
    echo.
    pause
) else (
    echo.
    echo [SUCCESS] Standalone package created!
    echo.
)

REM ============================================================
REM DONE!
REM ============================================================

echo.
echo ============================================================
echo BUILD COMPLETE!
echo ============================================================
echo.
echo Standalone application location:
echo   KindleDRMConverter-Windows\
echo.
echo What to do next:
echo   1. Go to: KindleDRMConverter-Windows\
echo   2. Copy: config.yaml.example to config.yaml
echo   3. Edit config.yaml with your Amazon credentials
echo   4. Run: KindleDRMConverter.exe --all
echo.
echo ============================================================
echo.

choice /C YN /M "Do you want to open the output folder now"
if errorlevel 1 if not errorlevel 2 (
    if exist "KindleDRMConverter-Windows" (
        explorer KindleDRMConverter-Windows
    )
)

echo.
echo Press any key to exit...
pause >nul

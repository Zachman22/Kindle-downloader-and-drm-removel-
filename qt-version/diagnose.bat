@echo off
setlocal enabledelayedexpansion

echo.
echo ============================================================
echo DIAGNOSTIC: Checking Build Status
echo ============================================================
echo.

echo [1] Current Location:
echo    %CD%
echo.

echo [2] Checking for build directories...
if exist "build-Release" (
    echo    [FOUND] build-Release directory exists
) else (
    echo    [ERROR] build-Release directory NOT FOUND
    echo    --- You need to run build.bat first! ---
)
echo.

if exist "build-Release\bin" (
    echo    [FOUND] build-Release\bin directory exists
) else (
    echo    [ERROR] build-Release\bin directory NOT FOUND
)
echo.

echo [3] Checking for KindleDRMConverter.exe...
if exist "build-Release\bin\KindleDRMConverter.exe" (
    echo    [SUCCESS] KindleDRMConverter.exe FOUND!
    echo    Location: build-Release\bin\KindleDRMConverter.exe
    dir "build-Release\bin\KindleDRMConverter.exe"
) else (
    echo    [ERROR] KindleDRMConverter.exe NOT FOUND
    echo.
    echo    THIS IS WHY deploy-windows.bat IS FAILING!
    echo.
)
echo.

echo [4] What's in the current directory:
dir /b
echo.

echo ============================================================
echo SOLUTION:
echo ============================================================
echo.

if not exist "build-Release\bin\KindleDRMConverter.exe" (
    echo The EXE does not exist yet. You need to:
    echo.
    echo   1. Make sure Qt is installed ^(with WebEngine^)
    echo   2. Run: build.bat
    echo   3. Wait for build to complete
    echo   4. Then run: deploy-windows.bat
    echo.
    echo To check if Qt is installed, run:
    echo   qmake --version
    echo   cmake --version
    echo.
) else (
    echo Everything looks good! You can now run:
    echo   deploy-windows.bat
    echo.
)

echo ============================================================
echo.
echo Press any key to close this window...
pause >nul

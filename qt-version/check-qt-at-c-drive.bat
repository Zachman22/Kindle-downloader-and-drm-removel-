@echo off
echo.
echo ============================================================
echo Checking Qt WebEngine at C:\Qt
echo ============================================================
echo.

REM Check if C:\Qt exists
if not exist "C:\Qt" (
    echo [ERROR] C:\Qt directory not found!
    echo.
    pause
    exit /b 1
)

echo [OK] C:\Qt directory found
echo.

REM Find Qt versions
echo Scanning for Qt versions...
echo.

set FOUND_VERSION=
set WEBENGINE_FOUND=0

REM Check common Qt 6 paths
for %%V in (6.7.0 6.7.1 6.7.2 6.6.0 6.5.0 6.4.0) do (
    if exist "C:\Qt\%%V" (
        echo [FOUND] Qt %%V installed
        set FOUND_VERSION=%%V

        REM Check for MinGW build
        if exist "C:\Qt\%%V\mingw_64" (
            echo    - MinGW 64-bit build found

            REM Check for Qt WebEngine
            if exist "C:\Qt\%%V\mingw_64\lib\cmake\Qt6WebEngine" (
                echo    - [OK] Qt WebEngine: INSTALLED
                set WEBENGINE_FOUND=1
            ) else (
                echo    - [MISSING] Qt WebEngine: NOT INSTALLED
            )

            if exist "C:\Qt\%%V\mingw_64\bin\Qt6WebEngineCore.dll" (
                echo    - [OK] Qt6WebEngineCore.dll found
            ) else (
                echo    - [MISSING] Qt6WebEngineCore.dll not found
            )
        )

        REM Check for MSVC build
        if exist "C:\Qt\%%V\msvc2019_64" (
            echo    - MSVC 2019 64-bit build found

            if exist "C:\Qt\%%V\msvc2019_64\lib\cmake\Qt6WebEngine" (
                echo    - [OK] Qt WebEngine: INSTALLED
                set WEBENGINE_FOUND=1
            ) else (
                echo    - [MISSING] Qt WebEngine: NOT INSTALLED
            )
        )

        echo.
    )
)

REM List all directories in C:\Qt
echo All directories in C:\Qt:
dir /b /ad C:\Qt
echo.

echo ============================================================
echo RESULT
echo ============================================================
echo.

if %WEBENGINE_FOUND%==1 (
    echo [SUCCESS] Qt WebEngine is INSTALLED!
    echo.
    echo You can now build the Kindle DRM Converter.
    echo.
    echo Next steps:
    echo   1. Set PATH to include Qt:
    echo      set PATH=%%PATH%%;C:\Qt\%FOUND_VERSION%\mingw_64\bin
    echo      set PATH=%%PATH%%;C:\Qt\Tools\mingw1120_64\bin
    echo      set PATH=%%PATH%%;C:\Qt\Tools\CMake_64\bin
    echo.
    echo   2. Navigate to project folder
    echo   3. Run: start-here.bat
    echo.
) else (
    echo [ERROR] Qt WebEngine is NOT INSTALLED!
    echo.
    echo How to install Qt WebEngine:
    echo.
    echo METHOD 1: Using Qt Maintenance Tool
    echo   1. Run: C:\Qt\MaintenanceTool.exe
    echo   2. Click "Add or remove components"
    echo   3. Sign in to your Qt account
    echo   4. Navigate to: Qt ^> [your version] ^> Additional Libraries
    echo   5. CHECK "Qt WebEngine"
    echo   6. Click "Next" then "Update"
    echo.
    echo METHOD 2: Download and install new Qt version
    echo   1. Open DOWNLOAD-LINKS.html in your browser
    echo   2. Download Qt Online Installer
    echo   3. Make sure to CHECK "Qt WebEngine" during installation
    echo.
    echo For detailed instructions, see: INSTALL_QT_WEBENGINE.txt
    echo.
)

echo ============================================================
echo.

REM Check if MaintenanceTool exists
if exist "C:\Qt\MaintenanceTool.exe" (
    echo.
    choice /C YN /M "Do you want to open Qt Maintenance Tool now"
    if errorlevel 1 if not errorlevel 2 (
        start C:\Qt\MaintenanceTool.exe
    )
)

pause

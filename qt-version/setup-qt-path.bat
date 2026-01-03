@echo off
REM Automatically set Qt PATH for C:\Qt installation

echo Setting up Qt environment...
echo.

REM Find Qt version in C:\Qt
set QT_VERSION=
for %%V in (6.7.2 6.7.1 6.7.0 6.6.0 6.5.0 6.4.0) do (
    if exist "C:\Qt\%%V\mingw_64" (
        set QT_VERSION=%%V
        goto found
    )
)

:found
if "%QT_VERSION%"=="" (
    echo [ERROR] Qt not found at C:\Qt
    echo.
    echo Please install Qt or specify Qt path manually.
    pause
    exit /b 1
)

echo [OK] Found Qt %QT_VERSION% at C:\Qt
echo.

REM Set PATH
set PATH=%PATH%;C:\Qt\%QT_VERSION%\mingw_64\bin
set PATH=%PATH%;C:\Qt\Tools\mingw1120_64\bin
set PATH=%PATH%;C:\Qt\Tools\CMake_64\bin
set CMAKE_PREFIX_PATH=C:\Qt\%QT_VERSION%\mingw_64

echo Qt PATH configured:
echo   Qt binaries: C:\Qt\%QT_VERSION%\mingw_64\bin
echo   MinGW: C:\Qt\Tools\mingw1120_64\bin
echo   CMake: C:\Qt\Tools\CMake_64\bin
echo.

REM Verify
qmake --version
echo.

echo ============================================================
echo Qt environment is ready!
echo ============================================================
echo.
echo You can now run: build.bat
echo.

@echo off
cd /d "%~dp0"

echo Installing Liquid Glass Demo dependencies...
echo.

:: Check if Git is installed
where git >nul 2>&1
if errorlevel 1 (
    echo ERROR: Git not found. Please install Git first.
    echo Download: https://git-scm.com/download/win
    pause
    exit /b 1
)

:: Install vcpkg if not exists
if not exist "vcpkg" (
    echo Downloading vcpkg...
    git clone https://github.com/Microsoft/vcpkg.git
    if errorlevel 1 (
        echo ERROR: Failed to download vcpkg
        pause
        exit /b 1
    )
)

cd vcpkg
if not exist vcpkg.exe (
    echo Building vcpkg...
    call bootstrap-vcpkg.bat
)

echo Installing libraries...
call vcpkg install glfw3:x64-windows
call vcpkg install glew:x64-windows
call vcpkg install glm:x64-windows
call vcpkg integrate install

cd ..
echo.
echo Installation complete!
echo.
echo Next steps:
echo 1. Double-click LiquidGlassDemo.sln
echo 2. Press F7 to build
echo 3. Press F5 to run
echo.
pause
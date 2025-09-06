@echo off
echo Building LiquidGlassDemo...

if not exist "build" mkdir build
cd build

cmake .. -G "Visual Studio 17 2022"
if %errorlevel% neq 0 (
    echo CMake configuration failed!
    pause
    exit /b 1
)

cmake --build . --config Release
if %errorlevel% neq 0 (
    echo Build failed!
    pause
    exit /b 1
)

echo Build completed successfully!
pause
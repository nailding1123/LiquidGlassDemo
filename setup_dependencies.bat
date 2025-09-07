@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

echo ======================================
echo Liquid Glass Demo 依赖库安装脚本
echo ======================================
echo.

echo 正在检查系统环境...

:: 检查管理员权限
net session >nul 2>&1
if "%errorLevel%"=="0" (
    echo 已获取管理员权限
) else (
    echo 提示：建议以管理员身份运行此脚本以获得最佳效果
)

:: 检查Git
where git >nul 2>&1
if "%errorLevel%"=="0" (
    echo Git 已安装
) else (
    echo 错误：未找到Git，请先安装Git
    echo 下载地址：https://git-scm.com/download/win
echo.
    pause
    exit /b 1
)

:: 设置vcpkg路径
set "VCPKG_ROOT=%CD%\vcpkg"

echo.
echo 正在设置vcpkg...

:: 检查是否安装了vcpkg
if not exist "%VCPKG_ROOT%" (
    echo 正在下载vcpkg...
    git clone https://github.com/Microsoft/vcpkg.git "%VCPKG_ROOT%"
    if not "%errorLevel%"=="0" (
        echo 错误：vcpkg下载失败
        pause
        exit /b 1
    )
) else (
    echo vcpkg已存在，正在更新...
    cd "%VCPKG_ROOT%"
    git pull
    cd ..
)

echo.
echo 正在初始化vcpkg...
cd "%VCPKG_ROOT%"
if not exist "vcpkg.exe" (
    call bootstrap-vcpkg.bat
    if not "%errorLevel%"=="0" (
        echo 错误：vcpkg初始化失败
        pause
        exit /b 1
    )
)

echo.
echo 正在集成vcpkg到Visual Studio...
call vcpkg integrate install
if not "%errorLevel%"=="0" (
    echo 警告：vcpkg集成失败，可能需要手动配置
)

echo.
echo 正在安装依赖库...
echo 这可能需要几分钟时间...

:: 安装GLFW3
echo.
echo [1/3] 正在安装GLFW3...
call vcpkg install glfw3:x64-windows
if not "%errorLevel%"=="0" (
    echo 警告：GLFW3安装可能遇到问题
)

:: 安装GLEW
echo.
echo [2/3] 正在安装GLEW...
call vcpkg install glew:x64-windows
if not "%errorLevel%"=="0" (
    echo 警告：GLEW安装可能遇到问题
)

:: 安装GLM
echo.
echo [3/3] 正在安装GLM...
call vcpkg install glm:x64-windows
if not "%errorLevel%"=="0" (
    echo 警告：GLM安装可能遇到问题
)

cd ..

echo.
echo ======================================
echo 依赖库安装完成！
echo ======================================
echo.
echo 下一步：
echo 1. 双击打开 LiquidGlassDemo.sln
echo 2. 选择 Release x64 配置
echo 3. 按 F7 构建项目
echo 4. 按 F5 运行程序
echo.
echo 背景图片：
echo 将任意图片重命名为 background.png 放入 backgrounds/ 目录
echo.
pause
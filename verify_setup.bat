@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

echo ======================================
echo Liquid Glass Demo 环境验证脚本
echo ======================================
echo.

echo 正在检查系统环境...

:: 检查Visual Studio
where devenv >nul 2>&1
if "%errorLevel%"=="0" (
    echo Visual Studio 已安装
) else (
    echo 未找到Visual Studio，请安装Visual Studio 2022
)

:: 检查Git
where git >nul 2>&1
if "%errorLevel%"=="0" (
    echo Git 已安装
) else (
    echo 未找到Git，请安装Git
)

:: 检查vcpkg
if exist "vcpkg\vcpkg.exe" (
    echo vcpkg 已安装
    
    :: 检查已安装的包
    vcpkg list | findstr "glfw3" >nul
    if "%errorLevel%"=="0" (
        echo GLFW3 已安装
    ) else (
        echo GLFW3 未安装
    )
    
    vcpkg list | findstr "glew" >nul
    if "%errorLevel%"=="0" (
        echo GLEW 已安装
    ) else (
        echo GLEW 未安装
    )
    
    vcpkg list | findstr "glm" >nul
    if "%errorLevel%"=="0" (
        echo GLM 已安装
    ) else (
        echo GLM 未安装
    )
) else (
    echo vcpkg 未安装，请运行 setup_dependencies.bat
)

echo.
echo 建议：
echo 1. 如果所有检查都通过，双击 LiquidGlassDemo.sln 打开项目
echo 2. 选择 Release x64 配置
echo 3. 按 F7 构建项目
echo.
pause
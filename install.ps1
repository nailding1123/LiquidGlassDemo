# Liquid Glass Demo 依赖安装脚本 (PowerShell)
Write-Host "======================================" -ForegroundColor Green
Write-Host "Liquid Glass Demo 依赖库安装脚本" -ForegroundColor Green
Write-Host "======================================" -ForegroundColor Green
Write-Host

# 检查管理员权限
$isAdmin = ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] "Administrator")
if ($isAdmin) {
    Write-Host "已获取管理员权限" -ForegroundColor Green
} else {
    Write-Host "提示：建议以管理员身份运行以获得最佳效果" -ForegroundColor Yellow
}

# 检查Git
if (!(Get-Command git -ErrorAction SilentlyContinue)) {
    Write-Host "错误：未找到Git，请先安装Git" -ForegroundColor Red
    Write-Host "下载地址：https://git-scm.com/download/win" -ForegroundColor Yellow
    pause
    exit 1
}
Write-Host "Git 已安装" -ForegroundColor Green

# 设置vcpkg路径
$vcpkgPath = Join-Path $PSScriptRoot "vcpkg"

# 安装vcpkg
if (!(Test-Path $vcpkgPath)) {
    Write-Host "正在下载vcpkg..." -ForegroundColor Yellow
    git clone https://github.com/Microsoft/vcpkg.git $vcpkgPath
    if ($LASTEXITCODE -ne 0) {
        Write-Host "错误：vcpkg下载失败" -ForegroundColor Red
        pause
        exit 1
    }
} else {
    Write-Host "vcpkg已存在，正在更新..." -ForegroundColor Yellow
    Set-Location $vcpkgPath
    git pull
    Set-Location $PSScriptRoot
}

# 初始化vcpkg
Set-Location $vcpkgPath
if (!(Test-Path "vcpkg.exe")) {
    Write-Host "正在构建vcpkg..." -ForegroundColor Yellow
    .\bootstrap-vcpkg.bat
    if ($LASTEXITCODE -ne 0) {
        Write-Host "错误：vcpkg构建失败" -ForegroundColor Red
        pause
        exit 1
    }
}

# 集成到Visual Studio
Write-Host "正在集成vcpkg到Visual Studio..." -ForegroundColor Yellow
.\vcpkg integrate install

# 安装依赖库
Write-Host "正在安装依赖库..." -ForegroundColor Yellow
$libraries = @("glfw3:x64-windows", "glew:x64-windows", "glm:x64-windows")
foreach ($lib in $libraries) {
    Write-Host "安装 $lib..." -ForegroundColor Cyan
    .\vcpkg install $lib
}

Set-Location $PSScriptRoot

Write-Host ""
Write-Host "======================================" -ForegroundColor Green
Write-Host "依赖库安装完成！" -ForegroundColor Green
Write-Host "======================================" -ForegroundColor Green
Write-Host ""
Write-Host "下一步：" -ForegroundColor Green
Write-Host "1. 双击打开 LiquidGlassDemo.sln" -ForegroundColor White
Write-Host "2. 选择 Release x64 配置" -ForegroundColor White
Write-Host "3. 按 F7 构建项目" -ForegroundColor White
Write-Host "4. 按 F5 运行程序" -ForegroundColor White
Write-Host ""
Write-Host "如果遇到问题，请参考MANUAL_INSTALL.md" -ForegroundColor Yellow
pause
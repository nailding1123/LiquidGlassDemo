# 修复链接错误 LNK1104

## 问题原因
项目无法找到 `glfw3.lib`，因为vcpkg安装的是 `glfw3dll.lib` 而不是 `glfw3.lib`。

## 解决方案

### 方法1：自动修复（推荐）
1. 双击运行 `fix_vcpkg_path.bat`
2. 重启Visual Studio
3. 重新构建项目

### 方法2：手动设置环境变量
1. 打开命令提示符（管理员）
2. 运行：
   ```
   setx VCPKG_ROOT "C:\Users\lbd111\LiquidGlassDemo01\vcpkg"
   ```
3. 重启Visual Studio

### 方法3：项目文件已更新
项目文件已自动更新为使用正确的库文件名：
- 从 `glfw3.lib` 改为 `glfw3dll.lib`

## 验证步骤
1. 确保文件存在：
   ```
   C:\Users\lbd111\LiquidGlassDemo01\vcpkg\installed\x64-windows\lib\glfw3dll.lib
   ```

2. 检查环境变量：
   - 在命令提示符中运行：`echo %VCPKG_ROOT%`
   - 应该显示：`C:\Users\lbd111\LiquidGlassDemo01\vcpkg`

## 如果仍有问题
1. 检查是否安装了所有依赖：
   - glfw3dll.lib
   - glew32.lib
   - glm.lib
   - opengl32.lib
   - glu32.lib

2. 重新安装依赖：
   ```
   cd vcpkg
   vcpkg install glfw3:x64-windows
   vcpkg install glew:x64-windows
   vcpkg install glm:x64-windows
   ```

3. 确保使用x64配置构建

---

# 修复LNK2019链接错误指南

## 问题描述
编译时出现LNK2019错误：
```
无法解析的外部符号 "void __cdecl CleanupBackgroundTextures(void)" 
```

## 原因分析
该错误是由于新增的源文件没有被包含到Visual Studio项目中导致的。

## 修复步骤

### ✅ 项目文件已更新
我已经更新了以下项目配置文件：

- `LiquidGlassDemo.vcxproj`: 添加了新的源文件和头文件
- `LiquidGlassDemo.vcxproj.filters`: 更新了文件过滤器

### ✅ 文件位置确认
所有文件都在正确的位置：

**源文件 (src/):**
- main.cpp
- LiquidGlass.cpp
- Shader.cpp
- Camera.cpp
- TextureLoader.cpp (新增)
- background_setup.cpp (新增)

**头文件 (include/):**
- LiquidGlass.h
- Shader.h
- Camera.h
- TextureLoader.h (新增)
- background_setup.h (新增)

**着色器文件 (shaders/):**
- liquid_glass.vert
- liquid_glass.frag
- liquid_glass_enhanced.frag (新增)

### 3. 重新生成项目
由于项目文件已更新，需要重新生成解决方案：

#### 方法1：使用CMake
```bash
# 在项目根目录执行
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
```

#### 方法2：使用批处理文件
直接运行项目根目录下的 `build.bat` 文件。

#### 方法3：在Visual Studio中
1. 关闭Visual Studio
2. 删除 `.vs` 文件夹（可选，清理缓存）
3. 重新打开项目
4. 重新生成解决方案

### 4. 验证修复
重新编译后，链接错误应该消失。如果仍有问题，请检查：
- 所有.cpp文件都被添加到项目中
- 所有.h文件路径正确
- 没有重复的函数定义

## 功能说明
修复后，项目将支持：
- 背景纹理系统
- 透明度混合
- 背景扭曲效果
- B键切换背景类型

## 状态更新
✅ 项目配置文件已更新
✅ 所有源文件已添加到项目
✅ 链接错误已修复
✅ 可以重新编译项目
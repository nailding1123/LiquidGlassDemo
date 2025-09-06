# 液态玻璃项目构建和运行指南

## 修复的编译错误

### 已修复的问题
1. **LiquidGlass.h重复定义** - 移除了文件末尾的重复结构体定义
2. **头文件包含** - 确保所有必要的OpenGL和GLM头文件正确包含
3. **CMakeLists.txt格式** - 修复了文件末尾的格式错误

## 构建步骤

### Windows (Visual Studio)
```bash
# 1. 创建构建目录
mkdir build
cd build

# 2. 使用CMake生成项目
cmake .. -G "Visual Studio 16 2019" -A x64

# 3. 构建项目
cmake --build . --config Release
```

### Windows (MinGW)
```bash
# 1. 创建构建目录
mkdir build
cd build

# 2. 使用CMake生成项目
cmake .. -G "MinGW Makefiles"

# 3. 构建项目
cmake --build . --config Release
```

### 运行程序
```bash
# 从构建目录运行
./LiquidGlassDemo

# 或从项目根目录运行
./build/Release/LiquidGlassDemo.exe
```

## Android技术实现集成

### 1. 切换到增强版本
修改 `src/LiquidGlass.cpp` 中的 LoadShaders 方法：
```cpp
void LiquidGlass::LoadShaders() {
    // 使用Android技术实现
    Shader shader("shaders/liquid_glass.vert", "shaders/android_liquid_glass.frag");
    m_shaderProgram = shader.ID;
}
```

### 2. 添加新参数支持
在 `Render` 方法中添加：
```cpp
// 添加Android技术实现的新参数
GLint blurRadiusLoc = glGetUniformLocation(m_shaderProgram, "blurRadius");
glUniform1f(blurRadiusLoc, 0.01f);
```

## 参数调优

### 运行时控制
- **F1**: 显示帮助信息
- **空格**: 切换Android/原始实现
- **数字键1-5**: 调整不同参数
- **鼠标**: 旋转视角
- **滚轮**: 缩放

### 推荐参数设置
```cpp
// Android技术实现优化参数
float distortion = 1.2f;        // 扭曲强度
float blurRadius = 0.015f;      // 背景模糊
float edgeBrightness = 2.5f;    // 边缘亮度
float fresnelPower = 3.5f;      // 菲涅尔效果
float thickness = 0.08f;        // 玻璃厚度
```

## 性能优化

### 高质量设置
- 使用5x5高斯核
- 启用3层液态波纹
- 开启边缘彩虹效果

### 性能模式设置
- 使用3x3高斯核
- 减少到2层波纹
- 关闭彩虹边缘效果

## 文件结构

### 新增文件
- `shaders/android_liquid_glass.frag` - Android技术实现着色器
- `docs/android_implementation_guide.md` - 详细实现指南
- `docs/liquid_glass_tutorial.md` - 技术学习笔记
- `BUILD_AND_RUN.md` - 本构建指南

### 修复的文件
- `include/LiquidGlass.h` - 修复重复定义
- `src/LiquidGlass.cpp` - 添加必要头文件
- `CMakeLists.txt` - 修复格式错误

## 故障排除

### 常见编译错误
1. **找不到GL/glew.h**
   - 确保已安装GLEW库
   - 检查vcpkg或系统路径

2. **glm相关错误**
   - 确保GLM库已正确安装
   - 检查CMake配置

3. **着色器编译错误**
   - 检查shaders目录是否存在
   - 确保文件路径正确

### 运行时问题
1. **黑屏**
   - 检查背景纹理是否正确加载
   - 验证着色器编译状态

2. **性能问题**
   - 降低分辨率
   - 减少模糊半径
   - 关闭复杂效果

## 验证构建

构建成功后，运行程序应该看到：
- 一个旋转的透明玻璃球
- 背景扭曲效果
- 边缘光散射
- 可调节的参数

使用Android技术实现时，将看到：
- 更精确的边缘检测
- 高斯模糊背景
- 彩虹色边缘效果
- 多层液态扭曲
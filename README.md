# Liquid Glass Demo - OpenGL液态玻璃演示

这是一个使用OpenGL实现的液态玻璃效果演示项目，可在Visual Studio中运行。

## 功能特性

- 实时液态玻璃渲染
- 动态扭曲效果
- Fresnel反射和折射
- 交互式相机控制
- 可调节的失真程度

## 系统要求

- Windows 10/11
- Visual Studio 2022
- OpenGL 3.3+
- GLFW3库
- GLEW库
- GLM数学库

## 安装步骤

### 1. 安装依赖库

#### 方法1：使用vcpkg（推荐）
```bash
# 安装vcpkg（如果尚未安装）
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.bat
./vcpkg integrate install

# 安装依赖
vcpkg install glew:x64-windows
vcpkg install glfw3:x64-windows
vcpkg install glm:x64-windows
```

#### 方法2：手动下载
1. 下载GLFW3（https://www.glfw.org/download.html）
2. 下载GLEW（http://glew.sourceforge.net/）
3. 下载GLM（https://github.com/g-truc/glm/releases）
4. 将头文件和库文件放入项目对应的include和lib目录

### 2. 配置Visual Studio

1. 双击 `LiquidGlassDemo.sln` 打开解决方案
2. 右键项目 -> 属性
3. 配置包含目录和库目录：
   - 包含目录：添加依赖库的include路径
   - 库目录：添加依赖库的lib路径

### 3. 构建和运行

1. 选择Release x64配置
2. 按F7构建项目
3. 按F5运行程序

## 使用说明

### 控制方式
- **WASD键**：移动相机
- **鼠标移动**：旋转视角
- **鼠标滚轮**：缩放
- **上下箭头键**：调整扭曲程度
- **ESC键**：退出程序

### 技术特点

- **实时渲染**：60+ FPS的实时液态效果
- **物理模拟**：基于波动方程的液体表面模拟
- **光照模型**：包含反射、折射和散射的完整光照计算
- **透明度处理**：正确的alpha混合和排序

## 项目结构

```
LiquidGlassDemo01/
├── LiquidGlassDemo.sln          # Visual Studio解决方案
├── LiquidGlassDemo.vcxproj      # Visual Studio项目文件
├── README.md                    # 项目说明文档
├── include/                     # 头文件目录
│   ├── Camera.h                # 相机控制类
│   ├── LiquidGlass.h           # 液态玻璃渲染类
│   └── Shader.h                # 着色器管理类
├── src/                        # 源代码目录
│   ├── main.cpp               # 主程序入口
│   ├── Camera.cpp             # 相机控制实现
│   ├── LiquidGlass.cpp        # 液态玻璃渲染实现
│   └── Shader.cpp             # 着色器管理实现
├── shaders/                    # 着色器文件目录
│   ├── liquid_glass.vert      # 顶点着色器
│   └── liquid_glass.frag      # 片段着色器
└── lib/                       # 第三方库目录
```

## 性能优化

- 使用索引缓冲区减少顶点数据
- 实现视锥体剔除
- 使用实例化渲染（可扩展）
- 优化着色器计算

## 扩展功能

- 添加环境贴图反射
- 实现焦散效果
- 支持多种液体类型
- 添加粒子系统
- 实现流体动力学模拟

## 故障排除

### 常见问题

1. **编译错误：找不到头文件**
   - 确保依赖库已正确安装
   - 检查项目包含目录设置

2. **运行时错误：缺少DLL**
   - 将DLL文件复制到可执行文件目录
   - 或添加到系统PATH环境变量

3. **OpenGL版本不支持**
   - 更新显卡驱动程序
   - 检查显卡支持的OpenGL版本

## 许可证

MIT License - 可自由使用和修改

## 作者

OpenGL液态玻璃演示项目
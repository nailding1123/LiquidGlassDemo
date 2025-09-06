# Android液态玻璃技术实现指南

## 概述

基于 https://lrdcq.com/me/read.php/166.htm 的技术实现，我们将Android平台的液态玻璃效果技术应用到当前OpenGL项目中。

## 核心技术对比

### Android实现 vs OpenGL实现

| 技术方面 | Android实现 | 当前OpenGL实现 | 改进点 |
|---------|-------------|----------------|---------|
| **内容捕获** | Bitmap + Canvas | 纹理采样 | 添加高斯模糊背景 |
| **SDF计算** | 8ssedt/JFA算法 | 距离场计算 | 更精确的边缘检测 |
| **背景处理** | 父级渲染捕获 | 直接纹理读取 | 支持动态模糊 |
| **渲染管线** | Runtime Shader | 片元着色器 | 统一效果框架 |

## 新增功能特性

### 1. 增强的SDF计算
- **边缘检测**: 基于距离场的精确边缘计算
- **动态厚度**: 根据视角调整玻璃厚度
- **实时更新**: 支持动画和交互

### 2. 高斯背景模糊
- **可配置模糊半径**: 支持0.001-0.1范围的模糊强度
- **性能优化**: 5x5高斯核，GPU并行计算
- **边缘过渡**: 平滑的清晰到模糊过渡

### 3. 液态扭曲增强
- **多层波纹**: 3个不同频率的波纹叠加
- **时间动画**: 基于sin/cos的动态效果
- **边缘增强**: 边缘处的扭曲强度自动调整

### 4. 彩虹边缘效果
- **角度依赖**: 基于视角和表面法线的彩虹色
- **动态变化**: 随时间变化的彩虹色调
- **物理正确**: 基于菲涅尔方程的光学效果

## 使用方法

### 切换到Android技术实现

1. **修改着色器加载**：
```cpp
// 在 LiquidGlass::LoadShaders() 中修改
fragmentShader = LoadShader("shaders/android_liquid_glass.frag");
```

2. **添加新uniform变量**：
```cpp
// 在 LiquidGlass::Render() 中添加
GLint blurRadiusLoc = glGetUniformLocation(shaderProgram, "blurRadius");
glUniform1f(blurRadiusLoc, 0.01f);
```

3. **启用增强效果**：
```cpp
// 新参数控制
float blurRadius = 0.01f;  // 背景模糊半径
float edgeWidth = 0.05f;   // 边缘宽度
```

### 参数调优建议

| 参数 | 范围 | 推荐值 | 效果描述 |
|------|------|--------|----------|
| `distortion` | 0.0-2.0 | 1.0 | 液态扭曲强度 |
| `blurRadius` | 0.001-0.1 | 0.01 | 背景模糊程度 |
| `edgeBrightness` | 0.5-5.0 | 2.0 | 边缘光亮度 |
| `fresnelPower` | 1.0-8.0 | 3.0 | 菲涅尔效果强度 |
| `thickness` | 0.05-0.3 | 0.1 | 玻璃厚度影响 |

## 性能优化

### GPU优化策略
- **采样优化**: 使用5x5高斯核而非更大尺寸
- **条件渲染**: 透明区域early return
- **纹理缓存**: 充分利用GPU纹理缓存

### 质量设置
```glsl
// 高质量模式（默认）
#define GAUSSIAN_KERNEL_SIZE 5
#define LIQUID_LAYERS 3

// 中等质量模式
#define GAUSSIAN_KERNEL_SIZE 3
#define LIQUID_LAYERS 2

// 性能模式
#define GAUSSIAN_KERNEL_SIZE 3
#define LIQUID_LAYERS 1
```

## 调试工具

### 可视化调试
- **SDF可视化**: 显示距离场计算结果
- **边缘检测**: 高亮显示检测到的边缘
- **法线可视化**: 显示表面法线方向

### 性能监控
```cpp
// 在渲染循环中添加性能统计
static float frameTime = 0.0f;
static int frameCount = 0;
frameTime += deltaTime;
frameCount++;
if (frameTime >= 1.0f) {
    std::cout << "FPS: " << frameCount << std::endl;
    frameTime = 0.0f;
    frameCount = 0;
}
```

## 扩展应用

### 1. 交互式变形
- 鼠标悬停时局部扭曲增强
- 点击时产生涟漪效果
- 拖拽时的实时变形

### 2. 物理模拟
- 重力影响下的液态流动
- 碰撞检测和响应
- 表面张力模拟

### 3. 材质系统
- 不同折射率的材质
- 彩色玻璃效果
- 磨砂玻璃纹理

## 常见问题解答

### Q: 为什么背景看起来不够真实？
A: 尝试调整`blurRadius`参数，并确保背景纹理分辨率足够高。

### Q: 性能下降明显怎么办？
A: 降低高斯核大小，或减少液态波纹层数。

### Q: 边缘出现锯齿？
A: 增加`edgeWidth`参数值，或启用多重采样抗锯齿。

## 下一步计划

1. **添加SDF纹理支持**: 实现离线SDF计算
2. **支持多玻璃对象**: 批量渲染优化
3. **移动端适配**: 针对手机GPU优化
4. **VR/AR支持**: 立体渲染适配
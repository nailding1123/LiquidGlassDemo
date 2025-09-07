# Shaders说明文档

## 📁 当前文件结构
```
shaders/
├── liquid_glass.vert      # 顶点着色器（必需）
├── liquid_glass.frag      # 平面玻璃片段着色器（必需）
└── README.md             # 本说明文档
```

## 🎯 文件用途说明

### liquid_glass.vert
- **作用**：处理顶点变换、法线计算、纹理坐标传递
- **功能**：
  - 将3D顶点坐标变换到屏幕空间
  - 计算法线方向用于光照
  - 传递纹理坐标给片段着色器

### liquid_glass.frag
- **作用**：实现平面玻璃片的视觉效果
- **功能特性**：
  - ✅ **平面矩形玻璃片**（替代旧版球状效果）
  - ✅ **背景图片扭曲放大**（基于距离场的边缘扭曲）
  - ✅ **边缘高光效果**（圆角矩形边缘）
  - ✅ **可移动玻璃片**（支持位置控制）
  - ✅ **动态波浪效果**（基于时间变化）

## 🎮 控制参数
- `distortion`: 扭曲强度控制
- `glassPosition`: 玻璃片中心位置
- `glassSize`: 玻璃片大小
- `thickness`: 玻璃厚度

## 🔧 使用说明
这些着色器文件与以下C++代码配合使用：
- `LiquidGlass.cpp` - 玻璃片渲染逻辑
- `main.cpp` - 主程序控制

## 📊 清理记录
已删除的过时文件：
- ❌ android_liquid_glass.frag（Android专用球状效果）
- ❌ liquid_glass_enhanced.frag（旧版增强效果）
- ❌ liquid_glass_optimized.vert（优化版顶点着色器）
- ❌ liquid_glass_realistic.frag（旧版真实效果）
- ❌ liquid_glass_simple.frag（旧版简单效果）
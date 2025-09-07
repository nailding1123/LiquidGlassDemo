# 液态玻璃旋转控制指南

## 当前状态
✅ **旋转已停止** - 液态玻璃现在保持静止状态

## 控制方法

### 通过代码控制
```cpp
// 在main.cpp或其他控制代码中
liquidGlass->SetRotationEnabled(false);  // 停止旋转（当前状态）
liquidGlass->SetRotationEnabled(true);   // 启用旋转
```

### 通过键盘控制（建议添加到main.cpp）
```cpp
// 在键盘回调函数中添加
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_R:  // R键切换旋转状态
                static bool rotationEnabled = false;
                rotationEnabled = !rotationEnabled;
                liquidGlass->SetRotationEnabled(rotationEnabled);
                std::cout << "旋转 " << (rotationEnabled ? "已启用" : "已停止") << std::endl;
                break;
        }
    }
}
```

## 参数说明

### 旋转速度调整
```cpp
// 如果需要调整旋转速度，可以修改LiquidGlass.cpp中的Render函数
// 将 0.5f 改为其他值
model = glm::rotate(model, m_time * 0.3f, glm::vec3(0.0f, 1.0f, 0.0f));  // 更慢
model = glm::rotate(model, m_time * 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));  // 更快
```

### 旋转轴调整
```cpp
// 可以修改为绕不同轴旋转
model = glm::rotate(model, m_time * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));  // 绕X轴
model = glm::rotate(model, m_time * 0.5f, glm::vec3(0.0f, 0.0f, 1.0f));  // 绕Z轴
```

## 效果对比

### 静止状态（当前）
- 液态玻璃保持固定位置
- 更容易观察细节效果
- 更好的性能表现

### 旋转状态（启用后）
- 动态展示不同角度的效果
- 更真实的玻璃质感
- 增强的3D立体感

## 调试建议

如果需要验证旋转是否真正停止，可以：
1. 观察玻璃球是否完全静止
2. 检查边缘光散射效果是否保持不变
3. 确认背景扭曲效果是否稳定

## 快速测试

运行程序后，你应该看到：
- 玻璃球完全静止在屏幕中央
- 液态效果仍在背景纹理上活跃
- 边缘光散射保持恒定
- 所有其他效果正常运行
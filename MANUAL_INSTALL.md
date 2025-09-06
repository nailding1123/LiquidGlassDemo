# 手动安装指南

如果自动安装脚本 `setup_dependencies.bat` 遇到问题，可以按照以下步骤手动安装依赖库。

## 方法1：使用vcpkg（推荐）

### 1. 安装vcpkg
```bash
# 打开命令提示符或PowerShell
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.bat
./vcpkg integrate install
```

### 2. 安装依赖库
```bash
vcpkg install glfw3:x64-windows
vcpkg install glew:x64-windows
vcpkg install glm:x64-windows
```

### 3. 验证安装
```bash
vcpkg list
```

## 方法2：手动下载和配置

### 1. 下载依赖库

#### GLFW3
- 访问：https://www.glfw.org/download.html
- 下载：64-bit Windows binaries
- 解压到：`lib/glfw3`

#### GLEW
- 访问：http://glew.sourceforge.net/
- 下载：Windows 32-bit and 64-bit
- 解压到：`lib/glew`

#### GLM
- 访问：https://github.com/g-truc/glm/releases
- 下载：最新版本
- 解压到：`lib/glm`

### 2. 配置项目

#### 修改项目属性
1. 右键项目 -> 属性
2. 配置属性 -> VC++目录
3. 添加包含目录：
   ```
   $(ProjectDir)lib\glfw3\include
   $(ProjectDir)lib\glew\include
   $(ProjectDir)lib\glm
   ```
4. 添加库目录：
   ```
   $(ProjectDir)lib\glfw3\lib-vc2019
   $(ProjectDir)lib\glew\lib\Release\x64
   ```

#### 修改链接器输入
1. 配置属性 -> 链接器 -> 输入
2. 添加依赖项：
   ```
   opengl32.lib
   glfw3.lib
   glew32.lib
   ```

### 3. 复制DLL文件

将以下DLL文件复制到可执行文件目录（通常是 `x64/Release/`）：
- `glew32.dll` （来自lib/glew/bin/Release/x64）
- `glfw3.dll` （来自lib/glfw3/lib-vc2019）

## 常见问题解决

### 问题1：找不到头文件
**错误信息**：`fatal error C1083: 无法打开包括文件: 'GL/glew.h'`

**解决方法**：
1. 检查包含目录是否正确设置
2. 确保文件确实存在于指定路径
3. 尝试使用绝对路径测试

### 问题2：链接错误
**错误信息**：`LNK2019: 无法解析的外部符号 __imp__glewInit@0`

**解决方法**：
1. 检查库目录是否正确设置
2. 检查链接器输入中的库文件名是否正确
3. 确保使用匹配的32位/64位版本

### 问题3：运行时错误
**错误信息**：`无法启动程序，因为缺少glfw3.dll`

**解决方法**：
1. 将DLL文件复制到可执行文件目录
2. 或将DLL路径添加到系统PATH环境变量

### 问题4：OpenGL版本不支持
**错误信息**：`Failed to initialize GLEW`

**解决方法**：
1. 更新显卡驱动程序
2. 检查显卡支持的OpenGL版本：
   ```cpp
   // 在main.cpp中添加调试代码
   std::cout << "OpenGL版本: " << glGetString(GL_VERSION) << std::endl;
   ```

## 验证安装

创建一个简单的测试文件 `test_dependencies.cpp`：

```cpp
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

int main() {
    if (!glfwInit()) {
        std::cerr << "GLFW初始化失败" << std::endl;
        return -1;
    }
    
    GLFWwindow* window = glfwCreateWindow(640, 480, "测试", NULL, NULL);
    if (!window) {
        std::cerr << "窗口创建失败" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW初始化失败" << std::endl;
        return -1;
    }
    
    std::cout << "OpenGL版本: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLEW版本: " << glewGetString(GLEW_VERSION) << std::endl;
    std::cout << "GLFW版本: " << glfwGetVersionString() << std::endl;
    
    glfwTerminate();
    return 0;
}
```

## 联系方式

如果仍然遇到问题，请检查：
1. 所有路径是否包含中文或特殊字符
2. 是否使用了正确版本的Visual Studio
3. 显卡驱动是否已更新到最新版本
4. 防火墙是否阻止了某些操作
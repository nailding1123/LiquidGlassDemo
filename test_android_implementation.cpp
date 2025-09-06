// 测试Android液态玻璃技术实现的简单程序
// 使用方法：编译后运行，按空格键切换原始/增强版本

#include <iostream>
#include <GLFW/glfw3.h>
#include "LiquidGlass.h"

// 全局变量
LiquidGlass* liquidGlass = nullptr;
bool useAndroidVersion = false;
GLFWwindow* window = nullptr;

// 键盘回调
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_SPACE:
                useAndroidVersion = !useAndroidVersion;
                std::cout << "切换到 " << (useAndroidVersion ? "Android技术实现" : "原始实现") << std::endl;
                
                // 重新加载着色器
                if (liquidGlass) {
                    // 这里需要修改LiquidGlass类来支持动态切换
                    // liquidGlass->SwitchShader(useAndroidVersion);
                }
                break;
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GLFW_TRUE);
                break;
        }
    }
}

// 初始化OpenGL
bool initOpenGL() {
    if (!glfwInit()) {
        std::cerr << "GLFW初始化失败" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(800, 600, "Android液态玻璃技术测试", nullptr, nullptr);
    if (!window) {
        std::cerr << "窗口创建失败" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyCallback);
    
    // 设置交换间隔
    glfwSwapInterval(1);

    // 初始化GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW初始化失败" << std::endl;
        return false;
    }

    return true;
}

// 主渲染循环
void renderLoop() {
    while (!glfwWindowShouldClose(window)) {
        // 清空屏幕
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 更新和渲染
        if (liquidGlass) {
            liquidGlass->Update(glfwGetTime());
            liquidGlass->Render();
        }

        // 显示信息
        static double lastTime = glfwGetTime();
        static int frameCount = 0;
        frameCount++;
        
        double currentTime = glfwGetTime();
        if (currentTime - lastTime >= 1.0) {
            std::cout << "FPS: " << frameCount << " 使用: " 
                     << (useAndroidVersion ? "Android" : "原始") << std::endl;
            frameCount = 0;
            lastTime = currentTime;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

// 参数调整接口
class AndroidLiquidGlassParams {
public:
    static float distortion;
    static float blurRadius;
    static float edgeBrightness;
    static float fresnelPower;
    static float thickness;
    
    static void increaseParam(const std::string& param) {
        if (param == "distortion") distortion = std::min(distortion + 0.1f, 2.0f);
        else if (param == "blur") blurRadius = std::min(blurRadius + 0.001f, 0.1f);
        else if (param == "brightness") edgeBrightness = std::min(edgeBrightness + 0.2f, 5.0f);
        else if (param == "fresnel") fresnelPower = std::min(fresnelPower + 0.5f, 8.0f);
        else if (param == "thickness") thickness = std::min(thickness + 0.01f, 0.3f);
        
        std::cout << "增加 " << param << " 值: " << getParam(param) << std::endl;
    }
    
    static void decreaseParam(const std::string& param) {
        if (param == "distortion") distortion = std::max(distortion - 0.1f, 0.0f);
        else if (param == "blur") blurRadius = std::max(blurRadius - 0.001f, 0.001f);
        else if (param == "brightness") edgeBrightness = std::max(edgeBrightness - 0.2f, 0.5f);
        else if (param == "fresnel") fresnelPower = std::max(fresnelPower - 0.5f, 1.0f);
        else if (param == "thickness") thickness = std::max(thickness - 0.01f, 0.05f);
        
        std::cout << "减少 " << param << " 值: " << getParam(param) << std::endl;
    }
    
    static float getParam(const std::string& param) {
        if (param == "distortion") return distortion;
        else if (param == "blur") return blurRadius;
        else if (param == "brightness") return edgeBrightness;
        else if (param == "fresnel") return fresnelPower;
        else if (param == "thickness") return thickness;
        return 0.0f;
    }
};

// 初始化参数
float AndroidLiquidGlassParams::distortion = 1.0f;
float AndroidLiquidGlassParams::blurRadius = 0.01f;
float AndroidLiquidGlassParams::edgeBrightness = 2.0f;
float AndroidLiquidGlassParams::fresnelPower = 3.0f;
float AndroidLiquidGlassParams::thickness = 0.1f;

int main() {
    std::cout << "=== Android液态玻璃技术测试程序 ===" << std::endl;
    std::cout << "按键说明：" << std::endl;
    std::cout << "  空格键: 切换原始/增强版本" << std::endl;
    std::cout << "  ESC键: 退出程序" << std::endl;
    std::cout << "  1-5键: 调整参数" << std::endl;
    std::cout << "    1: 扭曲强度" << std::endl;
    std::cout << "    2: 背景模糊" << std::endl;
    std::cout << "    3: 边缘亮度" << std::endl;
    std::cout << "    4: 菲涅尔效果" << std::endl;
    std::cout << "    5: 玻璃厚度" << std::endl;
    std::cout << "  +/-键: 增加/减少当前参数" << std::endl;

    if (!initOpenGL()) {
        return -1;
    }

    try {
        liquidGlass = new LiquidGlass();
        if (!liquidGlass->Initialize()) {
            std::cerr << "液态玻璃初始化失败" << std::endl;
            delete liquidGlass;
            return -1;
        }

        renderLoop();

    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
    }

    if (liquidGlass) {
        delete liquidGlass;
    }

    glfwTerminate();
    return 0;
}

// 编译说明：
// g++ test_android_implementation.cpp LiquidGlass.cpp -lglfw -lGLEW -lGL -o android_test
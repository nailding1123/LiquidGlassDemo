#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "LiquidGlass.h"
#include "Camera.h"
#include "TextureLoader.h"
#include "BackgroundCapture.h"

// Background texture integration
// #include "background_setup.h" // Removed - background setup functionality integrated elsewhere

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

LiquidGlass* liquidGlass;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    // Control distortion
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        float distortion = liquidGlass->GetDistortion() + 0.01f;
        if (distortion > 1.0f) distortion = 1.0f;
        liquidGlass->SetDistortion(distortion);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        float distortion = liquidGlass->GetDistortion() - 0.01f;
        if (distortion < 0.0f) distortion = 0.0f;
        liquidGlass->SetDistortion(distortion);
    }
    
    // Control edge scatter
    static float edgeScatter = 0.8f;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        edgeScatter -= 0.01f;
        if (edgeScatter < 0.0f) edgeScatter = 0.0f;
        liquidGlass->SetEdgeScatter(edgeScatter);
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        edgeScatter += 0.01f;
        if (edgeScatter > 2.0f) edgeScatter = 2.0f;
        liquidGlass->SetEdgeScatter(edgeScatter);
    }
    
    // Control blur radius
    static float blurRadius = 3.0f;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        blurRadius -= 0.1f;
        if (blurRadius < 0.1f) blurRadius = 0.1f;
        liquidGlass->SetBlurRadius(blurRadius);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        blurRadius += 0.1f;
        if (blurRadius > 10.0f) blurRadius = 10.0f;
        liquidGlass->SetBlurRadius(blurRadius);
    }
    
    // Control thickness
    static float thickness = 0.5f;
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
    {
        thickness -= 0.01f;
        if (thickness < 0.1f) thickness = 0.1f;
        liquidGlass->SetThickness(thickness);
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
    {
        thickness += 0.01f;
        if (thickness > 2.0f) thickness = 2.0f;
        liquidGlass->SetThickness(thickness);
    }
    
    // Switch background type
    static bool bPressed = false;
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !bPressed)
    {
        // SwitchBackgroundType(); // Background switching not implemented
        std::cout << "Background switching not available" << std::endl;
        bPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE)
    {
        bPressed = false;
    }
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Liquid Glass Demo", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    liquidGlass = new LiquidGlass();
    liquidGlass->Initialize();
    
    // 使用英文提示避免中文乱码问题
    // 使用ASCII兼容的中文提示
    std::cout << "=== Liquid Glass Demo ===" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "WASD  : Move camera" << std::endl;
    std::cout << "Mouse : Look around" << std::endl;
    std::cout << "Wheel : Zoom" << std::endl;
    std::cout << "Up/Down  : Distortion" << std::endl;
    std::cout << "Left/Right : Edge scatter" << std::endl;
    std::cout << "Q/E   : Blur radius" << std::endl;
    std::cout << "T/G   : Thickness" << std::endl;
    std::cout << "B     : Background (not ready)" << std::endl;
    std::cout << "ESC   : Exit" << std::endl;

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        // 清除背景
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        liquidGlass->Update(deltaTime);
        liquidGlass->Render(projection, view);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete liquidGlass;

    glfwTerminate();
    return 0;
}
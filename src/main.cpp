#include <iostream>
#include <vector>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "LiquidGlass.h"
#include "BackgroundCapture.h"
#include "SDFGenerator.h"
#include "BackgroundRenderer.h"
#include "Camera.h"
#include "TextureLoader.h"

const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 1536;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

float deltaTime = 0.0f;
float lastFrame = 0.0f;

LiquidGlass* liquidGlass;
BackgroundCapture* backgroundCapture;
SDFGenerator* sdfGenerator;
BackgroundRenderer* backgroundRenderer;

std::vector<std::string> backgroundFiles = {
    "backgrounds/background.png",
    "backgrounds/background1.png"
};

size_t currentBackgroundIndex = 0;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    if (backgroundRenderer) {
        backgroundRenderer->SetScreenSize(width, height);
    }
}

void switchBackground()
{
    if (!backgroundRenderer) return;
    
    currentBackgroundIndex = (currentBackgroundIndex + 1) % backgroundFiles.size();
    backgroundRenderer->LoadBackground(backgroundFiles[currentBackgroundIndex]);
    std::cout << "Switched to background: " << backgroundFiles[currentBackgroundIndex] << std::endl;
}

float g_ref_height = 20.0f;
float g_ref_length = 30.0f;

void processInput(GLFWwindow* window)
{
    static bool bKeyPressed = false;
    static bool iKeyPressed = false;
    static bool kKeyPressed = false;
    static bool jKeyPressed = false;
    static bool lKeyPressed = false;
    
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    glm::vec2 glassPos = liquidGlass->GetGlassPosition();
    float moveSpeed = 0.02f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        liquidGlass->SetGlassPosition(glassPos + glm::vec2(0.0f, moveSpeed));
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        liquidGlass->SetGlassPosition(glassPos + glm::vec2(0.0f, -moveSpeed));
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        liquidGlass->SetGlassPosition(glassPos + glm::vec2(-moveSpeed, 0.0f));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        liquidGlass->SetGlassPosition(glassPos + glm::vec2(moveSpeed, 0.0f));
        
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
    {
        if (!bKeyPressed)
        {
            switchBackground();
            bKeyPressed = true;
        }
    }
    else
    {
        bKeyPressed = false;
    }
    
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        if (!iKeyPressed)
        {
            g_ref_height += 10.0f;
            if (g_ref_height > 100.0f) g_ref_height = 100.0f;
            iKeyPressed = true;
        }
    }
    else
    {
        iKeyPressed = false;
    }
    
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        if (!kKeyPressed)
        {
            g_ref_height -= 10.0f;
            if (g_ref_height < 5.0f) g_ref_height = 5.0f;
            kKeyPressed = true;
        }
    }
    else
    {
        kKeyPressed = false;
    }
    
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        if (!jKeyPressed)
        {
            g_ref_length -= 10.0f;
            if (g_ref_length < 5.0f) g_ref_length = 5.0f;
            jKeyPressed = true;
        }
    }
    else
    {
        jKeyPressed = false;
    }
    
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        if (!lKeyPressed)
        {
            g_ref_length += 10.0f;
            if (g_ref_length > 100.0f) g_ref_length = 100.0f;
            lKeyPressed = true;
        }
    }
    else
    {
        lKeyPressed = false;
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

    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    backgroundRenderer = new BackgroundRenderer();
    backgroundRenderer->Initialize();
    backgroundRenderer->LoadBackground("backgrounds/background.png");
    backgroundRenderer->SetScreenSize(SCR_WIDTH, SCR_HEIGHT);

    backgroundCapture = new BackgroundCapture();
    backgroundCapture->Initialize(SCR_WIDTH, SCR_HEIGHT);

    sdfGenerator = new SDFGenerator();
    sdfGenerator->Initialize(SCR_WIDTH, SCR_HEIGHT);

    liquidGlass = new LiquidGlass();
    liquidGlass->Initialize();
    liquidGlass->SetBackgroundCapture(backgroundCapture);
    liquidGlass->SetSDFGenerator(sdfGenerator);
    liquidGlass->SetBackgroundRenderer(backgroundRenderer);
    liquidGlass->SetScreenSize(SCR_WIDTH, SCR_HEIGHT);
    liquidGlass->SetGlassPosition(glm::vec2(0.0f, 0.0f));
    liquidGlass->SetGlassSize(glm::vec2(0.6f, 0.4f));
    
    std::cout << "=== Liquid Glass Demo ===" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "WASD  : Move liquid glass" << std::endl;
    std::cout << "ESC   : Exit" << std::endl;

    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        liquidGlass->Update(deltaTime);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.GetViewMatrix();
        
        backgroundRenderer->Render(projection, view);
        
        liquidGlass->Render(projection, view);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete liquidGlass;
    delete backgroundCapture;
    delete sdfGenerator;
    delete backgroundRenderer;

    glfwTerminate();
    return 0;
}
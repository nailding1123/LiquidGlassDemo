#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

class BackgroundRenderer {
public:
    BackgroundRenderer();
    ~BackgroundRenderer();
    bool Initialize();
    void LoadBackground(const std::string& imagePath);
    void Render(const glm::mat4& projection, const glm::mat4& view);
    void Cleanup();
    void SetScreenSize(int width, int height);
    GLuint GetBackgroundTexture() const { return m_texture; }

private:
    void CreateFullscreenQuad();
    void LoadShader();

    GLuint m_VAO;
    GLuint m_VBO;
    GLuint m_EBO;
    GLuint m_shaderProgram;
    GLuint m_texture;
    int m_screenWidth;
    int m_screenHeight;
    bool m_initialized;
};
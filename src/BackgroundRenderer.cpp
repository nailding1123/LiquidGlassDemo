#include "BackgroundRenderer.h"
#include "TextureLoader.h"
#include "Shader.h"
#include <iostream>

BackgroundRenderer::BackgroundRenderer() : m_VAO(0), m_VBO(0), m_EBO(0), 
    m_shaderProgram(0), m_texture(0), m_screenWidth(800), m_screenHeight(600), 
    m_initialized(false) {
}

BackgroundRenderer::~BackgroundRenderer() {
    Cleanup();
}

bool BackgroundRenderer::Initialize() {
    if (m_initialized) return true;
    
    CreateFullscreenQuad();
    LoadShader();
    
    m_initialized = true;
    return true;
}

void BackgroundRenderer::CreateFullscreenQuad() {
    float vertices[] = {
        -1.0f, -1.0f,   0.0f, 0.0f,
         1.0f, -1.0f,   1.0f, 0.0f,
         1.0f,  1.0f,   1.0f, 1.0f,
        -1.0f,  1.0f,   0.0f, 1.0f
    };
    
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    
    glBindVertexArray(m_VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

void BackgroundRenderer::LoadShader() {
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoord;
        
        out vec2 TexCoord;
        
        void main() {
            gl_Position = vec4(aPos, 0.0, 1.0);
            TexCoord = aTexCoord;
        }
    )";
    
    const char* fragmentShaderSource = R"(
        #version 330 core
        in vec2 TexCoord;
        out vec4 FragColor;
        
        uniform sampler2D backgroundTexture;
        
        void main() {
            FragColor = texture(backgroundTexture, TexCoord);
        }
    )";
    
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    
    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, vertexShader);
    glAttachShader(m_shaderProgram, fragmentShader);
    glLinkProgram(m_shaderProgram);
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void BackgroundRenderer::LoadBackground(const std::string& imagePath) {
    m_texture = TextureLoader::loadTexture(imagePath);
    if (m_texture == 0) {
        std::cerr << "Failed to load background image: " << imagePath << std::endl;
    }
}

void BackgroundRenderer::Render(const glm::mat4& projection, const glm::mat4& view) {
    if (!m_initialized || m_texture == 0) return;
    
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    
    glUseProgram(m_shaderProgram);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glUniform1i(glGetUniformLocation(m_shaderProgram, "backgroundTexture"), 0);
    
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
}

void BackgroundRenderer::SetScreenSize(int width, int height) {
    m_screenWidth = width;
    m_screenHeight = height;
}

void BackgroundRenderer::Cleanup() {
    if (m_texture != 0) {
        TextureLoader::deleteTexture(m_texture);
        m_texture = 0;
    }
    
    if (m_VAO != 0) {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }
    
    if (m_VBO != 0) {
        glDeleteBuffers(1, &m_VBO);
        m_VBO = 0;
    }
    
    if (m_EBO != 0) {
        glDeleteBuffers(1, &m_EBO);
        m_EBO = 0;
    }
    
    if (m_shaderProgram != 0) {
        glDeleteProgram(m_shaderProgram);
        m_shaderProgram = 0;
    }
    
    m_initialized = false;
}
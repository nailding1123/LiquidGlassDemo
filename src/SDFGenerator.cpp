#include "SDFGenerator.h"
#include "Shader.h"
#include <iostream>

SDFGenerator::SDFGenerator() 
    : m_fbo(0), m_sdfTexture(0), m_vao(0), m_vbo(0), m_shaderProgram(0)
    , m_width(0), m_height(0) {
}

SDFGenerator::~SDFGenerator() {
    Cleanup();
}

bool SDFGenerator::Initialize(int width, int height) {
    m_width = width;
    m_height = height;
    
    if (!CreateFramebuffer()) {
        return false;
    }
    
    if (!CreateQuad()) {
        return false;
    }
    
    if (!LoadShaders()) {
        return false;
    }
    
    return true;
}

bool SDFGenerator::CreateFramebuffer() {
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    
    glGenTextures(1, &m_sdfTexture);
    glBindTexture(GL_TEXTURE_2D, m_sdfTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_sdfTexture, 0);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "SDFGenerator: Framebuffer is not complete!" << std::endl;
        Cleanup();
        return false;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

bool SDFGenerator::CreateQuad() {
    float vertices[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
    return true;
}

bool SDFGenerator::LoadShaders() {
    try {
        Shader sdfShader("shaders/sdf_generator.vert", "shaders/sdf_generator.frag");
        m_shaderProgram = sdfShader.ID;
        return true;
    } catch (const std::exception& e) {
        std::cout << "SDFGenerator: Failed to load shaders: " << e.what() << std::endl;
        
        try {
            Shader sdfShader("C:/Users/lbd111/LiquidGlassDemo01/shaders/sdf_generator.vert", 
                             "C:/Users/lbd111/LiquidGlassDemo01/shaders/sdf_generator.frag");
            m_shaderProgram = sdfShader.ID;
            return true;
        } catch (const std::exception& e2) {
            std::cout << "SDFGenerator: Failed to load shaders with absolute path: " << e2.what() << std::endl;
            return false;
        }
    }
}

void SDFGenerator::GenerateFromTexture(GLuint inputTexture, float threshold) {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_width, m_height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(m_shaderProgram);
    glUniform1i(glGetUniformLocation(m_shaderProgram, "backgroundTexture"), 0);
    glUniform2f(glGetUniformLocation(m_shaderProgram, "resolution"), (float)m_width, (float)m_height);
    glUniform1f(glGetUniformLocation(m_shaderProgram, "threshold"), threshold);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, inputTexture);
    
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SDFGenerator::SetSize(int width, int height) {
    if (m_fbo) {
        glDeleteFramebuffers(1, &m_fbo);
        m_fbo = 0;
    }
    if (m_sdfTexture) {
        glDeleteTextures(1, &m_sdfTexture);
        m_sdfTexture = 0;
    }
    if (m_vao) {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
    if (m_vbo) {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }
    
    m_width = width;
    m_height = height;
    
    CreateFramebuffer();
    CreateQuad();
}

void SDFGenerator::Cleanup() {
    if (m_fbo) {
        glDeleteFramebuffers(1, &m_fbo);
        m_fbo = 0;
    }
    if (m_sdfTexture) {
        glDeleteTextures(1, &m_sdfTexture);
        m_sdfTexture = 0;
    }
    if (m_vao) {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
    if (m_vbo) {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }
    m_shaderProgram = 0;
}
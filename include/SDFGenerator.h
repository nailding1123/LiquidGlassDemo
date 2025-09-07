#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

class SDFGenerator {
public:
    SDFGenerator();
    ~SDFGenerator();
    bool Initialize(int width, int height);
    void GenerateFromTexture(GLuint inputTexture, float threshold = 0.5f);
    GLuint GetSDFTexture() const { return m_sdfTexture; }
    void SetSize(int width, int height);
    void Cleanup();

private:
    bool CreateFramebuffer();
    bool CreateQuad();
    bool LoadShaders();
    void RenderQuad();

    GLuint m_fbo;
    GLuint m_sdfTexture;
    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_shaderProgram;
    int m_width;
    int m_height;
};
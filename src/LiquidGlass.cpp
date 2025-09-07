#include "LiquidGlass.h"
#include "BackgroundCapture.h"
#include "SDFGenerator.h"
#include "BackgroundRenderer.h"
#include "Shader.h"
#include "TextureLoader.h"
#include <iostream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

extern float g_ref_height;
extern float g_ref_length;

LiquidGlass::LiquidGlass() : m_distortion(3.0f)
{
    m_material.color = glm::vec3(0.98f, 0.99f, 1.0f);
    m_material.transparency = 0.98f;
    m_glassPosition = glm::vec2(0.0f, 0.0f);
    m_glassSize = glm::vec2(0.6f, 0.6f);
}

LiquidGlass::~LiquidGlass()
{
    Cleanup();
}

void LiquidGlass::Cleanup()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
}

void LiquidGlass::Initialize()
{
    CreatePlane();
    SetupBuffers();
    LoadShaders();
}

void LiquidGlass::CreatePlane()
{
    m_vertices.clear();
    m_indices.clear();

    float radius = 0.5f;
    int segments = 32;
    
    m_vertices.push_back(0.0f);
    m_vertices.push_back(0.0f);
    m_vertices.push_back(0.0f);
    m_vertices.push_back(0.0f);
    m_vertices.push_back(0.0f);
    m_vertices.push_back(1.0f);
    m_vertices.push_back(0.5f);
    m_vertices.push_back(0.5f);
    
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * 3.1415926f * float(i) / float(segments);
        float x = cos(angle) * radius;
        float y = sin(angle) * radius;
        
        m_vertices.push_back(x);
        m_vertices.push_back(y);
        m_vertices.push_back(0.0f);
        m_vertices.push_back(0.0f);
        m_vertices.push_back(0.0f);
        m_vertices.push_back(1.0f);
        m_vertices.push_back(x + 0.5f);
        m_vertices.push_back(y + 0.5f);
    }
    
    for (int i = 0; i < segments; i++) {
        m_indices.push_back(0);
        m_indices.push_back(i + 1);
        m_indices.push_back(i + 2);
    }
    
    m_vertexCount = segments + 2;
    m_indexCount = segments * 3;
}

void LiquidGlass::SetupBuffers()
{
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), &m_vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void LiquidGlass::LoadShaders()
{
    Shader glassShader("shaders/liquid_glass.vert", "shaders/liquid_glass.frag");
    m_shaderProgram = glassShader.ID;
}

void LiquidGlass::Update(float deltaTime)
{
    UpdateBackgroundCapture();
}

void LiquidGlass::UpdateBackgroundCapture()
{
    if (m_backgroundCapture) {
        m_backgroundCapture->UpdateCaptureRegion(m_glassPosition, m_glassSize);
    }
}

void LiquidGlass::Render(const glm::mat4& projection, const glm::mat4& view)
{
    if (!m_shaderProgram || !m_backgroundRenderer) return;

    GLuint backgroundTexture = m_backgroundRenderer->GetBackgroundTexture();
    
    if (backgroundTexture == 0) {
        std::cout << "Warning: Background texture is invalid!" << std::endl;
        return;
    }
    
    GLuint backgroundCaptureTexture = backgroundTexture;
    
    if (m_backgroundCapture) {
        glm::vec2 glassCenter = m_glassPosition;
        glm::vec2 captureSize = glm::vec2(m_glassSize.x, m_glassSize.x);
        
        m_backgroundCapture->UpdateCaptureRegion(glassCenter, captureSize);
        
        m_backgroundCapture->BeginCapture();
        m_backgroundCapture->EndCapture();
        backgroundCaptureTexture = m_backgroundCapture->GetCaptureTexture();
    }
    
    GLuint sdfTexture = 0;
    if (m_sdfGenerator && m_backgroundCapture) {
        m_sdfGenerator->SetSize(m_backgroundCapture->GetCaptureWidth(), m_backgroundCapture->GetCaptureHeight());
        m_sdfGenerator->GenerateFromTexture(backgroundCaptureTexture, 0.5f);
        sdfTexture = m_sdfGenerator->GetSDFTexture();
    }

    GLboolean depthTestEnabled;
    glGetBooleanv(GL_DEPTH_TEST, &depthTestEnabled);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE); 

    glUseProgram(m_shaderProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, backgroundTexture);
    glUniform1i(glGetUniformLocation(m_shaderProgram, "backgroundTexture"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, sdfTexture);
    glUniform1i(glGetUniformLocation(m_shaderProgram, "sdfTexture"), 1);

    glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(m_glassPosition.x, m_glassPosition.y, 0.0f));
    model = glm::scale(model, glm::vec3(m_glassSize.x, m_glassSize.y, 1.0f));
    glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);

    glUniform3fv(glGetUniformLocation(m_shaderProgram, "materialColor"), 1, &m_material.color[0]);
    glUniform1f(glGetUniformLocation(m_shaderProgram, "materialTransparency"), m_material.transparency);
    
    glUniform1f(glGetUniformLocation(m_shaderProgram, "ref_height"), g_ref_height);
    glUniform1f(glGetUniformLocation(m_shaderProgram, "ref_length"), g_ref_length);
    glUniform1f(glGetUniformLocation(m_shaderProgram, "ref_border_width"), 5.0f);
    glUniform1f(glGetUniformLocation(m_shaderProgram, "ref_exposure"), 1.0f);
    glUniform1f(glGetUniformLocation(m_shaderProgram, "scale"), 1.0f);

    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
    glDepthMask(GL_TRUE);
    
    if (!depthTestEnabled) {
        glDisable(GL_DEPTH_TEST);
    }
}
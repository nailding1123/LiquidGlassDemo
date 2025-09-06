#include "LiquidGlass.h"
#include "Shader.h"
#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

LiquidGlass::LiquidGlass() : m_time(0.0f), m_distortion(0.5f), m_edgeScatter(0.8f), m_blurRadius(3.0f), m_thickness(0.5f)
{
    m_material.color = glm::vec3(0.7f, 0.9f, 1.0f);
    m_material.refractiveIndex = 1.5f;
    m_material.reflectivity = 0.9f;
    m_material.transparency = 0.8f;
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
    glDeleteTextures(1, &m_backgroundTexture);
}

void LiquidGlass::Initialize()
{
    CreateSphere(1.0f, 64);
    SetupBuffers();
    LoadShaders();
    
    // 创建渐变背景纹理
    int width = 800, height = 600;
    std::vector<unsigned char> gradientData(width * height * 3);
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float t = (float)y / (float)height;
            int r = 30 + (int)(t * 80);
            int g = 60 + (int)(t * 120);
            int b = 120 + (int)(t * 100);
            
            gradientData[(y * width + x) * 3 + 0] = (unsigned char)r;
            gradientData[(y * width + x) * 3 + 1] = (unsigned char)g;
            gradientData[(y * width + x) * 3 + 2] = (unsigned char)b;
        }
    }
    
    glGenTextures(1, &m_backgroundTexture);
    glBindTexture(GL_TEXTURE_2D, m_backgroundTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, &gradientData[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void LiquidGlass::CreateSphere(float radius, int segments)
{
    m_vertices.clear();
    m_indices.clear();

    for (int lat = 0; lat <= segments; ++lat) {
        float theta = lat * glm::pi<float>() / segments;
        float sinTheta = sin(theta);
        float cosTheta = cos(theta);

        for (int lon = 0; lon <= segments; ++lon) {
            float phi = lon * 2.0f * glm::pi<float>() / segments;
            float sinPhi = sin(phi);
            float cosPhi = cos(phi);

            float x = cosPhi * sinTheta;
            float y = cosTheta;
            float z = sinPhi * sinTheta;

            // Position
            m_vertices.push_back(radius * x);
            m_vertices.push_back(radius * y);
            m_vertices.push_back(radius * z);

            // Normal
            m_vertices.push_back(x);
            m_vertices.push_back(y);
            m_vertices.push_back(z);

            // Texture coordinates
            m_vertices.push_back((float)lon / segments);
            m_vertices.push_back((float)lat / segments);
        }
    }

    for (int lat = 0; lat < segments; ++lat) {
        for (int lon = 0; lon < segments; ++lon) {
            int current = lat * (segments + 1) + lon;
            int next = current + segments + 1;

            m_indices.push_back(current);
            m_indices.push_back(next);
            m_indices.push_back(current + 1);

            m_indices.push_back(next);
            m_indices.push_back(next + 1);
            m_indices.push_back(current + 1);
        }
    }

    m_vertexCount = m_vertices.size() / 8;
    m_indexCount = m_indices.size();
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

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Texture coordinate attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void LiquidGlass::LoadShaders()
{
    Shader shader("shaders/liquid_glass.vert", "shaders/liquid_glass.frag");
    m_shaderProgram = shader.ID;
}

void LiquidGlass::Update(float deltaTime)
{
    m_time += deltaTime;
}

void LiquidGlass::Render(const glm::mat4& projection, const glm::mat4& view)
{
    glUseProgram(m_shaderProgram);

    // Set uniforms
    glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);
    
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, m_time * 0.5f, glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);

    glUniform3fv(glGetUniformLocation(m_shaderProgram, "material.color"), 1, &m_material.color[0]);
    glUniform1f(glGetUniformLocation(m_shaderProgram, "material.refractiveIndex"), m_material.refractiveIndex);
    glUniform1f(glGetUniformLocation(m_shaderProgram, "material.reflectivity"), m_material.reflectivity);
    glUniform1f(glGetUniformLocation(m_shaderProgram, "material.transparency"), m_material.transparency);
    glUniform1f(glGetUniformLocation(m_shaderProgram, "time"), m_time);
    glUniform1f(glGetUniformLocation(m_shaderProgram, "distortion"), m_distortion);
    glUniform1f(glGetUniformLocation(m_shaderProgram, "edgeScatter"), m_edgeScatter);
    glUniform1f(glGetUniformLocation(m_shaderProgram, "blurRadius"), m_blurRadius);
    glUniform1f(glGetUniformLocation(m_shaderProgram, "thickness"), m_thickness);
    glUniform2f(glGetUniformLocation(m_shaderProgram, "resolution"), 800.0f, 600.0f);
    
    // Bind background texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_backgroundTexture);
    glUniform1i(glGetUniformLocation(m_shaderProgram, "backgroundTexture"), 0);

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    // Render sphere
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glDisable(GL_BLEND);
}
#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class LiquidGlass
{
public:
    LiquidGlass();
    ~LiquidGlass();

    void Initialize();
    void Update(float deltaTime);
    void Render(const glm::mat4& projection, const glm::mat4& view);
    void Cleanup();
    void SetTime(float time) { m_time = time; }
    void SetDistortion(float distortion) { m_distortion = distortion; }
    float GetDistortion() const { return m_distortion; }
    void SetEdgeScatter(float scatter) { m_edgeScatter = scatter; }
    void SetBlurRadius(float radius) { m_blurRadius = radius; }
    void SetThickness(float thickness) { m_thickness = thickness; }

private:
    void CreateSphere(float radius, int segments);
    void SetupBuffers();
    void LoadShaders();

    GLuint m_VAO, m_VBO, m_EBO;
    GLuint m_vertexCount, m_indexCount;
    std::vector<float> m_vertices;
    std::vector<unsigned int> m_indices;

    GLuint m_shaderProgram;
    GLuint m_backgroundTexture;
    
    float m_time;
    float m_distortion;
    float m_edgeScatter;
    float m_blurRadius;
    float m_thickness;
    
    struct Material {
        glm::vec3 color;
        float refractiveIndex;
        float reflectivity;
        float transparency;
    } m_material;
};
#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>

class BackgroundCapture;
class SDFGenerator;
class BackgroundRenderer;

struct GlassMaterial {
    glm::vec3 color;
    float transparency;
};

class LiquidGlass
{
public:
    LiquidGlass();
    ~LiquidGlass();
    void Initialize();
    void Update(float deltaTime);
    void Render(const glm::mat4& projection, const glm::mat4& view);
    void Cleanup();
    void SetDistortion(float distortion) { m_distortion = distortion; }
    float GetDistortion() const { return m_distortion; }
    void SetRotationEnabled(bool enabled) { m_rotationEnabled = enabled; }
    bool IsRotationEnabled() const { return m_rotationEnabled; }
    void SetGlassPosition(const glm::vec2& pos) { 
        m_glassPosition = pos; 
        UpdateBackgroundCapture();
    }
    const glm::vec2& GetGlassPosition() const { return m_glassPosition; }
    void SetGlassSize(const glm::vec2& size) { 
        m_glassSize = size; 
        UpdateBackgroundCapture();
    }
    const glm::vec2& GetGlassSize() const { return m_glassSize; }
    void SetBackgroundCapture(BackgroundCapture* capture) { m_backgroundCapture = capture; }
    void SetSDFGenerator(SDFGenerator* generator) { m_sdfGenerator = generator; }
    void SetBackgroundRenderer(BackgroundRenderer* renderer) { m_backgroundRenderer = renderer; }
    void SetScreenSize(int width, int height) { 
        m_screenWidth = width; 
        m_screenHeight = height; 
    }

private:
    void CreatePlane();
    void SetupBuffers();
    void LoadShaders();
    void UpdateBackgroundCapture();

    GLuint m_VAO;
    GLuint m_VBO;
    GLuint m_EBO;
    GLuint m_vertexCount;
    GLuint m_indexCount;
    std::vector<float> m_vertices;
    std::vector<unsigned int> m_indices;
    GLuint m_shaderProgram;
    glm::vec2 m_glassPosition;
    glm::vec2 m_glassSize;
    float m_distortion;
    bool m_rotationEnabled;
    BackgroundCapture* m_backgroundCapture;
    SDFGenerator* m_sdfGenerator;
    BackgroundRenderer* m_backgroundRenderer;
    GlassMaterial m_material;
    int m_screenWidth;
    int m_screenHeight;
};
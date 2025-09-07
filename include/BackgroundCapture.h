#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

class BackgroundCapture {
public:
    BackgroundCapture();
    ~BackgroundCapture();
    bool Initialize(int screenWidth, int screenHeight);
    void BeginCapture();
    void BeginCapture(const glm::vec2& glassPosition, const glm::vec2& glassSize);
    void EndCapture();
    GLuint GetCaptureTexture() const { return m_captureTexture; }
    void SetCaptureRegion(int x, int y, int width, int height);
    void UpdateCaptureRegion(const glm::vec2& glassPosition, const glm::vec2& glassSize);
    void SetBackgroundTexture(GLuint texture) { m_backgroundTexture = texture; }
    void Cleanup();
    int GetCaptureWidth() const { return m_captureWidth; }
    int GetCaptureHeight() const { return m_captureHeight; }

private:
    bool CreateFramebuffer();
    void SetupCaptureViewport(int x, int y, int width, int height);

    GLuint m_fbo;
    GLuint m_captureTexture;
    GLuint m_rbo;
    GLuint m_backgroundTexture;
    int m_screenWidth;
    int m_screenHeight;
    int m_captureX;
    int m_captureY;
    int m_captureWidth;
    int m_captureHeight;
};
#include "BackgroundCapture.h"
#include <iostream>
#include <glm/glm.hpp>

BackgroundCapture::BackgroundCapture()
    : m_fbo(0), m_captureTexture(0), m_rbo(0)
    , m_screenWidth(0), m_screenHeight(0)
    , m_captureX(0), m_captureY(0), m_captureWidth(0), m_captureHeight(0) {
}

BackgroundCapture::~BackgroundCapture() {
    Cleanup();
}

bool BackgroundCapture::Initialize(int screenWidth, int screenHeight) {
    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;

    m_captureX = 0;
    m_captureY = 0;
    m_captureWidth = screenWidth;
    m_captureHeight = screenHeight;

    return CreateFramebuffer();
}

bool BackgroundCapture::CreateFramebuffer() {
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    glGenTextures(1, &m_captureTexture);
    glBindTexture(GL_TEXTURE_2D, m_captureTexture);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA,
        m_captureWidth, m_captureHeight, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, nullptr
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D, m_captureTexture, 0
    );

    glGenRenderbuffers(1, &m_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
    glRenderbufferStorage(
        GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
        m_captureWidth, m_captureHeight
    );
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER, m_rbo
    );

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "BackgroundCapture: Framebuffer is not complete!" << std::endl;
        Cleanup();
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

void BackgroundCapture::BeginCapture() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_captureWidth, m_captureHeight);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);

    glBlitFramebuffer(
        m_captureX, m_captureY,
        m_captureX + m_captureWidth, m_captureY + m_captureHeight,
        0, 0,
        m_captureWidth, m_captureHeight,
        GL_COLOR_BUFFER_BIT,
        GL_LINEAR
    );

    glClear(GL_DEPTH_BUFFER_BIT);
}

void BackgroundCapture::EndCapture() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void BackgroundCapture::SetCaptureRegion(int x, int y, int width, int height) {
    if (width <= 0 || height <= 0) {
        std::cout << "BackgroundCapture: Invalid capture region size!" << std::endl;
        return;
    }

    if (m_fbo) {
        glDeleteFramebuffers(1, &m_fbo);
        m_fbo = 0;
    }
    if (m_captureTexture) {
        glDeleteTextures(1, &m_captureTexture);
        m_captureTexture = 0;
    }
    if (m_rbo) {
        glDeleteRenderbuffers(1, &m_rbo);
        m_rbo = 0;
    }

    m_captureX = x;
    m_captureY = y;
    m_captureWidth = width;
    m_captureHeight = height;

    CreateFramebuffer();
}

void BackgroundCapture::UpdateCaptureRegion(const glm::vec2& glassPosition, const glm::vec2& glassSize) {
    
    int glassCenterX = static_cast<int>(((glassPosition.x + 1.0f) * 0.5f) * m_screenWidth);
    int glassCenterY = static_cast<int>(((1.0f - glassPosition.y) * 0.5f) * m_screenHeight);
    
    int glassHalfWidth = static_cast<int>(glassSize.x * m_screenWidth * 0.5f);
    int glassHalfHeight = static_cast<int>(glassSize.y * m_screenHeight * 0.5f);

    int margin = 0;
    
    int captureX = glassCenterX - glassHalfWidth;
    int captureY = glassCenterY - glassHalfHeight;
    int captureWidth = static_cast<int>(glassSize.x * m_screenWidth * 2.0f);
    int captureHeight = static_cast<int>(glassSize.y * m_screenHeight * 2.0f);

    captureX = glm::max(0, glm::min(captureX, m_screenWidth - captureWidth));
    captureY = glm::max(0, glm::min(captureY, m_screenHeight - captureHeight));
    captureWidth = glm::min(captureWidth, m_screenWidth);
    captureHeight = glm::min(captureHeight, m_screenHeight);

    if (captureX + captureWidth < m_screenWidth) {
        captureWidth = m_screenWidth - captureX;
    }
    if (captureY + captureHeight < m_screenHeight) {
        captureHeight = m_screenHeight - captureY;
    }

    if (captureWidth > 0 && captureHeight > 0) {
        SetCaptureRegion(captureX, captureY, captureWidth, captureHeight);
    }
}

void BackgroundCapture::Cleanup() {
    if (m_fbo) {
        glDeleteFramebuffers(1, &m_fbo);
        m_fbo = 0;
    }
    if (m_captureTexture) {
        glDeleteTextures(1, &m_captureTexture);
        m_captureTexture = 0;
    }
    if (m_rbo) {
        glDeleteRenderbuffers(1, &m_rbo);
        m_rbo = 0;
    }

    m_screenWidth = 0;
    m_screenHeight = 0;
    m_captureX = 0;
    m_captureY = 0;
    m_captureWidth = 0;
    m_captureHeight = 0;
}
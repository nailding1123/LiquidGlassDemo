#include "BackgroundCapture.h"
#include <iostream>

GLuint BackgroundCapture::fbo = 0;
GLuint BackgroundCapture::rbo = 0;
int BackgroundCapture::screenWidth = 0;
int BackgroundCapture::screenHeight = 0;
bool BackgroundCapture::initialized = false;

void BackgroundCapture::setupFramebuffer(int width, int height) {
    if (initialized) return;
    
    screenWidth = width;
    screenHeight = height;
    
    glGenFramebuffers(1, &fbo);
    glGenRenderbuffers(1, &rbo);
    
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer not complete!" << std::endl;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    initialized = true;
}

GLuint BackgroundCapture::captureBackground(int x, int y, int width, int height) {
    if (!initialized) return 0;
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // 从当前帧缓冲读取背景
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0); // 默认帧缓冲
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    
    glBlitFramebuffer(x, y, x + width, y + height, 
                      0, 0, width, height, 
                      GL_COLOR_BUFFER_BIT, GL_LINEAR);
    
    // 将捕获的内容复制到纹理
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, width, height);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return textureID;
}

void BackgroundCapture::cleanup() {
    if (fbo) glDeleteFramebuffers(1, &fbo);
    if (rbo) glDeleteRenderbuffers(1, &rbo);
    fbo = rbo = 0;
    initialized = false;
}
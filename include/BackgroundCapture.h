#pragma once

#include <GL/glew.h>
#include <vector>

class BackgroundCapture {
public:
    static GLuint captureBackground(int x, int y, int width, int height);
    static void setupFramebuffer(int screenWidth, int screenHeight);
    static void cleanup();
    
private:
    static GLuint fbo;
    static GLuint rbo;
    static int screenWidth;
    static int screenHeight;
    static bool initialized;
};
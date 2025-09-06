#pragma once

#include <GL/glew.h>
#include <string>
#include <vector>

class TextureLoader {
public:
    static GLuint loadTexture(const std::string& path);
    static GLuint createBackgroundTexture(int width, int height);
    static GLuint createSDFTexture(int width, int height);
    static void updateTexture(GLuint texture, const unsigned char* data, int width, int height);
    static void bindTexture(GLuint texture, GLuint slot = 0);
    static void deleteTexture(GLuint texture);
    
    // Background generation utilities
    static std::vector<unsigned char> generateGradientBackground(int width, int height);
    static std::vector<unsigned char> generateCheckerboardBackground(int width, int height);
    static std::vector<unsigned char> generateSDFMap(int width, int height, float radius);
    
private:
    static void checkError(const std::string& operation);
};
#include "TextureLoader.h"
#include <iostream>
#include <stb_image.h>
#include <vector>
#include <algorithm>
#include <glm/glm.hpp>

using namespace glm;

GLuint TextureLoader::loadTexture(const std::string& path) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    
    int width, height, nrComponents;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cerr << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

GLuint TextureLoader::createBackgroundTexture(int width, int height) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    return textureID;
}

GLuint TextureLoader::createSDFTexture(int width, int height) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    return textureID;
}

void TextureLoader::updateTexture(GLuint texture, const unsigned char* data, int width, int height) {
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
}

void TextureLoader::bindTexture(GLuint texture, GLuint slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texture);
}

void TextureLoader::deleteTexture(GLuint texture) {
    glDeleteTextures(1, &texture);
}

std::vector<unsigned char> TextureLoader::generateGradientBackground(int width, int height) {
    std::vector<unsigned char> data(width * height * 3);
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float t = float(y) / float(height);
            
            // Deep purple to light purple gradient
            unsigned char r = static_cast<unsigned char>(89 + t * 100);
            unsigned char g = static_cast<unsigned char>(62 + t * 100);
            unsigned char b = static_cast<unsigned char>(200 + t * 55);
            
            int index = (y * width + x) * 3;
            data[index] = r;
            data[index + 1] = g;
            data[index + 2] = b;
        }
    }
    
    return data;
}

std::vector<unsigned char> TextureLoader::generateCheckerboardBackground(int width, int height) {
    std::vector<unsigned char> data(width * height * 3);
    
    int checkerSize = 32;
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            bool isEven = ((x / checkerSize) + (y / checkerSize)) % 2 == 0;
            
            unsigned char color = isEven ? 80 : 120;
            
            int index = (y * width + x) * 3;
            data[index] = color + 20;
            data[index + 1] = color;
            data[index + 2] = color + 40;
        }
    }
    
    return data;
}

std::vector<unsigned char> TextureLoader::generateSDFMap(int width, int height, float radius) {
    std::vector<unsigned char> data(width * height);
    
    vec2 center = vec2(width / 2.0f, height / 2.0f);
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            vec2 pos = vec2(x, y);
            float dist = length(pos - center) - radius;
            
            // Normalize to 0-255 range
            float normalized = std::max(0.0f, std::min(255.0f, (dist + 128.0f) / 256.0f * 255.0f));
            unsigned char value = static_cast<unsigned char>(normalized);
            
            data[y * width + x] = value;
        }
    }
    
    return data;
}

void TextureLoader::checkError(const std::string& operation) {
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL error after " << operation << ": " << error << std::endl;
    }
}
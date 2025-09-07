#pragma once

#include <GL/glew.h>
#include <string>

/**
 * @brief 纹理加载器类
 * 提供静态方法用于加载和删除OpenGL纹理
 */
class TextureLoader {
public:
    /**
     * @brief 从文件加载纹理
     * @param path 纹理文件路径
     * @return 返回加载的纹理ID，失败返回0
     */
    static GLuint loadTexture(const std::string& path);
    
    /**
     * @brief 删除纹理
     * @param texture 要删除的纹理ID
     */
    static void deleteTexture(GLuint texture);
};
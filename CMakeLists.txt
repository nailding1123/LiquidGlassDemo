cmake_minimum_required(VERSION 3.10)
project(LiquidGlassDemo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find packages
find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm CONFIG REQUIRED)

# Link vcpkg targets
if (TARGET glfw)
    set(GLFW_TARGET glfw)
elseif (TARGET glfw3)
    set(GLFW_TARGET glfw3)
endif()
if (NOT TARGET GLEW::GLEW)
    add_library(GLEW::GLEW ALIAS GLEW::glew)
endif()
if (TARGET glm::glm)
    set(GLM_TARGET glm::glm)
else()
    set(GLM_TARGET glm)
endif()

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/main.cpp
    src/LiquidGlass.cpp
    src/BackgroundCapture.cpp
    src/SDFGenerator.cpp
    src/BackgroundRenderer.cpp
    src/Camera.cpp
    src/Shader.cpp
    src/TextureLoader.cpp
    src/stb_image.cpp
)

# Header files
set(HEADERS
    include/LiquidGlass.h
    include/BackgroundCapture.h
    include/SDFGenerator.h
    include/BackgroundRenderer.h
    include/Camera.h
    include/Shader.h
    include/TextureLoader.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(${PROJECT_NAME}
    OpenGL::GL
    glfw
    GLEW::GLEW
    glm::glm
)

# Compiler flags
if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

# Copy shaders to build directory
file(COPY shaders DESTINATION ${CMAKE_BINARY_DIR})

# Set working directory for debugging
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
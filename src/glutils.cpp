#include "glutils.h"

#include <iostream>

// source: https://github.com/dormon/PGR_examples/blob/main/04_vertexArrays/src/main.cpp
GLuint createShader(GLenum type, std::string const& src) {
    auto id = glCreateShader(type);
    char const* srcs[] = { src.data() };
    glShaderSource(id, 1, srcs, nullptr);
    glCompileShader(id);

    // validate shader
    int success;
    char infoLog[512];
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLsizei len;
        glGetShaderInfoLog(id, 512, &len, infoLog);
        std::cerr << (type == GL_VERTEX_SHADER ? "VS " : "FS ") << "FAIL: " << infoLog << std::endl;
    }

    return id;
}

// source: same as above
GLuint createProgram(std::vector<GLuint>const& shaders) {
    auto id = glCreateProgram();
    for (auto const& shader : shaders) {
        glAttachShader(id, shader);
    }
    glLinkProgram(id);

    int success;
    char infoLog[512];
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
        GLsizei len = 0;
        glGetProgramInfoLog(id, 512, &len, infoLog);
        std::cerr << "FAIL: " << infoLog << std::endl;
    }
    return id;
}


glm::mat4 ComputeProjMatrix() {
    float aspect = (float)WIDTH / (float)HEIGHT;
    return glm::perspective(glm::half_pi<float>(), aspect, NEAR, FAR);
}

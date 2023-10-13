#pragma once

#include <GL/glew.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include <string>
#include <vector>


// window size
#define WIDTH 1024
#define HEIGHT 768

#define NEAR 0.01f
#define FAR 30.f


/**
*   @brief Function to compile a shader and validate it.
*
*/
GLuint createShader(GLenum type, std::string const& src);

GLuint createProgram(std::vector<GLuint>const& shaders);


/**
*  @brief Calculates the projection matrix.
*/
glm::mat4 ComputeProjMatrix();
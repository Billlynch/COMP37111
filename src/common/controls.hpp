#ifndef CONTROLS_HPP
#define CONTROLS_HPP
// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

void computeMatricesFromInputs(GLFWwindow *window);

void getMatrices(mat4 &projectionMatrix, mat4 &viewMatrix, vec3 &cameraPosition, mat4 &viewProjectionMatrix);

#endif
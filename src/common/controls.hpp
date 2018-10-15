#ifndef CONTROLS_HPP
#define CONTROLS_HPP

void computeMatricesFromInputs(GLFWwindow *window);
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();
void getMatrices(mat4 &projectionMatrix,mat4 &viewMatrix,vec3 &cameraPosition, mat4 &viewProjectionMatrix);

#endif

#include "controls.hpp"
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;
glm::vec3 position = glm::vec3(0, -20, 0);
glm::vec3 centerOfParticleSystem = glm::vec3(0.0f, 0.0f, 0.0f);

float camAngleX = 60, camAngleY = 15;
float distanceFromCentre = -230.0f;
float initialFoV = 45.0f;

void getMatrices(mat4 &projectionMatrix, mat4 &viewMatrix, vec3 &cameraPosition, mat4 &viewProjectionMatrix) {
    projectionMatrix = ProjectionMatrix;
    viewMatrix = ViewMatrix;
    viewProjectionMatrix = ProjectionMatrix * ViewMatrix;
    cameraPosition = inverse(ViewMatrix)[3];
}


void computeMatricesFromInputs(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camAngleX--;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camAngleX++;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camAngleY--;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camAngleY++;
    }

    if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS) {
        distanceFromCentre--;
    }

    if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) {
        distanceFromCentre++;
    }

    position.x = distanceFromCentre * -sinf(camAngleX * (M_PI/180)) * cosf(camAngleY * (M_PI/180) );
    position.y = -distanceFromCentre * cosf(camAngleX * (M_PI/180)) * cosf(camAngleY * (M_PI/180) );
    position.z = distanceFromCentre * -sinf(camAngleY * (M_PI/180));

    glm::vec3 direction = centerOfParticleSystem - position;

    glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);

    float FoV = initialFoV;

    // Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 10000.0f);
    ViewMatrix = glm::lookAt(position, centerOfParticleSystem, up);
}
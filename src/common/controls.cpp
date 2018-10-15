// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "controls.hpp"

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix(){
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
	return ProjectionMatrix;
}

void getMatrices(mat4 &projectionMatrix,mat4 &viewMatrix,vec3 &cameraPosition, mat4 &viewProjectionMatrix)
{
	projectionMatrix = ProjectionMatrix;
	viewMatrix = ViewMatrix;
	viewProjectionMatrix = ProjectionMatrix * ViewMatrix;
	cameraPosition = inverse(ViewMatrix)[3];
}

// Initial position
glm::vec3 position = glm::vec3( 0, -20, 0);
float initialFoV = 45.0f;

glm::vec3 centerOfParticleSystem = glm::vec3(0.0f, 0.0f, 0.0f);

void computeMatricesFromInputs(GLFWwindow *window){
//	glm::vec3 direction = glm::vec3(0.0f,1.0f,0.0f);

	if (glfwGetKey(window, GLFW_KEY_RIGHT ) == GLFW_PRESS)
	{
		position.x++;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT ) == GLFW_PRESS)
	{
		position.x--;
	}

	glm::vec3 direction = centerOfParticleSystem - position;

	glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);

	float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

	// Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	ViewMatrix = glm::lookAt(position, position+direction, up);
}
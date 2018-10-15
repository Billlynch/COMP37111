#define GLM_ENABLE_EXPERIMENTAL

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <GL/glew.h>

#include <GLFW/glfw3.h>
GLFWwindow* window;


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
using namespace glm;

#include <fstream>
#include <string>
#include <sstream>
#include "common/shader.hpp"
#include "common/controls.hpp"
#include "common/file_loader.h"

// CPU representation of a particle
struct Particle{
	glm::vec3 pos, speed, target;
	unsigned char r,g,b,a; // Color
	int n_bounces;
	float size, angle, weight;
	float life; // Remaining life of the particle. if <0 : dead and unused.
	float cameradistance; // *Squared* distance to the camera. if dead : -1.0f

	bool operator<(const Particle& that) const {
		// Sort in reverse order : far particles drawn first.
		return this->cameradistance > that.cameradistance;
	}
};

bool spaceHeld = false;

void generateNewParticles(double delta);

int
simulateParticles(GLfloat *g_particule_position_size_data, GLubyte *g_particule_color_data, double delta,
                  const vec3 &CameraPosition);

void setupVertexShaderInputs(GLuint billboard_vertex_buffer, GLuint particles_position_buffer,
                             GLuint particles_color_buffer);

void
generateBuffers(GLuint &base_mesh_vertex_buffer, GLuint &particles_position_buffer, GLuint &particles_color_buffer);

const int MaxParticles = 5000;
Particle ParticlesContainer[MaxParticles];
int LastUsedParticle = 0;

// Finds a Particle in ParticlesContainer which isn't used yet.
// (i.e. life < 0);
int FindUnusedParticle(){

	for(int i=LastUsedParticle; i<MaxParticles; i++){
		if (ParticlesContainer[i].life < 0){
			LastUsedParticle = i;
			return i;
		}
	}

	for(int i=0; i<LastUsedParticle; i++){
		if (ParticlesContainer[i].life < 0){
			LastUsedParticle = i;
			return i;
		}
	}

	return 0; // All particles are taken, override the first one
}

void SortParticles(){
	std::sort(&ParticlesContainer[0], &ParticlesContainer[MaxParticles]);
}

int height, width;
float floorYVal;
vec3 gravity;


std::vector<glm::vec3> objVectors = std::vector<glm::vec3>();
int main()
{
    std::string fileName = "/Users/Bill/ClionProjects/Graphics/src/Models/teapot.obj";

    file_loader* FileLoader = new file_loader;
    FileLoader->readObjFile(fileName, objVectors);
    delete FileLoader;

    width = 768;
    height = 1024;
    gravity = vec3(0.0f,0.0f, -9.81f);
    floorYVal = -7.0f;

    // Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( height, width, "Graphics", NULL, NULL);
	if( window == nullptr ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = static_cast<GLboolean>(true); // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	printf("openGL version: %s \n", glGetString(GL_VERSION));

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, height/2, width/2);

	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);


	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "/Users/Bill/ClionProjects/Graphics/src/shaders/vertex_shader.glsl", "/Users/Bill/ClionProjects/Graphics/src/shaders/fragment_shader.glsl" );

	// Vertex shader
	GLint CameraRight_worldspace_ID  = glGetUniformLocation(programID, "CameraRight_worldspace");
	GLint CameraUp_worldspace_ID  = glGetUniformLocation(programID, "CameraUp_worldspace");
	GLint ViewProjMatrixID = glGetUniformLocation(programID, "VP");


	
	static GLfloat* particle_position_size_data = new GLfloat[MaxParticles * 4];
	static GLubyte* particle_colour_data = new GLubyte[MaxParticles * 4];

	// make all particles dead and behind the camera
	for(int i=0; i<MaxParticles; i++){
		ParticlesContainer[i].life = -1.0f;
		ParticlesContainer[i].cameradistance = -1.0f;
	}

	// set up the buffers
    GLuint base_mesh_vertex_buffer;
    GLuint particles_position_buffer;
    GLuint particles_color_buffer;
    generateBuffers(base_mesh_vertex_buffer, particles_position_buffer, particles_color_buffer);


    double lastTime = glfwGetTime();
	do
	{
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		double currentTime = glfwGetTime();
		double delta = currentTime - lastTime;
		lastTime = currentTime;


		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
		glm::vec3 CameraPosition(glm::inverse(ViewMatrix)[3]);
		glm::mat4 ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;


		if (!spaceHeld)
        {
            generateNewParticles(delta);
        }

        int ParticlesCount = simulateParticles(particle_position_size_data, particle_colour_data, delta, CameraPosition);

        SortParticles();


		glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
		glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), nullptr, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
		glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLfloat) * 4, particle_position_size_data);

		glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
		glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), nullptr, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
		glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLubyte) * 4, particle_colour_data);


		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Use our shader
		glUseProgram(programID);

		glUniform3f(CameraRight_worldspace_ID, ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
		glUniform3f(CameraUp_worldspace_ID   , ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
		glUniformMatrix4fv(ViewProjMatrixID, 1, GL_FALSE, &ViewProjectionMatrix[0][0]);


        setupVertexShaderInputs(base_mesh_vertex_buffer, particles_position_buffer, particles_color_buffer);


		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, ParticlesCount);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

        spaceHeld = glfwGetKey(window, GLFW_KEY_SPACE ) == GLFW_PRESS;

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );


	delete[] particle_position_size_data;

	// Cleanup VBO and shader
	glDeleteBuffers(1, &particles_color_buffer);
	glDeleteBuffers(1, &particles_position_buffer);
	glDeleteBuffers(1, &base_mesh_vertex_buffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);
	

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

void
generateBuffers(GLuint &base_mesh_vertex_buffer, GLuint &particles_position_buffer, GLuint &particles_color_buffer) {// The VBO containing the base mesh for all the particles at the moment this is a square
    static const GLfloat g_vertex_buffer_data[] = {
		 -0.5f, -0.5f, 0.0f,
		  0.5f, -0.5f, 0.0f,
		 -0.5f,  0.5f, 0.0f,
		  0.5f,  0.5f, 0.0f,
	};
    glGenBuffers(1, &base_mesh_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, base_mesh_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    // The VBO containing the positions and sizes of the particles
    glGenBuffers(1, &particles_position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), nullptr, GL_STREAM_DRAW);

    // The VBO containing the colors of the particles
    glGenBuffers(1, &particles_color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), nullptr, GL_STREAM_DRAW);
}

void setupVertexShaderInputs(GLuint billboard_vertex_buffer, GLuint particles_position_buffer,
                             GLuint particles_color_buffer) {// 1st attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    // 2nd attribute buffer : positions of particles' centers
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

    // 3rd attribute buffer : particles' colors
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, nullptr);


    glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
    glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
    glVertexAttribDivisor(2, 1); // color : one per quad                                  -> 1
}

int
simulateParticles(GLfloat *g_particule_position_size_data, GLubyte *g_particule_color_data, double delta,
                  const vec3 &CameraPosition) {// Simulate all particles
    int ParticlesCount = 0;

    for(int i=0; i<MaxParticles; i++){

			Particle& p = ParticlesContainer[i]; // shortcut

			if(p.life > 0.0f){

                float sp = 15.5f;

                // Decrease life
				if (!spaceHeld)
                {
				    p.life -= delta;
                }
				if (p.life > 0.0f){

                    glm::vec3 dir;
				    // are we hitting the floor?
				    if (p.pos.z <= floorYVal)
                    {
				        dir = vec3((rand() % 3) - 2 , 0, 10.0f / p.n_bounces);
				        p.n_bounces++;
                        sp = 100.0f;
                    }
                    else
                    {
                        dir = gravity;
                        if (spaceHeld)
                        {
                            dir = (p.target - p.pos);
                            sp = sp * 100 / glm::distance(p.pos, p.target);
                        }
                    }
                    p.speed =  dir * (float)delta * sp;
                    p.pos += p.speed * (float)delta;
                    p.cameradistance = length2( p.pos - CameraPosition );

					// Fill the GPU buffer
					g_particule_position_size_data[4*ParticlesCount+0] = p.pos.x;
					g_particule_position_size_data[4*ParticlesCount+1] = p.pos.y;
					g_particule_position_size_data[4*ParticlesCount+2] = p.pos.z;

					g_particule_position_size_data[4*ParticlesCount+3] = p.size;

					g_particule_color_data[4*ParticlesCount+0] = p.r;
					g_particule_color_data[4*ParticlesCount+1] = p.g;
					g_particule_color_data[4*ParticlesCount+2] = p.b;
					g_particule_color_data[4*ParticlesCount+3] = p.a;

				}else{
					// Particles that just died will be put at the end of the buffer in SortParticles();
					p.cameradistance = -1.0f;
				}

				ParticlesCount++;

			}
		}
    return ParticlesCount;
}

void generateNewParticles(double delta) {
    int newparticles = (int)(delta*100.0);
    if (newparticles > (int)(0.016f*100.0)) // every ms
			newparticles = (int)(100.0);

    for(int i=0; i<newparticles; i++){
			int particleIndex = FindUnusedParticle();
			ParticlesContainer[particleIndex].life = 10.0f;
			ParticlesContainer[particleIndex].pos = vec3(rand() % 20 - 10.0f,rand() % 4,10);

			vec3 initialVecocity = vec3(0.0f, 0.0f, 0.0f);
			ParticlesContainer[particleIndex].speed = initialVecocity;
            ParticlesContainer[particleIndex].target = objVectors[rand() % objVectors.size()];

            ParticlesContainer[particleIndex].n_bounces = 1;



        // give each particle a random purple colour
			ParticlesContainer[particleIndex].r = rand() % 255;
			ParticlesContainer[particleIndex].g = 0;
			ParticlesContainer[particleIndex].b = 255;
			ParticlesContainer[particleIndex].a = 255;

			ParticlesContainer[particleIndex].size = (rand()%100)/2000.0f + 0.1f;
//			ParticlesContainer[particleIndex].size = 1.1f;

		}
}


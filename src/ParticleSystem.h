//
// Created by Bill Lynch on 15/10/2018.
//

#ifndef GRAPHICS_PARTICLESYSTEM_H
#define GRAPHICS_PARTICLESYSTEM_H
#define GLM_ENABLE_EXPERIMENTAL

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtx/norm.hpp>
#include <vector>
#include <glm/glm.hpp>
#include <string>
#include "common/Particle.h"
#include "common/file_loader.h"
#include "common/shader.hpp"
#include "common/controls.hpp"
#include "Analysiser.h"
#include <algorithm>


using namespace glm;

const long MaxParticles = 100000;

class ParticleSystem {
private:
    GLFWwindow* window;
    int height = 768;
    int width = 1024;
    float floorZVal = -7.0f;
    vec3 gravity = vec3(0.0f,0.0f, -9.81f);
    std::vector<vec3*> objVectors;
    Particle particlesContainer[MaxParticles];
    bool spaceHeld = false;
    float delta;
    double lastTime;
    int lastUsedParticle = 0;
    mat4 ProjectionMatrix, ViewMatrix, ViewProjectionMatrix;
    vec3 CameraPosition;
    Analysiser *analysiser;
    int nParticlesToRender;

    void mainLoop();

    void generateBuffers(GLuint &base_mesh_vertex_buffer,
                         GLuint &particles_position_buffer,
                         GLuint &particles_color_buffer);

    int FindUnusedParticle();
    void SortParticles();
    void generateNewParticles();
    void setupVertexShaderInputs(GLuint billboard_vertex_buffer,
                                 GLuint particles_position_buffer,
                                 GLuint particles_color_buffer);
    int setupGLEWandGLFW();
public:
    ParticleSystem() {};
    ~ParticleSystem() {};

    GLuint programID;
    GLint CameraRight_worldspace_ID, CameraUp_worldspace_ID, ViewProjMatrixID;

    GLfloat* particle_position_size_data = new GLfloat[MaxParticles * 4];
    GLubyte* particle_colour_data = new GLubyte[MaxParticles * 4];
    GLuint particles_color_buffer, VertexArrayID, particles_position_buffer, base_mesh_vertex_buffer;

    void runParticleSystem(std::string &fileName);

    int simulateParticles(GLfloat *g_particule_position_size_data, GLubyte *g_particule_color_data, double delta,
                      const vec3 &CameraPosition);

    void loadObj(std::string &fileName);

    void cleanUpBuffers() const;

    bool isWindowClosed();

    void setupBuffers();

    void setupShaders();

    void calculateDelta();

    void loadDataIntoBuffers(int particlesCount) const;
};


#endif //GRAPHICS_PARTICLESYSTEM_H

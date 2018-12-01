//
// Created by Bill Lynch on 15/10/2018.
//

#include "ParticleSystem.h"
#include "Analysiser.h"

void ParticleSystem::runParticleSystem(std::string &fileName) {

    loadObj(fileName);

    analysiser = new Analysiser("/Users/Bill/ClionProjects/Graphics/src/testOfAnalysiser.txt", objVectors.size());

    ParticleSystem::setupGLEWandGLFW();

    lastTime = glfwGetTime();

    do {
        ParticleSystem::mainLoop();
    } while(!isWindowClosed());

    cleanUpBuffers();

    delete analysiser;
}

bool ParticleSystem::isWindowClosed() {
    return (glfwGetKey(window, GLFW_KEY_ESCAPE ) == GLFW_PRESS && glfwWindowShouldClose(window) == 0) ||
            glfwWindowShouldClose(window) == 1;
}

void ParticleSystem::cleanUpBuffers() const {// Cleanup VBO and shader
    delete[] particle_position_size_data;

    glDeleteBuffers(1, &particles_color_buffer);
    glDeleteBuffers(1, &particles_position_buffer);
    glDeleteBuffers(1, &base_mesh_vertex_buffer);
    glDeleteProgram(programID);
    glDeleteVertexArrays(1, &VertexArrayID);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();
}

void ParticleSystem::loadObj(std::string &fileName) {
    auto FileLoader = new file_loader;
    FileLoader->readObjFile(fileName, objVectors);
    delete FileLoader;
}

int ParticleSystem::setupGLEWandGLFW() {
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
    window = glfwCreateWindow( width, height, "Graphics", nullptr, nullptr);
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

    std::cout << "openGL version: " << glGetString(GL_VERSION) << std::endl;

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetInputMode(window, GLFW_CURSOR, 0);
    glfwPollEvents();
    glfwSetCursorPos(window, height/2.0f, width/2.0f);

    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    setupShaders();
    setupBuffers();

    return 0;
}

void ParticleSystem::setupShaders() {
    programID = LoadShaders("/home/bill/Documents/git/COMP37111/src/shaders/vertex_shader.glsl", "/home/bill/Documents/git/COMP37111/src/shaders/fragment_shader.glsl" );
    CameraRight_worldspace_ID = glGetUniformLocation(programID, "CameraRight_worldspace");
    CameraUp_worldspace_ID = glGetUniformLocation(programID, "CameraUp_worldspace");
    ViewProjMatrixID = glGetUniformLocation(programID, "VP");

}

void ParticleSystem::setupBuffers() {
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    generateBuffers(base_mesh_vertex_buffer, particles_position_buffer, particles_color_buffer);
}

void ParticleSystem::mainLoop() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    calculateDelta();
//    analysiser->addData(delta, nParticlesToRender, spaceHeld);

    computeMatricesFromInputs(window);
    getMatrices(ProjectionMatrix, ViewMatrix, CameraPosition, ViewProjectionMatrix);

    if (!spaceHeld)
    {
        generateNewParticles();
    }

    int particlesCount = simulateParticles(particle_position_size_data, particle_colour_data, delta, CameraPosition);


    loadDataIntoBuffers(particlesCount);


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Use the shader that was loaded
    glUseProgram(programID);

    glUniform3f(CameraRight_worldspace_ID, ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
    glUniform3f(CameraUp_worldspace_ID   , ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
    glUniformMatrix4fv(ViewProjMatrixID, 1, GL_FALSE, &ViewProjectionMatrix[0][0]);

    setupVertexShaderInputs(base_mesh_vertex_buffer, particles_position_buffer, particles_color_buffer);

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, particlesCount);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();

    spaceHeld = glfwGetKey(window, GLFW_KEY_SPACE ) == GLFW_PRESS;
}

void ParticleSystem::loadDataIntoBuffers(int particlesCount) const {
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), nullptr, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
    glBufferSubData(GL_ARRAY_BUFFER, 0, particlesCount * sizeof(GLfloat) * 4, particle_position_size_data);

    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), nullptr, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
    glBufferSubData(GL_ARRAY_BUFFER, 0, particlesCount * sizeof(GLubyte) * 4, particle_colour_data);
}

void ParticleSystem::calculateDelta() {
    double currentTime = glfwGetTime();
    delta = static_cast<float>(currentTime - lastTime);
    lastTime = currentTime;
}

void ParticleSystem::generateBuffers(GLuint &base_mesh_vertex_buffer, GLuint &particles_position_buffer,
                                     GLuint &particles_color_buffer) {
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

int ParticleSystem::simulateParticles(GLfloat *g_particule_position_size_data, GLubyte *g_particule_color_data,
                                      double delta, const vec3 &CameraPosition) {
    nParticlesToRender = 0;

    for(int i=0; i<MaxParticles; i++){

        Particle& p = particlesContainer[i];

        p.simulate(delta, CameraPosition, spaceHeld, floorZVal, gravity);

        if(p.isAlive()) // if we need to render the particle then push to the GPU buffer
        {
            g_particule_position_size_data[4*nParticlesToRender+0] = p.getPos().x;
            g_particule_position_size_data[4*nParticlesToRender+1] = p.getPos().y;
            g_particule_position_size_data[4*nParticlesToRender+2] = p.getPos().z;

            g_particule_position_size_data[4*nParticlesToRender+3] = p.getSize();

            g_particule_color_data[4*nParticlesToRender+0] = p.getR();
            g_particule_color_data[4*nParticlesToRender+1] = p.getG();
            g_particule_color_data[4*nParticlesToRender+2] = p.getB();
            g_particule_color_data[4*nParticlesToRender+3] = p.getA();
            nParticlesToRender++;
        }


    }
    return nParticlesToRender;
}

void ParticleSystem::setupVertexShaderInputs(GLuint billboard_vertex_buffer, GLuint particles_position_buffer,
                                             GLuint particles_color_buffer) {
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

void ParticleSystem::generateNewParticles() {
    int newparticles = (int)(delta*1000.0);

    if (newparticles > (int)(0.016f*100.0)) // limit to 100 if running at a low fps
    {
        newparticles = (int)(50.0); // number of new particles to create
    }// every ms

//    int newparticles = 1000;

    for(int i=0; i<newparticles; i++){
        int particleIndex = FindUnusedParticle();

        vec3 pos = vec3(rand() % 20 - 10.0f,rand() % 10,10);
        vec3 speed = vec3(0.0f, 0.0f, 0.0f);

        Particle *p = new Particle(
                                pos,                                        // position
                                speed,                                      // speed
                                objVectors[rand() % objVectors.size()],     // target position
                                rand() % 10 + 1.0f,                                // mass
                                static_cast<unsigned char>(rand() % 255),   // R
                                static_cast<unsigned char>(0),              // G
                                static_cast<unsigned char>(255),            // B
                                static_cast<unsigned char>(255),            // A
                                (rand()%100)/2000.0f + 0.1f,                  // size
                                10.0f);                                     // life

        particlesContainer[particleIndex] = *p;
    }
}

int ParticleSystem::FindUnusedParticle() {
    for(int i=lastUsedParticle; i<MaxParticles; i++){
        if (particlesContainer[i].getLife() < 0){
            lastUsedParticle = i;
            return i;
        }
    }

    for(int i=0; i<lastUsedParticle; i++){
        if (particlesContainer[i].getLife() < 0){
            lastUsedParticle = i;
            return i;
        }
    }

    return 0; // All particles are taken, override the first one
}

void ParticleSystem::SortParticles() {
    std::sort(&particlesContainer[0], &particlesContainer[MaxParticles]);
}

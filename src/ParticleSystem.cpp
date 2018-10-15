//
// Created by Bill Lynch on 15/10/2018.
//

#include "ParticleSystem.h"

void ParticleSystem::runParticleSystem(std::string &fileName) {
    auto FileLoader = new file_loader;
    FileLoader->readObjFile(fileName, objVectors);
    delete FileLoader;

    ParticleSystem::setupGLEWandGLFW();

    lastTime = glfwGetTime();
    do {
        ParticleSystem::mainLoop();
    } while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

    delete[] particle_position_size_data;

    // Cleanup VBO and shader
    glDeleteBuffers(1, &particles_color_buffer);
    glDeleteBuffers(1, &particles_position_buffer);
    glDeleteBuffers(1, &base_mesh_vertex_buffer);
    glDeleteProgram(programID);
    glDeleteVertexArrays(1, &VertexArrayID);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();
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

    // Create and compile our GLSL program from the shaders
    programID = LoadShaders( "/Users/Bill/ClionProjects/Graphics/src/shaders/vertex_shader.glsl", "/Users/Bill/ClionProjects/Graphics/src/shaders/fragment_shader.glsl" );


    // buffer setup -- refactor
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Vertex shader
    CameraRight_worldspace_ID  = glGetUniformLocation(programID, "CameraRight_worldspace");
    CameraUp_worldspace_ID  = glGetUniformLocation(programID, "CameraUp_worldspace");
    ViewProjMatrixID = glGetUniformLocation(programID, "VP");

    particle_position_size_data = new GLfloat[MaxParticles * 4];
    particle_colour_data = new GLubyte[MaxParticles * 4];



    // set up the buffers
    generateBuffers(base_mesh_vertex_buffer, particles_position_buffer, particles_color_buffer);


    return 0;
}

void ParticleSystem::mainLoop() {
// Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    double currentTime = glfwGetTime();
    delta = static_cast<float>(currentTime - lastTime);
    lastTime = currentTime;


    computeMatricesFromInputs(window);
    glm::mat4 ProjectionMatrix = getProjectionMatrix();
    glm::mat4 ViewMatrix = getViewMatrix();
    glm::vec3 CameraPosition(glm::inverse(ViewMatrix)[3]);
    glm::mat4 ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;


    if (!spaceHeld)
    {
        generateNewParticles();
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
    int ParticlesCount = 0;

    for(int i=0; i<MaxParticles; i++){

        Particle& p = particlesContainer[i];

        p.simulate(delta, CameraPosition, spaceHeld, floorZVal, gravity);

        if(p.getLife() > 0.0f) // if we need to render the particle then push to the GPU buffer
        {
            g_particule_position_size_data[4*ParticlesCount+0] = p.getPos().x;
            g_particule_position_size_data[4*ParticlesCount+1] = p.getPos().y;
            g_particule_position_size_data[4*ParticlesCount+2] = p.getPos().z;

            g_particule_position_size_data[4*ParticlesCount+3] = p.getSize();

            g_particule_color_data[4*ParticlesCount+0] = p.getR();
            g_particule_color_data[4*ParticlesCount+1] = p.getG();
            g_particule_color_data[4*ParticlesCount+2] = p.getB();
            g_particule_color_data[4*ParticlesCount+3] = p.getA();
        }

        ParticlesCount++;

    }
    return ParticlesCount;
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
    int newparticles = (int)(delta*100.0);

    if (newparticles > (int)(0.016f*100.0)) // limit to 100 if running at a low fps
    {
        newparticles = (int)(100.0); // number of new particles to create
    }// every ms


    for(int i=0; i<newparticles; i++){
        int particleIndex = FindUnusedParticle();

        vec3 pos = vec3(rand() % 20 - 10.0f,rand() % 4,10);
        vec3 speed = vec3(0.0f, 0.0f, 0.0f);

        Particle *p = new Particle(
                                pos,                                        // position
                                speed,                                      // speed
                                objVectors[rand() % objVectors.size()],     // target position
                                static_cast<unsigned char>(rand() % 255),   // R
                                static_cast<unsigned char>(0),              // G
                                static_cast<unsigned char>(255),            // B
                                static_cast<unsigned char>(255),            // A
                                (rand()%100)/2000.0f + 0.1f,                // size
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

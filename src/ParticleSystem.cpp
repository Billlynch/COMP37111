//
// Created by Bill Lynch on 15/10/2018.
//

#include "ParticleSystem.h"

void ParticleSystem::runParticleSystem(std::string &fileName) {

    loadObj(fileName);

    analyser = new Analyser("/Users/Bill/ClionProjects/Graphics/src/testOfAnalysiser.txt", objVectors.size());

    ParticleSystem::setupGLEWandGLFW();

    generateNewParticles();

#ifndef __APPLE__
    setupOpenCl();
#endif

    lastTime = glfwGetTime();


    do {
        ParticleSystem::mainLoop();
    } while (!isWindowClosed());

    cleanUpBuffers();

    delete analyser;
}

bool ParticleSystem::isWindowClosed() {
    return (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && glfwWindowShouldClose(window) == 0) ||
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
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow(width, height, "Graphics", nullptr, nullptr);
    if (window == nullptr) {
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
    glfwSetCursorPos(window, height / 2.0f, width / 2.0f);

    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    setupShaders();
    setupBuffers();

    return 0;
}

void ParticleSystem::setupShaders() {
    programID = LoadShaders("./src/shaders/vertex_shader.glsl",
                            "./src/shaders/fragment_shader.glsl");

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

    calculateFrameDelta();  // frame init time
    frameCount++;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    computeMatricesFromInputs(window);
    getMatrices(ProjectionMatrix, ViewMatrix, CameraPosition, ViewProjectionMatrix);

    prePhysicsTime = glfwGetTime();

    simParticlesOpenCL();
    //simParticles();

    postPhysicsTime = glfwGetTime();

    loadDataIntoBuffers();

    postLoadIntoOpenGLBuffersTime = glfwGetTime();


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Use the shader that was loaded
    glUseProgram(programID);

    glUniform3f(CameraRight_worldspace_ID, ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
    glUniform3f(CameraUp_worldspace_ID, ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
    glUniformMatrix4fv(ViewProjMatrixID, 1, GL_FALSE, &ViewProjectionMatrix[0][0]);

    setupVertexShaderInputs(base_mesh_vertex_buffer, particles_position_buffer, particles_color_buffer);

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, NUM_PARTICLES);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    postDrawCallTime = glfwGetTime();


    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();

    spaceHeld = new bool(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);
    glFlush();

    analyser->addData(frameCount, delta, postPhysicsTime - prePhysicsTime,postLoadIntoOpenGLBuffersTime - postPhysicsTime, postDrawCallTime - postLoadIntoOpenGLBuffersTime, spaceHeld);
}

void ParticleSystem::loadDataIntoBuffers() const {
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    glBufferData(GL_ARRAY_BUFFER, NUM_PARTICLES * 4 * sizeof(float), nullptr, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, NUM_PARTICLES * sizeof(float) * 4, particle_position_size_data);

    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    glBufferData(GL_ARRAY_BUFFER, NUM_PARTICLES * 4 * sizeof(GLubyte), nullptr, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, NUM_PARTICLES * sizeof(GLubyte) * 4, particle_colour_data);
}

void ParticleSystem::calculateFrameDelta() {
    frameInitTime = glfwGetTime();
    delta = static_cast<float>(frameInitTime - lastTime);
    lastTime = frameInitTime;
}

void ParticleSystem::generateBuffers(GLuint &base_mesh_vertex_buffer, GLuint &particles_position_buffer,
                                     GLuint &particles_color_buffer) {
    static const GLfloat g_vertex_buffer_data[] = {
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            -0.5f, 0.5f, 0.0f,
            0.5f, 0.5f, 0.0f,
    };
    glGenBuffers(1, &base_mesh_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, base_mesh_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    // The VBO containing the positions and sizes of the particles
    glGenBuffers(1, &particles_position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    glBufferData(GL_ARRAY_BUFFER, NUM_PARTICLES * 4 * sizeof(float), nullptr, GL_STREAM_DRAW);

    // The VBO containing the colors of the particles
    glGenBuffers(1, &particles_color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    glBufferData(GL_ARRAY_BUFFER, NUM_PARTICLES * 4 * sizeof(GLubyte), nullptr, GL_STREAM_DRAW);
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

    for (int i = 0; i < NUM_PARTICLES; i++) {

        float life, size, x, y, z, randLow, randHigh;

        randHigh = 5.0f;
        randLow = -5.0f;
        life = randLow + rand() / (RAND_MAX / (randHigh - randLow)); //  (rand() % 10) - 5.0f;

        randHigh = 0.15f;
        randLow = 0.10f;
        size = randLow + rand() / (RAND_MAX / (randHigh - randLow)); //(rand()%100)/2000.0f + 0.1f;

        randHigh = 100.0f;
        randLow = -100.0f;
        x = randLow + rand() / (RAND_MAX / (randHigh - randLow));

        randHigh = 50.0f;
        randLow = -50.0f;
        y = randLow + rand() / (RAND_MAX / (randHigh - randLow));
        z = 30.0f;

        particle_position_size_data[(i * 4) + 0] = x;
        particle_position_size_data[(i * 4) + 1] = y;
        particle_position_size_data[(i * 4) + 2] = z;
        particle_position_size_data[(i * 4) + 3] = size;


        particleMetaDataBuffer[i] = life;

        particlesContainer[i].size = size;

        randHigh = 25.0f;
        randLow = 10.0f;
        particlesContainer[i].life = randLow + rand() / (RAND_MAX / (randHigh - randLow));
        particlesContainer[i].position.x = x;
        particlesContainer[i].position.y = y;
        particlesContainer[i].position.z = z;

        long pointToGoTo = rand() % objVectors.size();

        particlesContainer[i].target.x = objVectors[pointToGoTo]->x;;
        particlesContainer[i].target.y = objVectors[pointToGoTo]->y;;
        particlesContainer[i].target.z = objVectors[pointToGoTo]->z;;

        randHigh = 15.0f;
        randLow = 0.2f;
        particlesContainer[i].mass = randLow + rand() / (RAND_MAX / (randHigh - randLow));

        randHigh = 5.1f;
        randLow = -5.1f;
        particlesContainer[i].randX = randLow + rand() / (RAND_MAX / (randHigh - randLow));
        particlesContainer[i].randY = randLow + rand() / (RAND_MAX / (randHigh - randLow));

        particle_colour_data[(i * 4) + 0] = static_cast<unsigned char>(rand() % 255);
        particle_colour_data[(i * 4) + 1] = static_cast<unsigned char>(0);
        particle_colour_data[(i * 4) + 2] = static_cast<unsigned char>(255);
        particle_colour_data[(i * 4) + 3] = static_cast<unsigned char>(255);
    }

}
#ifndef __APPLE__
void ParticleSystem::setupOpenCl() {

    cl_int err;
    std::vector<cl::Platform> platformList;
    cl::Platform::get(&platformList);

    checkErr(!platformList.empty() ? CL_SUCCESS : -1, "cl::Platform::get");
    std::cerr << "Platform count is: " << platformList.size() << std::endl;


    cl_context_properties cprops[3] =
            {CL_CONTEXT_PLATFORM, (cl_context_properties) (platformList[0])(), 0};

    cl::Context context(
            CL_DEVICE_TYPE_GPU,
            cprops,
            nullptr,
            nullptr,
            &err);
    checkErr(err, "Conext::Context()");


    kernelParticleBuffer = cl::Buffer(
            context,
            CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
            sizeof(particle) * NUM_PARTICLES,
            particlesContainer,
            &err);
    checkErr(err, "Buffer::Buffer()");


    kernelParticleBufferToOpenGl = cl::Buffer(
            context,
            CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
            sizeof(float) * NUM_PARTICLES * 4,
            particle_position_size_data,
            &err);
    checkErr(err, "Buffer::Buffer()");

    kernelParticleMetaBuffer = cl::Buffer(
            context,
            CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
            sizeof(float) * NUM_PARTICLES,
            particleMetaDataBuffer,
            &err);
    checkErr(err, "Buffer::Buffer()");

    kernelSpaceBuffer = cl::Buffer(
            context,
            CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
            sizeof(bool),
            spaceHeld,
            &err);
    checkErr(err, "Buffer::Buffer()");


    std::vector<cl::Device> devices;
    devices = context.getInfo<CL_CONTEXT_DEVICES>();
    checkErr(!devices.empty() ? CL_SUCCESS : -1, "devices.size() > 0");


    // ----------------------------------------------------------------------------------- physics kernel
    std::ifstream file("/home/bill/Documents/git/COMP37111/src/Kernel/simulate_particle.cl");
    checkErr(file.is_open() ? CL_SUCCESS : -1, "simulate_particle.cl");
    std::string prog(
            std::istreambuf_iterator<char>(file),
            (std::istreambuf_iterator<char>()));

    cl::Program::Sources source(1, std::make_pair(prog.c_str(), prog.length() + 1));

    cl::Program program(context, source);
    err = program.build(devices, "");
    checkErr(file.is_open() ? CL_SUCCESS : -1, "Program::build()");

    kernel = cl::Kernel(program, "simulate_particle", &err);
    checkErr(err, "Kernel::Kernel()");


    // ----------------------------------------------------------------------------------- physics kernel

    queue = cl::CommandQueue(context, devices[0], 0, &err);
    checkErr(err, "CommandQueue::CommandQueue()");

}
#endif

void ParticleSystem::simParticles() {

    for (int i = 0; i < NUM_PARTICLES; ++i) {
        if (particlesContainer[i].life > 0.0) {
            vect3 acc;
            acc.x = 0.0;
            acc.y = 0.0;
            acc.z = 0.0;
            double speed_multiplier = 5.5f;

            if (particle_position_size_data[(i * 4) + 2] <= FLOOR_Z) {
                particlesContainer[i].position.z = static_cast<float>(FLOOR_Z + 0.1f);
                acc.x = particlesContainer[i].randX * 0.0016f;
                acc.y = particlesContainer[i].randY * 0.0016f;
                acc.z = 5.0f * 0.0016f;
            } else {
                if (*spaceHeld) {
                    acc.x = (particlesContainer[i].target.x - particle_position_size_data[(i * 4) + 0]) * 0.016f;
                    acc.y = (particlesContainer[i].target.y - particle_position_size_data[(i * 4) + 1]) * 0.016f;
                    acc.z = (particlesContainer[i].target.z - particle_position_size_data[(i * 4) + 2]) * 0.016f;

                } else {
                    acc.z = static_cast<float>(GRAVITY * 0.0016f);
                }

            }

            float sx = acc.x * particlesContainer[i].mass;
            float sy = acc.y * particlesContainer[i].mass;
            float sz = acc.z * particlesContainer[i].mass;

            particle_position_size_data[(i * 4) + 0] += sx;
            particle_position_size_data[(i * 4) + 1] += sy;
            particle_position_size_data[(i * 4) + 2] += sz;


            if (!*spaceHeld) {
                particlesContainer[i].life -= 0.01;
            }
        } else {


            if (particlesContainer[i].life < -1.0 && !*spaceHeld) {
                particle_position_size_data[(i * 4) + 0] = particlesContainer[i].position.x;
                particle_position_size_data[(i * 4) + 1] = particlesContainer[i].position.y;
                particle_position_size_data[(i * 4) + 2] = particlesContainer[i].position.z;
                particle_position_size_data[(i * 4) + 3] = particlesContainer[i].size;

                particlesContainer[i].life = 50.0f;
            } else {
                particle_position_size_data[(i * 4) + 0] = -50.0f;
                particle_position_size_data[(i * 4) + 1] = -50.0f;
                particle_position_size_data[(i * 4) + 2] = -50.0f;

                particle_position_size_data[(i * 4) + 3] = -1.0f;
                particlesContainer[i].life -= 0.01;
            }
        }
    }

}
#ifndef __APPLE__
void ParticleSystem::simParticlesOpenCL() {
    cl_int err;


    err = queue.enqueueWriteBuffer(
            kernelSpaceBuffer,
            CL_TRUE,
            0,
            sizeof(bool),
            spaceHeld);
    checkErr(err, "ComamndQueue::enqueueReadBuffer()");


    err = kernel.setArg(0, kernelParticleBuffer);
    checkErr(err, "Kernel::setArg()");
    err = kernel.setArg(1, kernelParticleBufferToOpenGl);
    checkErr(err, "Kernel::setArg()");
    err = kernel.setArg(2, kernelParticleMetaBuffer);
    checkErr(err, "Kernel::setArg()");
    err = kernel.setArg(3, kernelSpaceBuffer);
    checkErr(err, "Kernel::setArg()");


    err = queue.enqueueNDRangeKernel(
            kernel,
            cl::NullRange,
            cl::NDRange(NUM_PARTICLES),
            cl::NDRange(128),
            nullptr,
            &event);
    checkErr(err, "ComamndQueue::enqueueNDRangeKernel()");

    err = queue.enqueueReadBuffer(
            kernelParticleBufferToOpenGl,
            CL_TRUE,
            0,
            sizeof(float) * NUM_PARTICLES * 4,
            particle_position_size_data);
    checkErr(err, "ComamndQueue::enqueueReadBuffer()");

    event.wait();
}
#endif

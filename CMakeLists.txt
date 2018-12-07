cmake_minimum_required(VERSION 3.3)
project(Graphics)

set(CMAKE_MODULE_PATH ./cmake)

find_package(OpenGL REQUIRED)
find_package(OpenCL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework Cocoa -framework OpenGL -framework IOKit")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")

set(SOURCE_FILES CMakeLists.txt)

# add extra include directories
include_directories(/usr/local/include)
include_directories(/usr/include)

# add extra lib directories
link_directories(/usr/local/lib)
link_directories(/usr/lib)

add_executable(Graphics
        src/main.cpp
        src/common/controls.cpp
        src/common/controls.hpp
        src/common/shader.cpp
        src/common/shader.hpp
        src/common/file_loader.cpp
        src/common/file_loader.h
        src/ParticleSystem.cpp
        src/ParticleSystem.h
        src/common/Particle.h
        src/Analyser.h
        src/Analyser.cpp)


set(INCLUDE_DIRS ${INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS} ${GLM_INCLUDE_DIR} ${OpenCL_INCLUDE_DIR})
set(LIBS ${LIBS} ${OPENGL_LIBRARY} ${GLEW_LIBRARY} ${OpenCL_LIBRARY} glfw)
include_directories(${INCLUDE_DIRS})

target_link_libraries(Graphics ${LIBS})

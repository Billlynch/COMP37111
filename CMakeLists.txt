cmake_minimum_required(VERSION 3.3)
project(Graphics)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework Cocoa -framework OpenGL -framework IOKit")

set(SOURCE_FILES CMakeLists.txt)

# add extra include directories
include_directories(/usr/local/include)

# add extra lib directories
link_directories(/usr/local/lib)

add_executable(Graphics src/main.cpp src/common/controls.cpp src/common/controls.hpp src/common/shader.cpp src/common/shader.hpp src/common/texture.cpp src/common/texture.hpp)
target_link_libraries(Graphics glfw)
target_link_libraries(Graphics glew)
find_package (GLM REQUIRED)
include_directories(include)
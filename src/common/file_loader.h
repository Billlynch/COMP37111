//
// Created by Bill Lynch on 15/10/2018.
//

#ifndef GRAPHICS_FILE_LOADER_H
#define GRAPHICS_FILE_LOADER_H

#include <sstream>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <fstream>


class file_loader {
private:
    float xVal{}, yVal{}, zVal{};
    std::string line{},id{};


public:
    file_loader() = default;

    ~file_loader() = default;

    int readObjFile(std::string fileName, std::vector<glm::vec3 *> &vectorsOfObj);

};


#endif //GRAPHICS_FILE_LOADER_H

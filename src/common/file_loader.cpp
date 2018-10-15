//
// Created by Bill Lynch on 15/10/2018.
//

#include "file_loader.h"

int file_loader::readObjFile(std::string fileName, std::vector<glm::vec3> &vectorsOfObj) {
    std::ifstream infile(fileName);

    if (!infile.is_open())
    {
        std::cout << "Failed to open the OBJ file: " << fileName << std::endl;
        return 1;
    }

    while (std::getline(infile, line))
    {
        std::istringstream iss(line);

        iss >> id >> xVal >> yVal >> zVal;

        if (id == "v")  // only care about the vectors
        {
            glm::vec3 newVector = glm::vec3(xVal, zVal, yVal);
            vectorsOfObj.push_back(newVector);
        }
    }
    infile.close();
    return 0;
}

int file_loader::loadBILL(std::vector<glm::vec3> vectorsOfObj) {
    // B
    glm::vec3 B1 = glm::vec3(-6,0,2);
    glm::vec3 B2 = glm::vec3(-6,0,3);
    glm::vec3 B0 = glm::vec3(-6,0,1);
    glm::vec3 B3 = glm::vec3(-6,0,4);
    glm::vec3 B4 = glm::vec3(-6,0,5);
    glm::vec3 B5 = glm::vec3(-6,0,6);
    glm::vec3 B6 = glm::vec3(-6,0,7);
    glm::vec3 B7 = glm::vec3(-5,0,7);
    glm::vec3 B8 = glm::vec3(-4,0,6);
    glm::vec3 B9 = glm::vec3(-4,0,5);
    glm::vec3 B10 = glm::vec3(-5,0,4);
    glm::vec3 B11 = glm::vec3(-4,0,3);
    glm::vec3 B12 = glm::vec3(-4,0,2);
    glm::vec3 B13 = glm::vec3(-5,0,1);
    // I
    glm::vec3 I0 = glm::vec3(-1,0,1);
    glm::vec3 I1 = glm::vec3(-1,0,2);
    glm::vec3 I2 = glm::vec3(-1,0,3);
    glm::vec3 I3 = glm::vec3(-1,0,4);
    glm::vec3 I4 = glm::vec3(-1,0,6);
    // L
    glm::vec3 l10 = glm::vec3(1,0,1);
    glm::vec3 l11 = glm::vec3(1,0,2);
    glm::vec3 l12 = glm::vec3(1,0,3);
    glm::vec3 l13 = glm::vec3(1,0,4);
    glm::vec3 l14 = glm::vec3(1,0,5);
    glm::vec3 l15 = glm::vec3(1,0,6);
    // L
    glm::vec3 l20 = glm::vec3(3,0,1);
    glm::vec3 l21 = glm::vec3(3,0,2);
    glm::vec3 l22 = glm::vec3(3,0,3);
    glm::vec3 l23 = glm::vec3(3,0,4);
    glm::vec3 l24 = glm::vec3(3,0,5);
    glm::vec3 l25 = glm::vec3(3,0,6);

    vectorsOfObj = {B0, B1, B2, B3, B4, B5, B6, B7, B8, B9, B10, B11, B12, B13, // B
                    I0, I1, I2, I3, I4,                                         // I
                    l10, l11, l12, l13, l14, l15,                               // L
                    l20, l21, l22, l23, l24, l25};                              // L


    return 0;
}

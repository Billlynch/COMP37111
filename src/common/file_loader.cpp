//
// Created by Bill Lynch on 15/10/2018.
//

#include "file_loader.h"

int file_loader::readObjFile(std::string fileName, std::vector<glm::vec3 *> &vectorsOfObj) {
    std::ifstream infile(fileName);

    if (!infile.is_open()) {
        std::cout << "Failed to open the OBJ file: " << fileName << std::endl;
        return 1;
    }

    while (std::getline(infile, line)) {
        std::istringstream iss(line);

        iss >> id >> xVal >> yVal >> zVal;

        if (id == "v")  // only care about the vectors
        {
            auto *newVector = new glm::vec3(xVal, zVal, yVal);
            vectorsOfObj.push_back(newVector);
        }
    }
    infile.close();
    return 0;
}

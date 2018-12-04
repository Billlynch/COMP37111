//
// Created by Bill Lynch on 15/10/2018.
//

#ifndef GRAPHICS_ANALYSER_H
#define GRAPHICS_ANALYSER_H

#include <string>
#include <utility>
#include <fstream>
#include <iostream>


class Analyser {
private:
    std::string fileName;
    unsigned long nVertexInOBJ;
    std::ofstream outputFile;

public:
    Analyser(std::string fileName, unsigned long numberOfVertex) :
            fileName(std::move(fileName)),
            nVertexInOBJ(numberOfVertex) {
        outputFile.open("analysisResult.csv", std::ofstream::out);

        if (outputFile.is_open()) {
            std::cout << "correct" << std::endl;
        }
    };

    ~Analyser() {
        outputFile.close();
    }

    void addData(unsigned long frameCount, double frameDelta, double physicsDelta, double openGLPushDelta,
                 double drawCallDelta, bool *spaceHeld);

};


#endif //GRAPHICS_ANALYSER_H


/*
 * FILE LAYOUT
 *      1st number is the frame number
 *      2nd number is the frame delta
 *      3rd number is the physics delta
 *      4th number is the push data to openGL delta
 *      5th number is the draw call delta delta
 *      5th bool is tis space was held
 */

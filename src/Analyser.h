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

    void addData(float frameDelta, unsigned long n_particles, bool spaceHeld, float physicsDelta);

};


#endif //GRAPHICS_ANALYSER_H


/*
 * FILE LAYOUT
 *
 * 'ID <number> <number>'
 *
 * where ID is in:
 *  FR (frame rate)
 *      1st number is the number of particles
 *      2nd number is the frame delta
 */

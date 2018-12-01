//
// Created by Bill Lynch on 15/10/2018.
//

#ifndef GRAPHICS_ANALYSISER_H
#define GRAPHICS_ANALYSISER_H

#include <string>
#include <utility>
#include <fstream>
#include <iostream>


class Analysiser {
private:
    std::string fileName;
    unsigned long nVertexInOBJ;
    std::ofstream outputFile;

public:
    Analysiser(std::string fileName, unsigned long numberOfVertex):
        fileName(std::move(fileName)),
        nVertexInOBJ(numberOfVertex)
    {
        outputFile.open("analysisResult.csv", std::ofstream::out);

        if (outputFile.is_open())
        {
            std::cout << "correct" << std::endl;
        }
    };

    ~Analysiser() {
       outputFile.close();
    }

    void addData(float frameDelta, unsigned long n_particles, bool spaceHeld);

};


#endif //GRAPHICS_ANALYSISER_H


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

//
// Created by Bill Lynch on 15/10/2018.
//

#include "Analyser.h"

void Analyser::addData(float frameDelta, unsigned long n_particles, bool spaceHeld, float physicsDelta) {
    outputFile << n_particles << "," << frameDelta << "," << spaceHeld << "," << physicsDelta << std::endl;
}

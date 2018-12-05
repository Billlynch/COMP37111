#include <iostream>

#define NUM_PARTICLES 3008000
#include <string>
#include <utility>
#include <fstream>
#include <cstdlib>


float *lifes = new float[NUM_PARTICLES];
float *lifesReplacement = new float[NUM_PARTICLES];

long workOutNParticlesLivingAtNextFrame() {
    long countAlive = 0;
    for (int j = 0; j < NUM_PARTICLES; ++j) {
        lifes[j] = lifes[j] -0.01;
        if (lifes[j] > 0.0f) {
            countAlive++;
        } else if (lifes[j] < -1.0 ){
            lifes[j] = lifesReplacement[j];
        }
    }
    return countAlive;
}

int main(int argc, char *argv[]) {
    std::ofstream outputFile;
    std::string fileName;
    outputFile.open("particleAtFrameCount.csv", std::ofstream::out);
    if (outputFile.is_open()) {
        std::cout << "file opened" << std::endl;
    }

    // this is just to calculate how many particles are actually being rendered at any frame


    // get the 'random' life of each particle
    srand(2587837); // same seed as the actual run

    float life, z, randLow, randHigh;

    randHigh = 5.0f;
    randLow = -5.0f;

    // the first rand is the life - this is what I care about
    // 9 other
    for (int i = 0; i < NUM_PARTICLES; ++i) {
        float life = randLow + rand() / (RAND_MAX / (randHigh - randLow));
        lifes[i] = life;
        lifesReplacement[i] = life;

        z = rand();
        z = rand();
        z = rand();

        z = rand();
        z = rand();
        z = rand();

        z = rand();
        z = rand();
        z = rand();
    }

    for (int j = 0; j < 5500; ++j) {
        outputFile << j << "," << workOutNParticlesLivingAtNextFrame() << std::endl;
    }

    outputFile.close();

    return 0;
}

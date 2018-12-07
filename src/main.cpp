
#include "ParticleSystem.h"


int main(int argc, char *argv[]) {
    std::string file = "./src/Models/teapot.obj";

    auto *ps = new ParticleSystem();
    ps->runParticleSystem(file);

    delete ps;
    return 0;
}

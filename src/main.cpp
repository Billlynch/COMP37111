
#include "ParticleSystem.h"

int main(int argc, char* argv[])
{
    std::string file = "/home/bill/Documents/git/COMP37111/src/Models/teapot.obj";

    if (argc >= 2) {
        file = argv[1];
    }

    auto *ps = new ParticleSystem();
    ps->runParticleSystem(file);

    delete ps;
	return 0;
}

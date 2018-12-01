
#include "ParticleSystem.h"

int main(int argc, char* argv[])
{
    std::string file = "/Users/Bill/ClionProjects/Graphics/src/Models/teapot.obj"; // default make a teapot

    if (argc >= 2) {
        file = argv[1];
    }

    auto *ps = new ParticleSystem();

    ps->runParticleSystem(file);

    delete ps;
	return 0;
}

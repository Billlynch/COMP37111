
#include "ParticleSystem.h"




int main(int argc, char* argv[])
{
    std::string file = "/home/bill/Documents/git/COMP37111/src/Models/teapot.obj";

    auto *ps = new ParticleSystem();
    ps->runParticleSystem(file);

    delete ps;
	return 0;
}

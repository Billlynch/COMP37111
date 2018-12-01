
#include "ParticleSystem.h"

int main()
{
    ParticleSystem *ps = new ParticleSystem();

    std::string file = "/home/bill/Documents/git/COMP37111/src/Models/teapot.obj";
    ps->runParticleSystem(file);

    delete ps;

	return 0;
}

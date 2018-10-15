
#include "ParticleSystem.h"

int main()
{
    ParticleSystem *ps = new ParticleSystem();

    std::string file = "/Users/Bill/ClionProjects/Graphics/src/Models/teapot.obj";
    ps->runParticleSystem(file);

    delete ps;

	return 0;
}

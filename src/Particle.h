//
// Created by Bill Lynch on 01/10/2018.
//

#ifndef GRAPHICS_PARTICLE_H
#define GRAPHICS_PARTICLE_H

#include <glm/glm.hpp>

class Particle{
        glm::vec3 pos, speed;
        unsigned char r,g,b,a; // Color
        float size, angle, weight;
        float life; // Remaining life of the particle. if < 0 : dead and unused.
    };


#endif //GRAPHICS_PARTICLE_H

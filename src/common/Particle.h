//
// Created by Bill Lynch on 15/10/2018.
//

#ifndef GRAPHICS_PARTICLE_H
#define GRAPHICS_PARTICLE_H

typedef struct tag_vect3
{
    float x;
    float y;
    float z;
} vect3;

typedef struct tag_particle
{
    vect3 position;
    float size;
    vect3 target;
    float mass;
    vect3 speed;
    float life;
    float randX;
    float randY;
} particle;

#endif //GRAPHICS_PARTICLE_H

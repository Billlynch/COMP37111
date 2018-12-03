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
    float s;
    vect3 target;
    double mass;
    vect3 speed;
    float life;
    int n_bounces;
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} particle;

#endif //GRAPHICS_PARTICLE_H

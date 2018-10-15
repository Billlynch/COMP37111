//
// Created by Bill Lynch on 15/10/2018.
//

#include "Particle.h"

void Particle::simulate(double &delta, const vec3 &CameraPosition, bool &spaceHeld, float &floorZVal, vec3 &gravity) {
    if(Particle::isAlive()) {

        float sp = 15.5f;

        // Decrease life
        if (!spaceHeld) {
            life -= delta;
        }
        if (Particle::isAlive()) {

            glm::vec3 dir;
            // are we hitting the floor?
            if (pos.z <= floorZVal) {
                dir = vec3((rand() % 3) - 2, 0, 10.0f / n_bounces);
                n_bounces++;
                sp = 100.0f;
            } else {
                dir = gravity;
                if (spaceHeld) {
                    dir = (target - pos);
                    sp = sp * 100 / glm::distance(pos, target);
                }
            }
            speed = dir * (float) delta * sp;
            pos += speed * (float) delta;
            cameradistance = length2(pos - CameraPosition);


        } else {
            // Particles that just died will be put at the end of the buffer in SortParticles();
            cameradistance = -1.0f;
        }
    }
}

bool Particle::isAlive() {
    return life > 0.0f;
}

Particle::Particle() {
    life = -1.0f;
    cameradistance = -1.0f;
}

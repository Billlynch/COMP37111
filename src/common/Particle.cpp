//
// Created by Bill Lynch on 15/10/2018.
//

#include "Particle.h"

void Particle::simulate(double &delta, const vec3 &CameraPosition, bool &spaceHeld, float &floorZVal, vec3 &gravity) {
    if (Particle::isAlive()) {

        // Decrease life
        if (!spaceHeld) { // Don't de-spawn particles if space is being held.
            life -= delta;
        }

        if (Particle::isAlive()) {
            calculateMotion(delta, CameraPosition, spaceHeld, floorZVal, gravity);

        } else {
            cameradistance = -1.0f; // so it will not get rendered.
        }
    }
}

void Particle::calculateMotion(const double &delta, const vec3 &CameraPosition, const bool &spaceHeld,
                               const float &floorZVal, const vec3 &gravity) {
    vec3 acceleration;
    float speed_multiplier = 5.5f;

    if (pos.z <= floorZVal) {
        makeParticleBounce(floorZVal, acceleration, speed_multiplier);
    } else {
        makeParticleFallWithGravity(spaceHeld, gravity, acceleration, speed_multiplier);
    }

    speed = acceleration * mass * (float) delta * speed_multiplier; // F = m * a

    pos += speed * (float) delta; // move the particle based on the 'force'

    cameradistance = length2(pos - CameraPosition); // calculate the distance from the camera
}

void Particle::makeParticleFallWithGravity(const bool &spaceHeld, const vec3 &gravity, vec3 &acceleration,
                                           float &speed_multiplier) const {
    acceleration = gravity;
    if (spaceHeld) {
        acceleration = (target - pos);
        speed_multiplier = speed_multiplier * distance(pos, target);
    }
}

void Particle::makeParticleBounce(const float &floorZVal, vec3 &acceleration, float &speed_multiplier) {
    pos.z = floorZVal + 0.1f;
    acceleration = vec3((rand() % 3) - 1, (rand() % 3) - 1, 5.0f / n_bounces);
    n_bounces++;
    speed_multiplier = 50.0f;
}

bool Particle::isAlive() {
    return life > 0.0f;
}

// when creating a new Particle make it so that it is 'dead' and push it to the back of the buffer so it is not rendered
Particle::Particle() {
    life = -1.0f;
    cameradistance = -1.0f;
}

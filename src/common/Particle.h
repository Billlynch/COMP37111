//
// Created by Bill Lynch on 15/10/2018.
//

#ifndef GRAPHICS_PARTICLE_H
#define GRAPHICS_PARTICLE_H
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/norm.hpp>
#include <glm/glm.hpp>

using namespace glm;

class Particle {
private:
    vec3 pos, speed, target;
    unsigned char r, g, b, a; // Color
    int n_bounces;
    float size, life, cameradistance, mass;

    void makeParticleBounce(const float &floorZVal, vec3 &acceleration, float &speed_multiplier);

    void makeParticleFallWithGravity(const bool &spaceHeld, const vec3 &gravity, vec3 &acceleration,
                                     float &speed_multiplier) const;

    void calculateMotion(const double &delta, const vec3 &CameraPosition, const bool &spaceHeld, const float &floorZVal,
                         const vec3 &gravity);

public:
    Particle();

    Particle(vec3 &pos,
             vec3 &speed,
             vec3 &target,
             float mass,
             unsigned char r,
             unsigned char g,
             unsigned char b,
             unsigned char a,
             float size,
             float life) : pos(pos),
                           speed(speed),
                           target(target), r(r), g(g),
                           b(b), a(a),
                           size(size), life(life), mass(mass) { n_bounces = 1; }

    ~Particle() {}

    void simulate(double &delta, const vec3 &CameraPosition, bool &spaceHeld, float &floorZVal, vec3 &gravity);

    bool isAlive();

    bool operator<(const Particle &that) const {
        // Sort in reverse order : far particles drawn first.
        return this->cameradistance > that.cameradistance;
    }

    const vec3 &getPos() const {
        return pos;
    }

    void setPos(const vec3 &pos) {
        Particle::pos = pos;
    }

    const vec3 &getSpeed() const {
        return speed;
    }

    void setSpeed(const vec3 &speed) {
        Particle::speed = speed;
    }

    const vec3 &getTarget() const {
        return target;
    }

    void setTarget(const vec3 &target) {
        Particle::target = target;
    }

    unsigned char getR() const {
        return r;
    }

    void setR(unsigned char r) {
        Particle::r = r;
    }

    unsigned char getG() const {
        return g;
    }

    void setG(unsigned char g) {
        Particle::g = g;
    }

    unsigned char getB() const {
        return b;
    }

    void setB(unsigned char b) {
        Particle::b = b;
    }

    unsigned char getA() const {
        return a;
    }

    void setA(unsigned char a) {
        Particle::a = a;
    }

    int getN_bounces() const {
        return n_bounces;
    }

    void setN_bounces(int n_bounces) {
        Particle::n_bounces = n_bounces;
    }

    float getSize() const {
        return size;
    }

    void setSize(float size) {
        Particle::size = size;
    }

    float getLife() const {
        return life;
    }

    void setLife(float life) {
        Particle::life = life;
    }

    float getCameradistance() const {
        return cameradistance;
    }

    void setCameradistance(float cameradistance) {
        Particle::cameradistance = cameradistance;
    }


};


#endif //GRAPHICS_PARTICLE_H

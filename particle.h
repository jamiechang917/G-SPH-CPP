#pragma once
#include "myvector.h"
#include <iostream>
#include <random>
using namespace std;

class Particle {
  public:
    Particle(long id, double mass); // initialization function
    long id;
    double mass;
    vec3d pos;
    vec3d vel;
    vec3d acc;
    vec3d acc_old;
    void info();                                        // show information     
    void setPosition(vec3d pos);                        // set postion
    void setVelocity(vec3d vel);                        // set velocity
    void setAccleration(vec3d vel);                     // set accleration
    void setRandomPosition(double lower, double upper); // set random position
    void copy(Particle* particle);                      // copy informations to this one
};
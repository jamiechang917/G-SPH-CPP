#include "force.h"

vec3d gravityAcc(double M, vec3d rVector) {
    // return the accleration vector
    vec3d acc;
    double r = vecNorm(rVector);
    assert(r != 0.0 || SOFTEN_FACTOR != 0.0);
    double s = CONST_G * M / pow((pow(r, 2.0) + pow(SOFTEN_FACTOR, 2.0)), 1.5);
    for (int i = 0; i < 3; i++) {
        acc[i] = -s * rVector[i];
    }
    return acc;
}
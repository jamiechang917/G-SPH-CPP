#include "myvector.h"

vec3d vecMinus(vec3d vec1, vec3d vec2) {
    vec3d result;
    for (int i = 0; i < 3; i++) {
        result[i] = vec1[i] - vec2[i];
    }
    return result;
}

double vecNorm(vec3d vec) {
    double result = 0.0;
    for (int i = 0; i < 3; i++) {
        result += pow(vec[i], 2.0);
    }
    assert(result >= 0.0);
    return sqrt(result);
}
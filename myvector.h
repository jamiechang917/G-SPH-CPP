#pragma once
#include <array>
#include <assert.h>
#include <math.h>
using namespace std;

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

typedef array<float, 3> vec3f;
typedef array<double, 3> vec3d;

vec3d vecMinus(vec3d vec1, vec3d vec2);
double vecNorm(vec3d vec);
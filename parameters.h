#pragma once
#define OUTPUT_PRECISION    7
// #define CONST_G 6.6743e-11 // gravitational constant
#define CONST_G             1.0
#define SOFTEN_FACTOR       1e-3
#define THETA_THRESHOLD     0.5 // set to zero will return to direct N-body
#define TIMESTEP            0.25       // dt

// CUDA parameters
#define GRIDDIM             64  // how many blocks in a grid
#define BLOCKDIM           912  // how many threads in a block
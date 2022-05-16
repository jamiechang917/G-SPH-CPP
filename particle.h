#ifndef PARTICLES
#define PARTICLES

#include <vector>
#include "randoms.h"
#include "vector_ops.h"
using namespace std;

class Particles {
    public:
        Particles(long int id, double m, vector<double> pos, vector<double> vel, vector<double> acc); // init function
        long int id; // id
        double m; // mass
        vector<double> pos; // position
        vector<double> vel; // velocity
        vector<double> acc; // acceleration
};

vector<Particles*> generate_random_particles(long unsigned int N, double lower_bound, double upper_bound);



#endif

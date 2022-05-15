#include "particles.h"
using namespace std;
#include <iostream>

Particles::Particles(long int id, double m, vector<double> pos, vector<double> vel, vector<double> acc) {
    this->id = id;
    this->m = m;
    this->pos = pos;
    this->vel = vel;
    this->acc = acc;
};

vector<Particles*> generate_random_particles(long unsigned int N, double lower_bound, double upper_bound) {
    // vector<Particles*> particles;
    vector<Particles*>* particles = new vector<Particles*>;
    for (long unsigned int i=0; i<N; i++) {
        long int id = (long int) i;
        double m = 1.0;
        vector<double>* pos = new vector<double>;
        vector<double>* vel = new vector<double>;
        vector<double>* acc = new vector<double>;
        // random
        for (int i=0; i < 3; i++) {
            random_device rd;
            mt19937 gen(rd());
            uniform_real_distribution<> dist(lower_bound, upper_bound);
            double t = dist(gen);
            pos->push_back(t);
            vel->push_back(0.0);
            acc->push_back(0.0);
        }
        Particles* particle = new Particles(id, m, *pos, *vel, *acc);
        particles->push_back(particle);
    }
    return *particles;
}
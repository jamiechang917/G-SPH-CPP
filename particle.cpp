#include "particle.h"
using namespace std;

Particle::Particle(long id, double mass) {
    this->id = id;
    this->mass = mass;
    this->pos = {0., 0., 0.};
    this->vel = {0., 0., 0.};
    this->acc = {0., 0., 0.};
}

void Particle::info() {
    cout << "\n[Particle] Particle Information" << endl;
    cout << "ID:\t\t" << this->id << endl;
    cout << "Mass:\t\t" << this->mass << endl;
    cout << "Position:\t(" << this->pos[0] << ", " << this->pos[1] << ", "
         << this->pos[2] << ")" << endl;
    cout << "Velocity:\t(" << this->vel[0] << ", " << this->vel[1] << ", "
         << this->vel[2] << ")" << endl;
    cout << "Accleration:\t(" << this->acc[0] << ", " << this->acc[1] << ", "
         << this->acc[2] << ")" << endl;
}

void Particle::setPosition(vec3d pos) {
    for (int i = 0; i < 3; i++) {
        this->pos[i] = pos[i];
    }
}

void Particle::setVelocity(vec3d vel) {
    for (int i = 0; i < 3; i++) {
        this->vel[i] = vel[i];
    }
}

void Particle::setAccleration(vec3d acc) {
    for (int i = 0; i < 3; i++) {
        this->acc[i] = acc[i];
    }
}

void Particle::setRandomPosition(double lower, double upper) {
    for (int i = 0; i < 3; i++) {
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<> dist(lower, upper);
        this->pos[i] = dist(gen);
    }
}
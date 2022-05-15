#ifndef FORCE_H
#define FORCE_H

#include <vector>
#include "BHtree.h"
#include "particles.h"

using namespace std;

vector<double> get_gravity_acc(double M, vector<double> r_vector);
void init_acc(BHNode* root_ptr);
void update_gravity_acc_BHtree(Particles* target_particle, BHNode* root_ptr, int* times);
void update_gravity_acc_direct(Particles* target_particle, BHNode* root_ptr, int* times);

#endif
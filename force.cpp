#include "force.h"
#include "parameters.h"
#include <assert.h>
#include <iostream>
using namespace std;

vector<double> get_gravity_acc(double M, vector<double> r_vector) {
    // return vector acceleration
    double r = vec1D_norm(r_vector);
    double s = CONST_G*M / pow((pow(r, 2.0) + pow(SOFTEN_FACTOR, 2.0)), 1.5);
    vector<double> acc;
    for (double e : r_vector) {
        acc.push_back(-s*e);
    }
    return acc;
}

void init_acc(BHNode* root_ptr) {
    assert (root_ptr->isroot);
    for (Particles* particle : root_ptr->particles) {
        fill(particle->acc.begin(), particle->acc.end(), 0.0);
    }
}

void update_gravity_acc_BHtree(Particles* target_particle, BHNode* node_ptr, int* times) {
    if (node_ptr->particles.size() == 0) {
        return;
    }
    else if (node_ptr->particles.size() == 1) {
        *times += 1;
        vector<double> r_vector = vec1D_minus(target_particle->pos, node_ptr->center_of_mass);
        vector<double> acc = get_gravity_acc(node_ptr->total_mass, r_vector);
        if (node_ptr->particles[0] != target_particle) {
            for (int i=0; i<3; i++) {
                target_particle->acc[i] += acc[i];
            }
        }   
    }
    else {
        vector<double> r_vector = vec1D_minus(target_particle->pos, node_ptr->center_of_mass);
        double r = vec1D_norm(r_vector);
        double d = node_ptr->cell_size;
        if (d/r < THETA_THRESHOLD) {
            // F = force between target particle and the entire node.
            // acc = ( G*Mass_of_node / r^3 ) * vector(r)
            *times += 1;
            vector<double> acc = get_gravity_acc(node_ptr->total_mass, r_vector);
            for (int i=0; i<3; i++) {
                target_particle->acc[i] += acc[i];
            }

        }
        else {
            for (BHNode* child_ptr : node_ptr->children_nodes) {
                update_gravity_acc_BHtree(target_particle, child_ptr, times);
            }
        }
    }
    return;
}

void update_gravity_acc_direct(Particles* target_particle, BHNode* node_ptr, int* times) {
    assert(node_ptr->isroot);
    for (Particles* particle : node_ptr->particles) {
        vector<double> r_vector = vec1D_minus(target_particle->pos, particle->pos);
        vector<double> acc = get_gravity_acc(particle->m, r_vector);
        for (int i=0; i<3; i++) {
            target_particle->acc[i] += acc[i];
        }
    }
}
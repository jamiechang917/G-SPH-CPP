#include "BHtree.h"
#include <iostream>
#include <string>
using namespace std;

BHNode::BHNode(BHNode* parent_node, long unsigned int cell_depth, double cell_size, vector<double> cell_pos, vector<Particles*> particles, bool isroot) {
            this->parent_node = parent_node;
            if (!isroot) {
                 parent_node->children_nodes.push_back(this); // add this into parent's children
            }
            this->cell_depth = cell_depth;
            this->cell_size = cell_size;
            this->cell_pos = cell_pos;
            this->particles = particles;
            this->isroot = isroot;
            };

double get_total_mass(vector<Particles*> particles) {
    double total_mass = 0.0;
    for (Particles* particle : particles) {
        total_mass += particle->m;
    }
    return total_mass;
}

vector<double> get_center_of_mass(vector<Particles*> particles) {
    double total_mass = 0.0;
    vector<double> cm = {0.0, 0.0, 0.0};
    for (Particles* particle : particles) {
        total_mass += particle->m;
        vector<double> pos;
        for (double e : particle->pos) {
            pos.push_back(e*particle->m);
        }
        transform(begin(cm), end(cm), begin(pos), begin(cm), plus<double>());
    }
    for (double &e : cm) {
        e = e/total_mass;
    }
    return cm;
}

BHNode create_root(vector<Particles*> particles) {
    // Find the size (length) of the root, which is the max/min position of particles.
    vector<double>* all_pos = new vector<double>;
    for (Particles* particle : particles) { // add x,y,z to all_pos
        all_pos->push_back(particle->pos[0]);
        all_pos->push_back(particle->pos[1]);
        all_pos->push_back(particle->pos[2]);
    }
    double max_pos = *max_element(begin(*all_pos), end(*all_pos));
    double min_pos = *min_element(begin(*all_pos), end(*all_pos));
    double root_size = abs(max_pos - min_pos);
    vector<double> root_pos = {min_pos, min_pos, min_pos};
    delete all_pos;
    BHNode root(nullptr, 0, root_size, root_pos, particles, true);
    return root;
}

void build_tree(BHNode* node_ptr) {
    if (node_ptr->particles.size() > 1) {
        node_ptr->total_mass = get_total_mass(node_ptr->particles);
        node_ptr->center_of_mass = get_center_of_mass(node_ptr->particles);

        // Distribute particles to eight different subcells.
        vector<Particles*>* D1 = new vector<Particles*>;
        vector<Particles*>* D2 = new vector<Particles*>;
        vector<Particles*>* D3 = new vector<Particles*>;
        vector<Particles*>* D4 = new vector<Particles*>;
        vector<Particles*>* D5 = new vector<Particles*>;
        vector<Particles*>* D6 = new vector<Particles*>;
        vector<Particles*>* D7 = new vector<Particles*>;
        vector<Particles*>* D8 = new vector<Particles*>;
        vector<double>* P1 = new vector<double>;
        vector<double>* P2 = new vector<double>;
        vector<double>* P3 = new vector<double>;
        vector<double>* P4 = new vector<double>;
        vector<double>* P5 = new vector<double>;
        vector<double>* P6 = new vector<double>;
        vector<double>* P7 = new vector<double>;
        vector<double>* P8 = new vector<double>;
        vector<double> cell_center = {
                                    node_ptr->cell_pos[0] + 0.5*node_ptr->cell_size,
                                    node_ptr->cell_pos[1] + 0.5*node_ptr->cell_size,
                                    node_ptr->cell_pos[2] + 0.5*node_ptr->cell_size,
                                    };
        for (Particles* particle : node_ptr->particles) {
            double x = particle->pos[0];
            double y = particle->pos[1];
            double z = particle->pos[2];
            if (x <= cell_center[0]) {
                if (y <= cell_center[1]) {
                    if (z <= cell_center[2]) {
                        D1->push_back(particle);
                    }
                    else {
                        D2->push_back(particle);
                    }
                }
                else {
                    if (z <= cell_center[2]) {
                        D3->push_back(particle);
                    }
                    else {
                        D4->push_back(particle);
                    }
                }
            }
            else {
                if (y <= cell_center[1]) {
                    if (z <= cell_center[2]) {
                        D5->push_back(particle);
                    }
                    else {
                        D6->push_back(particle);
                    }
                }
                else {
                    if (z <= cell_center[2]) {
                        D7->push_back(particle);
                    }
                    else {
                        D8->push_back(particle);
                    }
                }
            }
        }
        P1->insert(P1->end(), {node_ptr->cell_pos[0], node_ptr->cell_pos[1], node_ptr->cell_pos[2]});
        P2->insert(P2->end(), {node_ptr->cell_pos[0], node_ptr->cell_pos[1], cell_center[2]});
        P3->insert(P3->end(), {node_ptr->cell_pos[0], cell_center[1], node_ptr->cell_pos[2]});
        P4->insert(P4->end(), {node_ptr->cell_pos[0], cell_center[1], cell_center[2]});
        P5->insert(P5->end(), {cell_center[0], node_ptr->cell_pos[1], node_ptr->cell_pos[2]});
        P6->insert(P6->end(), {cell_center[0], node_ptr->cell_pos[1], cell_center[2]});
        P7->insert(P7->end(), {cell_center[0], cell_center[1], node_ptr->cell_pos[2]});
        P8->insert(P8->end(), {cell_center[0], cell_center[1], cell_center[2]});
    
        BHNode* N1 = new BHNode(node_ptr, node_ptr->cell_depth + 1, 0.5*node_ptr->cell_size, *P1, *D1, false);
        BHNode* N2 = new BHNode(node_ptr, node_ptr->cell_depth + 1, 0.5*node_ptr->cell_size, *P2, *D2, false);
        BHNode* N3 = new BHNode(node_ptr, node_ptr->cell_depth + 1, 0.5*node_ptr->cell_size, *P3, *D3, false);
        BHNode* N4 = new BHNode(node_ptr, node_ptr->cell_depth + 1, 0.5*node_ptr->cell_size, *P4, *D4, false);
        BHNode* N5 = new BHNode(node_ptr, node_ptr->cell_depth + 1, 0.5*node_ptr->cell_size, *P5, *D5, false);
        BHNode* N6 = new BHNode(node_ptr, node_ptr->cell_depth + 1, 0.5*node_ptr->cell_size, *P6, *D6, false);
        BHNode* N7 = new BHNode(node_ptr, node_ptr->cell_depth + 1, 0.5*node_ptr->cell_size, *P7, *D7, false);
        BHNode* N8 = new BHNode(node_ptr, node_ptr->cell_depth + 1, 0.5*node_ptr->cell_size, *P8, *D8, false);

        build_tree(N1);
        build_tree(N2);
        build_tree(N3);
        build_tree(N4);
        build_tree(N5);
        build_tree(N6);
        build_tree(N7);
        build_tree(N8);
    // add relation of the parent and children (THIS iS DONE when create BHNODE)
    }
    else if (node_ptr->particles.size() == 1) {
        node_ptr->total_mass = get_total_mass(node_ptr->particles);
        node_ptr->center_of_mass = get_center_of_mass(node_ptr->particles);
    }
    else {
        return;
    }
    return;
}

void calculate_gravity(BHNode* root_ptr) {
    
}
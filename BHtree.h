#ifndef BHTREE_H
#define BHTREE_H

#include <vector>
#include <algorithm>
// #include <matplotlibcpp.h>
#include "particles.h"
#include "vector_ops.h"
using namespace std;
// namespace plt = matplotlibcpp;

// reference: http://arborjs.org/docs/barnes-huthttp://arborjs.org/docs/barnes-hut
// workflow of Barnes Hut Algorithm

// How to build the tree
// 1. create root node
// 2. add all particles into root node
// 3. update root node center of mass
// 4. distribute particles to eight different child nodes.
// 5. check the number of particles of each child node.
// 6. if the child node contains more than one particles, repeat step 3 for that child node.
// 7. if the child contain only one particles, update its center of mass. (add the pointer to that particles)
// 8. if the child doesn't contain any particles, then nothing needs to do.

// How to calculate the force
// 1. 
// 2.

class BHNode {
    public:
        BHNode( BHNode* parent_node, 
                long unsigned int cell_depth,
                double cell_size, 
                vector<double> cell_pos,
                vector<Particles*> particles,
                bool isroot);
        BHNode* parent_node; // pointer of the parent node
        vector<BHNode*> children_nodes; // pointers of 8 child nodes (oct-tree)
        int cell_depth; // depth of the cell (node)
        double cell_size; // size (length) of the cell (node)
        vector<double> cell_pos; // corner position of the cell (cell boundaries: i = cell_pos[i] ~ cell_pos[i]+cell_size)
        vector<Particles*> particles; // pointers of particles inside this cell (node)
        double total_mass; // total mass of the particles
        vector<double> center_of_mass; // center of mass
        bool isroot; // whether this is root node
    // private:
};

double get_total_mass(vector<Particles*> particles);

vector<double> get_center_of_mass(vector<Particles*> particles);

BHNode create_root(vector<Particles*> particles);

void build_tree(BHNode* node_ptr);
// void delete_BHtree(BHNode* root);

void calculate_gravity(BHNode* root_ptr);


#endif
#include <iostream>
#include <string>
#include "particles.h"
#include "fileIO.h"
#include "randoms.h"
#include "BHtree.h"
#include "vector_ops.h"

using namespace std;

int N = 100000;
// vector<Particles*> particles = generate_random_particles(N, -100.0, 100.0);
vector<Particles*> particles = read_particles("particles.txt");
BHNode root = create_root(particles);

int main() {
    build_tree(&root);
    // save_particles("particles.txt", root.particles);
    // save_cells(&root);
    return 0;
}
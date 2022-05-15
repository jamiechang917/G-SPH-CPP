#include <iostream>
#include <string>
#include <iomanip>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include "parameters.h"
#include "particles.h"
#include "fileIO.h"
#include "randoms.h"
#include "BHtree.h"
#include "vector_ops.h"
#include "force.h"

using namespace std;

int N = 1000;
// vector<Particles*> particles = generate_random_particles(N, -100.0, 100.0);
vector<Particles*> particles = read_particles("particles.txt");
BHNode root = create_root(particles);

void update(BHNode* root_ptr) {
    init_acc(root_ptr);
    for (Particles* particle : root_ptr->particles) {
        int times = 0;
        update_gravity_acc_BHtree(particle, root_ptr, &times);
        for (int i=0; i<3; i++) {
            particle->vel[i] += particle->acc[i]*TIMESTEP;
            particle->pos[i] += particle->vel[i]*TIMESTEP;
        }
    }
    return;
}

void save_data(BHNode* root_ptr, long int* index, string output_folder_name) {
    // save two files: particles_{*index}.txt and cells_{*index}.txt in output_folder_name
    char particles_filepath[100];
    char cells_filepath[100];
    sprintf(particles_filepath, "%s/particles_%ld.txt", output_folder_name.c_str(), *index);
    sprintf(cells_filepath, "%s/cells_%ld.txt", output_folder_name.c_str(), *index);
    cout << "Save particles (" << *index << ")..." << endl;
    save_particles(particles_filepath, root_ptr->particles);
    // cout << "Save cells (" << *index << ")..." << endl;
    // save_cells(cells_filepath, root_ptr);
    *index += 1;
}

int main() {
    // save_particles("particles.txt", root.particles);
    // index for saving data
    long int save_index = 0;

    // Make a directory to save the output
    struct timeval tp;
    char output_folder_name[50];
    gettimeofday(&tp, NULL);
    sprintf(output_folder_name, "output-%ld", tp.tv_sec);
    mkdir(output_folder_name, 0755);

    // Build BH-tree and save initial data
    build_tree(&root);
    save_data(&root, &save_index, output_folder_name);

    for (int i=0; i<50; i++) {
        update(&root);
        save_data(&root, &save_index, output_folder_name);
    }

    return 0;
}
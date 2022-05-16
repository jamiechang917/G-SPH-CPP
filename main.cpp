#include "fileIO.h"
#include "particle.h"
#include "tree.h"
#include "updater.h"
#include <iostream>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <vector>
using namespace std;

// notice
// 1. all particles must be declared as { Particle* p = new Particle(id, mass) }
// 2. or use static (assure it will not be deleted)

char* createOutputFolder() {
    struct timeval tp;
    static char outputFolder[50];
    gettimeofday(&tp, NULL);
    sprintf(outputFolder, "output-%ld", tp.tv_sec);
    mkdir(outputFolder, 0755);
    return outputFolder;
}

int main() {
    // frame counter
    long frame = 0;

    // create output folder to store result
    char* outputFolder = createOutputFolder();

    // vector<Particle*> particles;
    // for (int i = 0; i < 1000; i++) {
    //     Particle* p = new Particle(i, 1.0);
    //     p->setRandomPosition(-50, 50);
    //     particles.push_back(p);
    // }

    vector<Particle*> particles = readParticles("particles_N1000.txt");

    // ====================
    for (int i = 0; i < 100; i++) {
        Node* root = new Node(particles);
        buildTree(root);
        root->info();
        saveData(root, outputFolder, frame, true, false);
        updateTreeMethod(root);
        deleteTree(root);
        frame++;
    }
}
#include "fileIO.h"
#include "gpuplugin.h"
#include "particle.h"
#include "tree.h"
#include "updater.h"
#include <cuda_runtime.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
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
    clock_t clkStart, clkFinish;

    // CUDA UnifinedMemory Notes:
    // 1. new -> cudaMallocManaged() -> cudaDeviceSynchronize() -> cudaFree() ->
    // free()

    // frame counter
    long frame = 0;
    // create output folder to store result
    char* outputFolder = createOutputFolder();

    vector<Particle*> particles =
        readParticles("inputs/P10000New/P-N10000.txt");

    clkStart = clock();
    for (int i = 0; i < 300; i++) {
        Node* root = new Node(particles);
        // clkStart = clock();
        buildTree(root);
        // clkFinish = clock();
        // cout << "Runtime: " << (clkFinish - clkStart) * 1e-6 << " s" << endl;
        saveData(root, outputFolder, frame, true, false, true);
        updateTreeMethod(root);
        // updateBruteMethod(root);
        deleteTree(root);
        frame++;
    }
    clkFinish = clock();

    // vector<Particle*> particles;
    // for (int i = 0; i < 1000; i++) {
    //     Particle* p = new Particle(i, 1.0);
    //     p->setRandomPosition(-100, 100);
    //     particles.push_back(p);
    // }
    // printf("Generate N particles\n");

    // const long N = particles.size();

    // // CUDA Unified Memory (1D linearlized array)
    // long* idUM = new long[N];
    // double* massUM = new double[N];
    // double* posUM = new double[N * 3];
    // double* velUM = new double[N * 3];
    // double* accUM = new double[N * 3];
    // cudaMallocManaged((void**)&idUM, sizeof(long) * N);
    // cudaMallocManaged((void**)&massUM, sizeof(double) * N);
    // cudaMallocManaged((void**)&posUM, sizeof(double) * N * 3);
    // cudaMallocManaged((void**)&velUM, sizeof(double) * N * 3);
    // cudaMallocManaged((void**)&accUM, sizeof(double) * N * 3);
    // particlesToUM(particles, idUM, massUM, posUM, velUM, accUM);
    // checkUM(particles, idUM, massUM, posUM, velUM, accUM);

    // clkStart = clock();
    // for (int i = 0; i < 10; i++) {

    //     // Node root(particles);
    //     // buildTree(&root);
    //     // updateTreeMethod(&root);

    //     updateBruteMethodGPU(N, massUM, posUM, velUM, accUM);

    //     cout << i << endl;
    // }
    // clkFinish = clock();

    // UMToParticles(particles, idUM, massUM, posUM, velUM, accUM);
    // particles[10]->info();

    cout << "Runtime: " << (clkFinish - clkStart) * 1e-6 << " s" << endl;
    return 0;
}

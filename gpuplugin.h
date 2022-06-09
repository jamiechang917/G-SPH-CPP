#pragma once
#include "force.h"
#include "myvector.h"
#include "parameters.h"
#include "particle.h"
#include <cuda_runtime.h>
#include <iostream>
#include <vector>


void checkGPUInfo();

void particlesToUM(vector<Particle*> particles, long* idUM, double* massUM, double* posUM, double* velUM, double* accUM);

void UMToParticles(vector<Particle*> particles, long* idUM, double* massUM, double* posUM, double* velUM, double* accUM);

void checkUM(vector<Particle*> particles, long* idUM, double* massUM, double* posUM, double* velUM, double* accUM);

void updateBruteMethodGPU(long N, double* massUM, double* posUM, double* velUM, double* accUM);

// CUDA functions
__global__ void initializeAcc(long N, double* accUM);
__global__ void move(long N, double* posUM, double* velUM, double* accUM);
__global__ void updateGravityBruteMethod(long N, double* massUM, double* posUM, double* velUM, double* accUM);


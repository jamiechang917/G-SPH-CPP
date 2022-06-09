#include "gpuplugin.h"

// GPU functions

void checkGPUInfo() {
    int deviceCount, device;
    struct cudaDeviceProp properties;
    cudaError_t cudaResultCode = cudaGetDeviceCount(&deviceCount);
    if (cudaResultCode != cudaSuccess)
        deviceCount = 0;
    /* machines with no GPUs can still report one emulation device */
    for (device = 0; device < deviceCount; ++device) {
        cudaGetDeviceProperties(&properties, device);
        if (properties.major != 9999) /* 9999 means emulation only */
            if (device == 0) {
                printf("multiProcessorCount %d\n",
                       properties.multiProcessorCount);
                printf("maxThreadsPerMultiProcessor %d\n",
                       properties.maxThreadsPerMultiProcessor);
            }
    }
}

void particlesToUM(vector<Particle*> particles, long* idUM, double* massUM,
                   double* posUM, double* velUM, double* accUM) {
    long N = particles.size();
    for (long i = 0; i < N; i++) {
        idUM[i] = particles[i]->id;
        massUM[i] = particles[i]->mass;
        for (int j = 0; j < 3; j++) {
            posUM[i * 3 + j] = particles[i]->pos[j];
            velUM[i * 3 + j] = particles[i]->vel[j];
            accUM[i * 3 + j] = particles[i]->acc[j];
        }
    }
}

void UMToParticles(vector<Particle*> particles, long* idUM, double* massUM,
                   double* posUM, double* velUM, double* accUM) {
    long N = particles.size();
    for (long i = 0; i < N; i++) {
        particles[i]->id = idUM[i];
        particles[i]->mass = massUM[i];
        for (int j = 0; j < 3; j++) {
            particles[i]->pos[j] = posUM[i * 3 + j];
            particles[i]->vel[j] = velUM[i * 3 + j];
            particles[i]->acc[j] = accUM[i * 3 + j];
        }
    }
}

void checkUM(vector<Particle*> particles, long* idUM, double* massUM,
             double* posUM, double* velUM, double* accUM) {
    long N = particles.size();
    for (long i = 0; i < N; i++) {
        assert(particles[i]->id == idUM[i] && "IDs not match!");
        assert(particles[i]->mass == massUM[i] && "Masses not match!");
        for (int j = 0; j < 3; j++) {
            assert(particles[i]->pos[j] == posUM[i * 3 + j] &&
                   "Positions not match!");
            assert(particles[i]->vel[j] == velUM[i * 3 + j] &&
                   "Velocities not match!");
            assert(particles[i]->acc[j] == accUM[i * 3 + j] &&
                   "Acclerations not match!");
        }
    }
}

__global__ void initializeAcc(long N, double* accUM) {
    int tid = blockDim.x * blockIdx.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    for (long i = 0; i < N; i += stride) {
        if (tid + i < N) {
            for (int j = 0; j < 3; j++) {
                accUM[(tid + i) * 3 + j] = 0.0;
            }
        }
    }
}

__global__ void move(long N, double* posUM, double* velUM, double* accUM) {
    int tid = blockDim.x * blockIdx.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    for (long i = 0; i < N; i += stride) {
        if (tid + i < N) {
            for (int j = 0; j < 3; j++) {
                atomicAdd(&velUM[(tid + i) * 3 + j],
                          accUM[(tid + i) * 3 + j] * TIMESTEP);
                atomicAdd(&posUM[(tid + i) * 3 + j],
                          velUM[(tid + i) * 3 + j] * TIMESTEP);
            }
        }
    }
}

// ----
__global__ void updateGravityBruteMethod(long N, double* massUM, double* posUM,
                                         double* velUM, double* accUM) {
    int tid = blockDim.x * blockIdx.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    for (long i = 0; i < N; i += stride) {
        if (tid + i < N) {
            for (long k = tid + i + 1; k < N; k++) {
                // particles pair
                double rVector[3];
                double rNorm = 0.0;
                for (int j = 0; j < 3; j++) {
                    rVector[j] = posUM[(tid + i) * 3 + j] - posUM[k * 3 + j];
                    rNorm += pow(rVector[j], 2.0);
                }
                rNorm = sqrt(rNorm);
                assert(rNorm != 0.0 || SOFTEN_FACTOR != 0.0);
                for (int j = 0; j < 3; j++) {
                    atomicAdd(
                        &accUM[(tid + i) * 3 + j],
                        -CONST_G * massUM[k] * rVector[j] /
                            pow((pow(rNorm, 2.0) + pow(SOFTEN_FACTOR, 2.0)),
                                1.5));
                    atomicAdd(
                        &accUM[k * 3 + j],
                        CONST_G * massUM[tid + i] * rVector[j] /
                            pow((pow(rNorm, 2.0) + pow(SOFTEN_FACTOR, 2.0)),
                                1.5));
                }
            }
        }
    }
    return;
}

void updateBruteMethodGPU(long N, double* massUM, double* posUM, double* velUM,
                          double* accUM) {

    initializeAcc<<<GRIDDIM, BLOCKDIM>>>(N, accUM);
    cudaDeviceSynchronize();

    updateGravityBruteMethod<<<GRIDDIM, BLOCKDIM>>>(N, massUM, posUM, velUM,
                                                    accUM);
    cudaDeviceSynchronize();
    move<<<GRIDDIM, BLOCKDIM>>>(N, posUM, velUM, accUM);
    cudaDeviceSynchronize();
    return;
}

// ================= Example =============== //

// __global__ void test(double posGPU[100000][3]) {
//     int id = blockIdx.x * blockDim.x + threadIdx.x;
//     // printf("%lf\n", posGPU[400][2]);
// }

// int main() {
//     // CUDA UnifinedMemory Notes:
//     // 1. new -> cudaMallocManaged() -> cudaDeviceSynchronize() ->
//     cudaFree()
//     ->
//     // free()

//     vector<Particle*> particles = readParticles("particles_N1000.txt");
//     // const long N = particles.size();
//     const long N = 1000000;

//     auto* posGPU = new double[N][3];
//     cudaMallocManaged((void**)&posGPU, sizeof(double) * N * 3);
//     for (int i = 0; i < N; i++) {
//         for (int j = 0; j < 3; j++) {
//             posGPU[i][j] = i * j;
//         }
//     }

//     test<<<GRIDDIM, BLOCKDIM>>>(posGPU);
//     cudaDeviceSynchronize();
//     return 0;
// }
// ================================================= //
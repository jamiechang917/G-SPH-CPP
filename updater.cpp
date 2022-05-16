#include "updater.h"

void initialize(Node* rootNode) {
    // set the accleration to zero for every particles
    assert(rootNode->isRootNode && "The input Node must be root node.");
    for (Particle* p : rootNode->particles) {
        for (int i = 0; i < 3; i++) {
            p->acc[i] = 0.0;
        }
    }
    return;
}

void move(Node* rootNode) {
    // update velocity and position
    assert(rootNode->isRootNode && "The input Node must be root node.");
    for (Particle* p : rootNode->particles) {
        for (int i = 0; i < 3; i++) {
            p->vel[i] += p->acc[i] * TIMESTEP;
            p->pos[i] += p->vel[i] * TIMESTEP;
        }
    }
    return;
}

void updateGravityBruteMethod(Particle* targetParticle, Node* rootNode) {
    assert(rootNode->isRootNode && "The input Node must be root node.");
    for (Particle* p : rootNode->particles) {
        if (targetParticle != p) {
            vec3d rVector = vecMinus(targetParticle->pos, p->pos);
            vec3d acc = gravityAcc(p->mass, rVector);
            for (int i = 0; i < 3; i++) {
                targetParticle->acc[i] += acc[i];
            }
        }
    }
    return;
}

void updateGravityTreeMethod(Particle* targetParticle, Node* node) {
    if (node->particles.size() == 0) {
        return;
    } else if (node->particles.size() == 1) {
        if (targetParticle != node->particles[0]) {
            vec3d rVector =
                vecMinus(targetParticle->pos, node->particles[0]->pos);
            vec3d acc = gravityAcc(node->particles[0]->mass, rVector);
            for (int i = 0; i < 3; i++) {
                targetParticle->acc[i] += acc[i];
            }
        }
        return;
    } else {
        vec3d rVector = vecMinus(targetParticle->pos, node->cellCM);
        double r = vecNorm(rVector);
        double d = node->cellSize;
        if (d / r < THETA_THRESHOLD) {
            // Approximate by using cell's CM and total mass.
            vec3d acc = gravityAcc(node->cellMass, rVector);
            for (int i = 0; i < 3; i++) {
                targetParticle->acc[i] += acc[i];
            }
        } else {
            for (Node* childNode : node->childNodes) {
                updateGravityTreeMethod(targetParticle, childNode);
            }
        }
    }
    return;
}

void updateBruteMethod(Node* rootNode) {
    assert(rootNode->isRootNode && "The input Node must be root node.");
    initialize(rootNode);
    for (Particle* p : rootNode->particles) {
        // update gravity for each particles
        updateGravityBruteMethod(p, rootNode);
    }
    move(rootNode);
    return;
}

void updateTreeMethod(Node* rootNode) {
    assert(rootNode->isRootNode && "The input Node must be root node.");
    initialize(rootNode);
    for (Particle* p : rootNode->particles) {
        // update gravity for each particles
        updateGravityTreeMethod(p, rootNode);
    }
    move(rootNode);
    return;
}
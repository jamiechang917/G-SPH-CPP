#pragma once
#include "force.h"
#include "myvector.h"
#include "particle.h"
#include "tree.h"
#include <assert.h>

void initialize(Node* rootNode);
void move(Node* rootNode);

void updateGravityBruteMethod(Particle* targetParticle, Node* rootNode);
void updateGravityTreeMethod(Particle* targetParticle, Node* node);

void updateBruteMethod(Node* rootNode);
void updateTreeMethod(Node* rootNode);
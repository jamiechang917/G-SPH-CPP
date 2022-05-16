#pragma once
#include "myvector.h"
#include "parameters.h"
#include "particle.h"
#include "tree.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;

vector<Particle*> readParticles(string filePath);
void saveParticles(string filePath, vector<Particle*> particles);
void saveCells(string filePath, Node* rootNode);
void saveData(Node* rootNode, string outputFolderPath, long frame,
              bool toSaveParticles,
              bool toSaveCells); // save data for given frame
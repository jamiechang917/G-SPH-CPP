#ifndef FILEIO
#define FILEIO
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <fstream>
#include "particles.h"
#include "BHtree.h"
using namespace std;

void save_vector_1D(string filepath, vector<double> vec, bool append_mode);
void save_vector_2D(string filepath, vector<vector<double> > vec, bool append_mode);
vector<double> read_vector_1D(string filepath);
vector<vector<double> > read_vector_2D(string filepath);

void save_particles(string filepath, vector<Particles*> particles);
vector<Particles*> read_particles(string filepath);

void save_cells(BHNode* node_ptr);

#endif
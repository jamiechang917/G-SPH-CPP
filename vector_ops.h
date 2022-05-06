#ifndef VECTOR_OPS_H
#define VECTOR_OPS_H
#include <vector>
#include <algorithm>
using namespace std;

vector<double> vec1D_add(vector<double> a, vector<double> b);

vector<double> vec1D_minus(vector<double> a, vector<double> b);

double vec1D_sum(vector<double> vec);

double vec1D_max(vector<double> vec);

double vec1D_min(vector<double> vec);

void print_vec1D(vector<double> vec);

#endif
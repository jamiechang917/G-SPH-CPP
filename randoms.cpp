#include "randoms.h"

vector<double> uniform_double_dist(double lower, double upper, size_t size) {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dist(lower, upper);
    vector<double>* vec = new vector<double>;

    for (size_t i=0; i < size; i++) {
        vec->push_back(dist(gen));
    }
    return *vec;
}
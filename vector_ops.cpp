#include "vector_ops.h"
#include <assert.h>
#include <iostream>
#include <string>

using namespace std;


vector<double> vec1D_add(vector<double> a, vector<double> b) {
    // return vector a+b
    assert(a.size() == b.size());
    vector<double> result(a.size());
    for (long unsigned int i=0; i<a.size(); i++) {
        result[i] = a[i] + b[i];
    }
    return result;
}

vector<double> vec1D_minus(vector<double> a, vector<double> b) {
    // return vector a-b
    assert(a.size() == b.size());
    vector<double> result(a.size());
    for (long unsigned int i=0; i<a.size(); i++) {
        result[i] = a[i] - b[i];
    }
    return result;
}

double vec1D_sum(vector<double> vec) {
    double s = 0.0;
    for (long unsigned int i=0; i<vec.size(); i++) {
        s += vec[i];
    }
    return s;
}

double vec1D_max(vector<double> vec) {
    return *max_element(begin(vec), end(vec));
}

double vec1D_min(vector<double> vec) {
    return *min_element(begin(vec), end(vec));
}

void print_vec1D(vector<double> vec) {
    cout << endl << "Showing 1D vector (size: " << vec.size() << ")\n";
    for (double &e : vec) { 
        cout << e << " ";
    }
    cout << endl;
}


double vec1D_norm(vector<double> vec) {
    double result = 0.0;
    for (double e : vec) {
        result += pow(e, 2.);
    }
    return sqrt(result);
}

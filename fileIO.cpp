#include "fileIO.h"
using namespace std;

void save_vector_1D(string filepath, vector<double> vec, bool append_mode) {
    // write each element of the vector (1D) with a space at the end of the file.
    // vector<double> vec = {1., 2., 3.};
    // save_vector_1D("output.txt", vec);
    // $ cat output.txt
    // 1. 2. 3.
    if (append_mode) {
        ofstream ofs(filepath, ios::app); // append mode
        for (const auto &e: vec) {
            ofs << e << " ";
        }
        ofs << "\n";
    }
    else {
        ofstream ofs(filepath); // this will clean the file first
        for (const auto &e: vec) {
            ofs << e << " ";
        }
        ofs << "\n";
    }
    return;
}

void save_vector_2D(string filepath, vector<vector<double> > vec, bool append_mode) {
    // write each element of the vector (2D) with a space at the end of the file.
    // vector<vector<double> > vec = {{1., 2.}, {3., 4.}, {5., 6.}};
    // save_vector_2D("output.txt", vec);
    // $ cat output.txt
    // 1. 2.
    // 3. 4.
    // 5. 6.
    if (append_mode) {
        ofstream ofs(filepath, ios::app); // append mode
        for (const auto &row: vec) {
            for (const auto &e: row) {
                ofs << e << " ";    
            }
            ofs << "\n";   
        }
    }
    else {
        ofstream ofs(filepath); // this will clean the file first
        for (const auto &row: vec) {
            for (const auto &e: row) {
                ofs << e << " ";    
            }
            ofs << "\n";   
        }
    }
    return;
}

vector<double> read_vector_1D(string filepath) {
    // read the file and return 1D vector, remind the interval between each elements is a space.
    // $ cat output.txt
    // 1. 2. 3.
    // vector<double> vec_1D = read_vector_1D("output.txt")
    // vec_1D = {1., 2., 3.};
    ifstream ifs(filepath);
    string temprow;
    double tempdouble;
    vector<double> vec_1D;
    while (getline(ifs, temprow)) {
        istringstream iss(temprow);
        while (iss >> tempdouble) {
            vec_1D.push_back(tempdouble);
        }
    }
    return vec_1D;
}

vector<vector<double> > read_vector_2D(string filepath) {
    // read the file and return 2D vector, remind the interval between each elements is a space.
    // $ cat output.txt
    // 1. 2.
    // 3. 4.
    // 5. 6.
    // vector<vector<double> > vec_2D = read_vector_2D("output.txt")
    // vec_2D = {{1., 2.}, {3., 4.}, {5., 6.}};
    ifstream ifs(filepath);
    string temprow;
    vector<vector<double> > vec_2D;
    while (getline(ifs, temprow)) {
        double tempdouble;
        vector<double> vec_1D;
        istringstream iss(temprow);
        while (iss >> tempdouble) {
            vec_1D.push_back(tempdouble);
        }
        vec_2D.push_back(vec_1D);
    }
    return vec_2D;
}

void save_particles(string filepath, vector<Particles*> particles) {
    ofstream ofs(filepath);
    ofs << particles.size() << endl; // First line is the number of particles.
    for (const auto particle : particles) {
        ofs << particle->id << " ";
        ofs << particle->m << " ";
        ofs << particle->pos[0] << " " << particle->pos[1] << " " << particle->pos[2] << " ";
        ofs << particle->vel[0] << " " << particle->vel[1] << " " << particle->vel[2] << " ";
        ofs << particle->acc[0] << " " << particle->acc[1] << " " << particle->acc[2] << " ";
        ofs << endl;
    }
    return;
}

vector<Particles*> read_particles(string filepath) {
    ifstream ifs(filepath);
    string row_string;
    long int N;
    long int id;
    double m;
    double temp_double; 
    vector<Particles*>* particles = new vector<Particles*>;
    // read each line, first line is N
    getline(ifs, row_string);
    istringstream iss(row_string);
    iss >> N;
    for (long int i=0; i<N; i++) {
        vector<double>* pos = new vector<double>;
        vector<double>* vel = new vector<double>;
        vector<double>* acc = new vector<double>;
        getline(ifs, row_string);
        istringstream iss(row_string);
        iss >> id;
        iss >> m;
        for (int _j=0; _j<3; _j++) {
            iss >> temp_double;
            pos->push_back(temp_double);
        }
        for (int _j=0; _j<3; _j++) {
            iss >> temp_double;
            vel->push_back(temp_double);
        }
        for (int _j=0; _j<3; _j++) {
            iss >> temp_double;
            acc->push_back(temp_double);
        } 
        Particles* particle = new Particles(id, m, *pos, *vel, *acc);
        particles->push_back(particle);
    }
    return *particles;
}

void save_cells(BHNode* node_ptr) {
    if (node_ptr->isroot) {
        ofstream ofs("cells.txt");
        ofs << "";
    }
    if (node_ptr->particles.size() > 1) {
        for (BHNode* child : node_ptr->children_nodes) {
            save_cells(child);
        }
    }
    else {
        ofstream ofs("cells.txt", ios::app);
        ofs << node_ptr->cell_size << " " << node_ptr->cell_pos[0] << " " << node_ptr->cell_pos[1] << " " << node_ptr->cell_pos[2] << endl;
    }
    return;
}
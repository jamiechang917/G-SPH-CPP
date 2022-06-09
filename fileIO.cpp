#include "fileIO.h"
using namespace std;

vector<Particle*> readParticles(string filePath) {
    ifstream ifs(filePath);
    string rowString;
    long int N;
    long int id;
    double mass;
    double tempDouble;
    vector<Particle*> particles;
    // read each line, first line is N
    getline(ifs, rowString);
    istringstream iss(rowString);
    iss >> N;
    cout << "[fileIO] Read " << N << " particles..." << endl;
    for (long int i = 0; i < N; i++) {
        vec3d pos, vel, acc;
        getline(ifs, rowString);
        istringstream iss(rowString);
        iss >> id;
        iss >> mass;
        for (int j = 0; j < 3; j++) {
            iss >> tempDouble;
            pos[j] = tempDouble;
        }
        for (int j = 0; j < 3; j++) {
            iss >> tempDouble;
            vel[j] = tempDouble;
        }
        for (int j = 0; j < 3; j++) {
            iss >> tempDouble;
            acc[j] = tempDouble;
        }
        Particle* particle = new Particle(id, mass);
        particle->setPosition(pos);
        particle->setVelocity(vel);
        particle->setAccleration(acc);
        particles.push_back(particle);
    }
    return particles;
}

void saveParticles(string filePath, vector<Particle*> particles, bool saveEnergies) {
    ofstream ofs(filePath);
    if (saveEnergies) {
        // Calculate Kinectic and Potential Energy
        double KE, PE, TE = 0.0;
        for (Particle* p : particles) {
            KE += 0.5 * p->mass * (p->vel[0]*p->vel[0] + p->vel[1]*p->vel[1] + p->vel[2]*p->vel[2]);
            for (Particle* p2 : particles) {
                if (p2 != p) {
                    vec3d rVec = vecMinus(p2->pos, p->pos);
                    double r = vecNorm(rVec);
                    PE += -CONST_G * p->mass * p2->mass / sqrt(pow(r, 2.0)+pow(SOFTEN_FACTOR, 2.0));
                }
            }
        }
        PE = PE / 2;
        TE = KE + PE;
        ofs << std::fixed << std::setprecision(OUTPUT_PRECISION) << particles.size() << " " << KE << " " << PE << " " << TE << endl;
    }
    else {
        ofs << particles.size() << endl; // First line is the number of particles.
    }
    for (Particle* p : particles) {
        ofs << p->id << " ";
        ofs << std::fixed << std::setprecision(OUTPUT_PRECISION) << p->mass
            << " ";
        ofs << std::fixed << std::setprecision(OUTPUT_PRECISION) << p->pos[0]
            << " " << p->pos[1] << " " << p->pos[2] << " ";
        ofs << std::fixed << std::setprecision(OUTPUT_PRECISION) << p->vel[0]
            << " " << p->vel[1] << " " << p->vel[2] << " ";
        ofs << std::fixed << std::setprecision(OUTPUT_PRECISION) << p->acc[0]
            << " " << p->acc[1] << " " << p->acc[2] << " ";
        ofs << endl;
    }
    return;
}

void saveCells(string filePath, Node* rootNode) {
    assert(rootNode->isRootNode && "The input Node must be root node.");
    // clear file
    ofstream ofsClean(filePath);
    ofsClean << "";
    ofstream ofs(filePath, ios::app);
    // save rootNode info
    ofs << std::fixed << std::setprecision(OUTPUT_PRECISION)
        << rootNode->cellSize << " " << rootNode->cellPos[0] << " "
        << rootNode->cellPos[1] << " " << rootNode->cellPos[2] << endl;
    // save other nodes info
    for (Node* node : rootNode->allChildNodes) {
        if (node->particles.size() > 1) {
            ofs << std::fixed << std::setprecision(OUTPUT_PRECISION)
                << node->cellSize << " " << node->cellPos[0] << " "
                << node->cellPos[1] << " " << node->cellPos[2] << endl;
        }
    }
    return;
}

void saveData(Node* rootNode, string outputFolderPath, long frame,
              bool toSaveParticles, bool toSaveCells, bool toSaveEnergies) {
    // save two files: particles_{frame}.txt and cells_{frame}.txt in
    // outputFolderPath for each frame
    char particlesFilePath[100];
    char cellsFilePath[100];
    sprintf(particlesFilePath, "%s/particles_%ld.txt", outputFolderPath.c_str(),
            frame);
    sprintf(cellsFilePath, "%s/cells_%ld.txt", outputFolderPath.c_str(), frame);
    if (toSaveParticles) {
        cout << "[fileIO] Save particles (frame=" << frame << ")..." << endl;
        saveParticles(particlesFilePath, rootNode->particles, toSaveEnergies);
    }
    if (toSaveCells) {
        cout << "[fileIO] Save cells (frame=" << frame << ")..." << endl;
        saveCells(cellsFilePath, rootNode);
    }
    return;
}
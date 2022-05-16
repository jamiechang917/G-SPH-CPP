#include "tree.h"
#include <iostream>
using namespace std;

Node::Node(vector<Particle*> particles) {
    // This is the constructor for the root node.
    this->rootNode = this;
    this->parentNode = this;
    for (Particle* p : particles) {
        this->particles.push_back(p);
    }
    this->isRootNode = true;
    this->depth = 0;
    setCellMass(this->particles);

    // determine cellSize and cellPos
    assert(this->particles.size() > 1 &&
           "The particles in the root node must larger than one.");
    double maxPos = this->particles[0]->pos[0]; // maximum position of particles
    double minPos = this->particles[0]->pos[0]; // minimum position of particles
    for (Particle* p : this->particles) {
        for (int i = 0; i < 3; i++) { // x, y, z
            maxPos = MAX(maxPos, p->pos[i]);
            minPos = MIN(minPos, p->pos[i]);
        }
    }
    this->cellSize = abs(maxPos - minPos);
    this->cellPos = {minPos, minPos, minPos};
    setCellCenter(this->cellPos, this->cellSize);
    setCellCM(this->particles, this->cellMass);
}

Node::Node(Node* parentNode, vector<Particle*> particles, vec3d cellPos,
           double cellSize) {
    // This is the constructor for the normal node.
    this->rootNode = parentNode->rootNode;
    this->parentNode = parentNode;
    this->parentNode->childNodes.push_back(this);
    this->rootNode->allChildNodes.push_back(this);
    for (Particle* p : particles) {
        this->particles.push_back(p);
    }
    this->isRootNode = false;
    this->depth = this->parentNode->depth + 1;
    this->cellSize = cellSize;
    setCellMass(this->particles);
    for (int i = 0; i < 3; i++) {
        this->cellPos[i] = cellPos[i];
    }
    setCellCenter(this->cellPos, this->cellSize);
    setCellCM(this->particles, this->cellMass);
}

void Node::info() {
    cout << "\n[Tree] Node Information" << endl;
    cout << "Pointer:\t" << this << endl;
    cout << "Is Root:\t" << this->isRootNode << endl;
    cout << "Parent Node:\t" << this->parentNode << endl;
    cout << "Depth:\t\t" << this->depth << endl;
    cout << "Tree Nodes:\t" << this->rootNode->allChildNodes.size() << endl;
    cout << "Cell Mass:\t" << this->cellMass << endl;
    cout << "Cell Size:\t" << this->cellSize << endl;
    cout << "Cell Position:\t(" << this->cellPos[0] << ", " << this->cellPos[1]
         << ", " << this->cellPos[2] << ")\n";
    cout << "Cell Center:\t(" << this->cellCenter[0] << ", "
         << this->cellCenter[1] << ", " << this->cellCenter[2] << ")\n";
    cout << "Cell CM:\t(" << this->cellCM[0] << ", " << this->cellCM[1] << ", "
         << this->cellCM[2] << ")\n";

    return;
}

void Node::setCellMass(vector<Particle*> particles) {
    double totalMass = 0.0;
    for (const Particle* p : particles) {
        totalMass += p->mass;
    }
    this->cellMass = totalMass;
    return;
}

void Node::setCellCM(vector<Particle*> particles, double totalMass) {
    if (particles.size() == 0) {
        for (int i = 0; i < 3; i++) {
            cellCM[i] = 0.0;
        }
    } else {
        assert(totalMass != 0);
        vec3d cellCM = {0.0, 0.0, 0.0};
        for (const Particle* p : particles) {
            for (int i = 0; i < 3; i++) {
                cellCM[i] += p->pos[i] * p->mass / totalMass;
            }
        }
        for (int i = 0; i < 3; i++) {
            this->cellCM[i] = cellCM[i];
        }
    }
    return;
}

void Node::setCellCenter(vec3d cellPos, double cellSize) {
    for (int i = 0; i < 3; i++) {
        this->cellCenter[i] = cellPos[i] + 0.5 * cellSize;
    }
    return;
}

void buildTree(Node* node) {
    // build the barnes-hut tree
    if (node->particles.size() > 1) {
        // distribute particles into eight subcells (oct-tree)
        vector<Particle*> P1, P2, P3, P4, P5, P6, P7, P8;
        double cx = node->cellCenter[0];
        double cy = node->cellCenter[1];
        double cz = node->cellCenter[2];
        double px = node->cellPos[0];
        double py = node->cellPos[1];
        double pz = node->cellPos[2];
        for (Particle* p : node->particles) {
            double x = p->pos[0];
            double y = p->pos[1];
            double z = p->pos[2];
            if (x <= cx) {
                if (y <= cy) {
                    if (z <= cz) {
                        P1.push_back(p);
                    } else {
                        P2.push_back(p);
                    }
                } else {
                    if (z <= cz) {
                        P3.push_back(p);
                    } else {
                        P4.push_back(p);
                    }
                }
            } else {
                if (y <= cy) {
                    if (z <= cz) {
                        P5.push_back(p);
                    } else {
                        P6.push_back(p);
                    }
                } else {
                    if (z <= cz) {
                        P7.push_back(p);
                    } else {
                        P8.push_back(p);
                    }
                }
            }
        }

        // cellPos for each subcell
        vec3d C1 = {px, py, pz};
        vec3d C2 = {px, py, cz};
        vec3d C3 = {px, cy, pz};
        vec3d C4 = {px, cy, cz};
        vec3d C5 = {cx, py, pz};
        vec3d C6 = {cx, py, cz};
        vec3d C7 = {cx, cy, pz};
        vec3d C8 = {cx, cy, cz};
        // create new nodes
        Node* N1 = new Node(node, P1, C1, 0.5 * node->cellSize);
        Node* N2 = new Node(node, P2, C2, 0.5 * node->cellSize);
        Node* N3 = new Node(node, P3, C3, 0.5 * node->cellSize);
        Node* N4 = new Node(node, P4, C4, 0.5 * node->cellSize);
        Node* N5 = new Node(node, P5, C5, 0.5 * node->cellSize);
        Node* N6 = new Node(node, P6, C6, 0.5 * node->cellSize);
        Node* N7 = new Node(node, P7, C7, 0.5 * node->cellSize);
        Node* N8 = new Node(node, P8, C8, 0.5 * node->cellSize);
        buildTree(N1);
        buildTree(N2);
        buildTree(N3);
        buildTree(N4);
        buildTree(N5);
        buildTree(N6);
        buildTree(N7);
        buildTree(N8);
    }
    return;
}

void deleteTree(Node* rootNode) {
    // delete all Nodes, including itself.
    assert(rootNode->isRootNode && "The input Node must be root node.");
    for (Node* node : rootNode->allChildNodes) {
        delete node;
    }
    delete rootNode;
    return;
}

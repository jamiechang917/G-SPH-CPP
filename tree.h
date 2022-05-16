#pragma once
#include "myvector.h"
#include "particle.h"
#include <assert.h>
#include <vector>
using namespace std;

class Node {
  public:
    // constructor for the root node
    Node(vector<Particle*> particles);
    // constructor for the normal node
    Node(Node* parentNode, vector<Particle*> particles, vec3d cellPos,
         double cellSize);
    Node* rootNode;
    Node* parentNode;
    vector<Node*> childNodes;
    vector<Node*> allChildNodes;
    vector<Particle*> particles;
    bool isRootNode;
    long depth;
    double cellSize;
    double cellMass;
    vec3d cellPos;    // position of cell's corner
    vec3d cellCenter; // geometric center postion
    vec3d cellCM;     // center of mass of the cell

    void info();
    void setCellMass(vector<Particle*> particles);
    void setCellCM(vector<Particle*> particles, double totalMass);
    void setCellCenter(vec3d cellPos, double cellSize);
};

void buildTree(Node* node);
void deleteTree(Node* rootNode);

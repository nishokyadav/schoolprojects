#ifndef THETA_STAR_H
#define THETA_STAR_H

#include "A_Star.h"

using namespace std;

#include <iostream>

class Theta_Star : public A_Star
{
    friend class VisibilityGraph;
    
    public:
        double h(int x, int y);
        Theta_Star(Grid *sourceGrid, wxPoint startVal, wxPoint goalVal); 
    private:
        void updateVertex(PriorityQueue &fringe, Node current, Node &succ, Node *vertex);
        bool lineOfSight(Node current, Node &succ);
};

#include "Theta_Star.cpp"

#endif

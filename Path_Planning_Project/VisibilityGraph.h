#ifndef VISIBILITY_GRAPH_H
#define VISIBILITY_GRAPH_H

#include "Grid.h"
#include "Node.h"

class VisibilityGraph : public Grid
{
    public:
        VisibilityGraph(int width, int height, int percent, Node goalVal);
        list<wxPoint> getSucc(Node current);
        bool lineOfSight(Node current, Node &succ);
    private:
        Node goal;
};

#include "VisibilityGraph.cpp"

#endif
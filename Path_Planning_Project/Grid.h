#ifndef GRID_H
#define GRID_H

#include <map>
#include "wx/wx.h"
#include "wx/sizer.h"
#include "Node.h"

using namespace std;

class Grid
{
friend class VisibilityGraph;
    
public:
    Grid(int width, int height, int percent);
    bool isBlocked(int x, int y);
    bool isBlocked(double x, double y);
    int getRows();
    int getCols();
    int getPoints();
    virtual list<wxPoint> getSucc(Node current);
    map<int, bool> obstacles;
    
private:
    int gridRows, gridCols, gridPoints;
    int round(double x);
};

#include "Grid.cpp"

#endif
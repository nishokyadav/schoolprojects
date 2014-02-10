#ifndef A_STAR_H
#define A_STAR_H

const double MAX_VAL = 900000;

#include "wx/wx.h"
#include "wx/sizer.h"
#include "Grid.h"
#include "PriorityQueue.h"
#include "Node.h"
#include <cmath>
#include <set>
#include <map>
#include <list>

using namespace std;

class A_Star
{
    public:
        A_Star(Grid *sourceGrid, wxPoint startVal, wxPoint goalVal);  
        ~A_Star();
        list<wxPoint> getPath();
        virtual double h(int x, int y);
        map<int,double> f_val;
        map<int,double> g_val;
        void computePath();
    
    protected:
        virtual void updateVertex(PriorityQueue &fringe, Node current, Node &succ, Node *vertex);
        Grid *blocked;
        PriorityQueue *fringe;
        list<wxPoint> path;
        set<Node> closed;
        wxPoint start, goal;
        int min(int a, int b);
        int max(int a, int b);
        bool isInClosed(Node query);
};

#include "A_Star.cpp"

#endif

#ifndef A_STAR_ED_H
#define A_STAR_ED_H

#include "A_Star.h"

using namespace std;

class A_Star_ED : public A_Star
{
    public:
        A_Star_ED(Grid *sourceGrid, wxPoint startVal, wxPoint goalVal); 
        double h(int x, int y);
};

A_Star_ED::A_Star_ED(Grid *sourceGrid, wxPoint startVal, wxPoint goalVal) : A_Star(sourceGrid, startVal, goalVal)
{}

double A_Star_ED::h(int x, int y)
{
    return sqrt((x-goal.x)*(x-goal.x)+(y-goal.y)*(y-goal.y));
}

#endif

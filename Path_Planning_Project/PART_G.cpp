#include "wx/wx.h"
#include "wx/sizer.h"
#include "A_Star_ED.h"
#include "Theta_Star.h"
#include "VisibilityGraph.h"

#include <iostream>
#include <ctime> 

using namespace std;

const int ROWS = 100;
const int COLS = 50;

// g++ `wx-config --cxxflags --libs` PART_G.cpp  -o driver
// in A_Star.cpp add *100 to priority queue implementation
// 0.0730571

int main()
{
    Node goalNode;
    
    wxPoint start, goal;
    
    double diff = 0;
    
    srand(time(NULL));
    
    for(int i=0; i<50;i++)
    {
    
    start.x = rand() % (COLS+1);
	start.y = rand() % (ROWS+1);
    
	do
	{
		goal.x = rand() % (COLS+1);
		goal.y = rand() % (ROWS+1);
	} while(goal.x==start.x && goal.y==start.y);
    
    goalNode.x = goal.x;
    goalNode.y = goal.y;
    
    
    Grid testG(ROWS, COLS, 10);
    VisibilityGraph testVG(ROWS, COLS, 10, goalNode);
    testVG.obstacles = testG.obstacles;
    
    
    A_Star_ED aStarEDTrial(&testVG, start, goal);
    aStarEDTrial.computePath();
    
    
    Theta_Star tStarTrial(&testG, start, goal);
    tStarTrial.computePath();
    
    diff += tStarTrial.g_val[goal.x+goal.y*(testG.getCols() + 1)] - aStarEDTrial.g_val[goal.x+goal.y*(testG.getCols() + 1)];
        
        cout << "=====================" << endl
             << "#" << i << endl
             << "=====================" << endl;
    }
    
    cout << diff / 50 << endl;
    
    return 0;
}
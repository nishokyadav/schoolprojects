#include "wx/wx.h"
#include "wx/sizer.h"
#include "A_Star.h"
#include "Theta_Star.h"
#include "Grid.h"

#include <iostream>
#include <ctime> 

using namespace std;

const int ROWS = 50;
const int COLS = 100;

// g++ `wx-config --cxxflags --libs` PART_F.cpp  -o driver

/*
 
 No, it isn't fair to compare them because Eq 1 is a better heuristic for A*, although it is inadmissable for Theta*.  
 Theta*'s heuristic is 2x faster than A*'s heuristic.
 
 A* should use the better heuristic because it makes its running time faster than Theta* even with the slower heuristic. 
 */

int main()
{
    double totalATime = 0;
    double totalTTime = 0;
    double totalDiff = 0;
    double totalAHeuristic = 0;
    double totalTHeuristic = 0;
    
    for(int k=0; k<50; k++)
    {
    Grid testGrid(ROWS, COLS, 10);
    
    wxPoint start, goal;
    
    clock_t startTime;
    
    srand(time(NULL));
    
    start.x = rand() % COLS;
	start.y = rand() % ROWS;
    
	do
	{
		goal.x = rand() % COLS;
		goal.y = rand() % ROWS;
	} while(goal.x==start.x && goal.y==start.y);
    
    double diff = 0;
    
    startTime = clock();
    A_Star aStarTrial(&testGrid, start, goal);
    aStarTrial.computePath();
    totalATime += ( ( clock() - startTime ) / (double)CLOCKS_PER_SEC );
    //cout << "A* Time: " << ( ( clock() - startTime ) / (double)CLOCKS_PER_SEC ) << endl;
    
    
    startTime = clock();
    Theta_Star thetaStarTrial(&testGrid, start, goal);
    thetaStarTrial.computePath();
    totalTTime += ( ( clock() - startTime ) / (double)CLOCKS_PER_SEC );
    // cout << "Theta* Time: " << ( ( clock() - startTime ) / (double)CLOCKS_PER_SEC ) << endl << endl;
    
    
    for(int i = 0; i<COLS+1; i++)
	{
		for(int j = 0; j<ROWS+1; j++)
		{	
            diff += aStarTrial.h(i,j) - thetaStarTrial.h(i,j);
        }
    }
    totalDiff = diff / ((COLS+1)*(ROWS+1));
    //cout << "Difference: " << diff / ((COLS+1)*(ROWS+1)) << endl << endl;
    
    
    startTime = clock();
    
    for(int i = 0; i<COLS+1; i++)
	{
		for(int j = 0; j<ROWS+1; j++)
		{	
            thetaStarTrial.h(i, j);
        }
    }
    totalAHeuristic += ( ( clock() - startTime ) / (double)CLOCKS_PER_SEC );
    //cout << "Theta* Heuristic Time: " << ( ( clock() - startTime ) / (double)CLOCKS_PER_SEC ) << endl;
    
    
    
    startTime = clock();
    
    for(int i = 0; i<COLS+1; i++)
	{
		for(int j = 0; j<ROWS+1; j++)
		{	
            aStarTrial.h(i, j);
        }
    }
        
    totalTHeuristic += ( ( clock() - startTime ) / (double)CLOCKS_PER_SEC );
    //cout << "A* Heuristic Time: " << ( ( clock() - startTime ) / (double)CLOCKS_PER_SEC ) << endl;
    
    
    }
    
    cout << endl << endl
         << "===================================================" << endl
         << "Average A* Time                : " << totalATime / 50 << endl
         << "Average Theta* Time            : " << totalTTime / 50 << endl
         << "Average Heuristic Difference   : " << totalDiff / 50 << endl
         << "Average A* Heuristic Time      : " << totalAHeuristic / 50 << endl
         << "Average Theta* Heuristic Time  : " << totalTHeuristic / 50 << endl;
         
        
    return 0;
}
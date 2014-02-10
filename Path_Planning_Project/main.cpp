//////////////////////////////////////////////////////////////////////////
// Any Angle Path Planner                                     Version 1 //
// by Jared Rhizor and Nishok Yadav                       Feb. 10, 2012 //
//////////////////////////////////////////////////////////////////////////

#include "wx/wx.h"
#include "wx/sizer.h"
#include <sstream>
#include <list>
#include <set>
#include <stack>
#include "PriorityQueue.h"
#include "A_Star_ED.h"
#include "Theta_Star.h"
#include "Grid.h"
#include "VisibilityGraph.h"

using namespace std; 

// sizing constants

const int COLS = 100;
const int ROWS = 50;

const int BOX_SIZE = 15;
const int DOT_SIZE = 1;

const int WIDTH = BOX_SIZE * (COLS + 2);
const int HEIGHT = BOX_SIZE * (ROWS + 1) + 92;

const int POINTS = (COLS + 1) * (ROWS + 1);

const bool THETA_STAR = TRUE;
const bool A_STAR = FALSE;

class SearchPanel : public wxPanel
{
	public:
    	SearchPanel(wxFrame* parent);
    
    	void paintEvent(wxPaintEvent & evt);
		void mouseEvent(wxMouseEvent & evt);
    
    	void render(wxDC& dc);
  
    	wxButton *newConfigButton;

    	DECLARE_EVENT_TABLE()

	private:
		bool hovering;
        int hoveringNode;
	
		list<wxPoint> pathAStar, pathThetaStar;
    
        set<Node> closed;
	
		wxPoint start, goal;
	
        A_Star *aStarTrial;
        Theta_Star *thetaStarTrial;
    
		bool blocked[COLS*ROWS];
        Grid *myGrid;
    VisibilityGraph *testVG;
    
        double f_val[POINTS];
        double g_val[POINTS];
    
		void newConfigEvent(wxCommandEvent& event);
		void configure();
        bool getBlockedValue(double x, double y);
        int round(double x);
};

class MyApp: public wxApp
{
	public:
		bool OnInit();
        wxFrame *frame;
		SearchPanel * drawPane;
};

IMPLEMENT_APP(MyApp)
 
bool MyApp::OnInit()
{	
    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    frame = new wxFrame((wxFrame *)NULL, -1,  wxString("Any-Angle Path Planner"), 
                        wxPoint(50,50), wxSize(WIDTH,HEIGHT));
   
    drawPane = new SearchPanel( (wxFrame*) frame );
    sizer->Add(drawPane, 1, wxEXPAND);
   
    frame->SetSizer(sizer);
    frame->SetAutoLayout(true);
   
    frame->Show();
    return true;
} 
 
BEGIN_EVENT_TABLE(SearchPanel, wxPanel)

EVT_PAINT(SearchPanel::paintEvent)
EVT_LEFT_UP(SearchPanel::mouseEvent)

END_EVENT_TABLE()
 

SearchPanel::SearchPanel(wxFrame* parent) :
wxPanel(parent)
{	
	// random number init
	srand( time( NULL ) );
	
	// create button and events
	newConfigButton = new wxButton(this, wxID_PROPERTIES, wxString("New Configuration"), wxPoint(WIDTH - 170, HEIGHT - 71));
	Connect(wxID_PROPERTIES, wxEVT_COMMAND_BUTTON_CLICKED, 
	      wxCommandEventHandler(SearchPanel::newConfigEvent) );
		
	// show stuff on command line
	cout << endl
		 << "+=====================================+" << endl
		 << "| Any-Angle Path Planner              |" << endl
		 << "+-------------------------------------+" << endl
		 << "| by Jared Rhizor and Nishok Yadav    |" << endl
		 << "+=====================================+" << endl << endl;

	// just get user input
	configure();
	
	// somehow handle what the start and end positions are
}
 

void SearchPanel::paintEvent(wxPaintEvent & evt)
{	
    wxPaintDC dc(this);
    render(dc);
}


void SearchPanel::mouseEvent(wxMouseEvent & evt)
{
	wxPoint mousePos = wxGetMousePosition();
	mousePos = this->ScreenToClient(mousePos);
		
	hovering = false;
	
	for(int i = 0; i<COLS+1 && hovering==false; i++)
	{
		for(int j = 0; j<ROWS+1 && hovering==false; j++)
		{	
			// check if mouse is hovering over a point
			if(mousePos.x >= BOX_SIZE+BOX_SIZE*i-7 && mousePos.x <= BOX_SIZE+BOX_SIZE*i+7 &&
			   mousePos.y >= BOX_SIZE+BOX_SIZE*j-7 && mousePos.y <= BOX_SIZE+BOX_SIZE*j+7)
			{
				// notify that it is hovering
				hovering = true;
				hoveringNode = j*(COLS+1)+i;
			}
		}
	}
	
    Update();
	Refresh();
}
 

void SearchPanel::render(wxDC&  dc)
{	
	// set background to white
	dc.SetBrush( wxBrush( wxColor(255,255,255) ) );
	dc.SetPen( wxPen( wxColor(255,255,255), 1 ) ); 
    dc.DrawRectangle( 0, 0, WIDTH, HEIGHT );

	// draw obstacles
	dc.SetBrush(wxBrush( wxColor(200,200,200) ) ); 
	dc.SetPen( wxPen( wxColor(0,0,0), 0 ) );
	
	for(int i = 0; i<COLS; i++)
	{
		for(int j = 0; j<ROWS; j++)
		{	
			if(myGrid->isBlocked(i,j))
			{
                dc.DrawRectangle( BOX_SIZE+BOX_SIZE*i,BOX_SIZE+BOX_SIZE*j, BOX_SIZE, BOX_SIZE );
			}
		}
	}
	
    // draw A* path
    
    dc.SetPen( wxPen( wxColor(204,85,0), 1 ) );
    
    list<wxPoint> pathBackup = pathAStar;
    
    if(!pathBackup.empty())
    {
        wxPoint first, next;
        first = pathBackup.front();
        pathBackup.pop_front();
        
        while(!pathBackup.empty())
        {
            next = pathBackup.front();
            pathBackup.pop_front();
            
            // SCALE THESE POINTS
            
            first.x = first.x * BOX_SIZE + BOX_SIZE;
            next.x = next.x * BOX_SIZE + BOX_SIZE;
            
            first.y = first.y * BOX_SIZE + BOX_SIZE;
            next.y = next.y * BOX_SIZE + BOX_SIZE;
            
            dc.DrawLine(first, next);
           
            next.x = (next.x - BOX_SIZE)/BOX_SIZE;
            next.y = (next.y - BOX_SIZE)/BOX_SIZE;
            
            first = next;
        }
    } 
    
    // draw Theta* path
    dc.SetPen( wxPen( wxColor(153,50,204), 1 ) );
    
    pathBackup = pathThetaStar;
    
    if(!pathBackup.empty())
    {
        wxPoint first, next;
        first = pathBackup.front();
        pathBackup.pop_front();
        
        while(!pathBackup.empty())
        {
            next = pathBackup.front();
            pathBackup.pop_front();
            
            // SCALE THESE POINTS
            
            first.x = first.x * BOX_SIZE + BOX_SIZE;
            next.x = next.x * BOX_SIZE + BOX_SIZE;
            
            first.y = first.y * BOX_SIZE + BOX_SIZE;
            next.y = next.y * BOX_SIZE + BOX_SIZE;
            
            dc.DrawLine(first, next);
            
            next.x = (next.x - BOX_SIZE)/BOX_SIZE;
            next.y = (next.y - BOX_SIZE)/BOX_SIZE;
            
            first = next;
        }
    }
    
	// print circles
	dc.SetPen( wxPen( wxColor(0,0,0), 0 ) );
	
	for(int i = 0; i<COLS+1; i++)
	{
		for(int j = 0; j<ROWS+1; j++)
		{
			if(start.x == i && start.y == j)
			{
				dc.SetBrush(wxColor(30, 185, 30)); // green
				dc.DrawCircle( wxPoint(BOX_SIZE+BOX_SIZE*i,BOX_SIZE+BOX_SIZE*j), DOT_SIZE * 4 );
                
			}
			else if(goal.x == i && goal.y == j)
			{
				dc.SetBrush(*wxRED_BRUSH); // red
				dc.DrawCircle( wxPoint(BOX_SIZE+BOX_SIZE*i,BOX_SIZE+BOX_SIZE*j), DOT_SIZE * 4 );
			}
			else
			{
				dc.SetBrush(*wxBLACK_BRUSH);
				dc.DrawCircle( wxPoint(BOX_SIZE+BOX_SIZE*i,BOX_SIZE+BOX_SIZE*j), DOT_SIZE );
			}
			
			if(hovering && hoveringNode==j*(COLS+1)+i)
			{
				dc.SetBrush(wxTransparentColour);
				dc.SetPen( wxPen( wxColor(72, 118, 255), DOT_SIZE * 2 ) ); // blue
				dc.DrawCircle( wxPoint(BOX_SIZE+BOX_SIZE*i,BOX_SIZE+BOX_SIZE*j), DOT_SIZE * 8 );
				dc.SetPen( wxPen( wxColor(0,0,0), 0 ) );
			}
		}
	}
	
	// if set, draw f,g,h values
	dc.SetTextForeground( wxColor(50,50,50) );
    
    dc.SetFont(wxFont(wxSize(10,10), wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    dc.DrawText(wxString("A*"), BOX_SIZE*1 + 3, BOX_SIZE * (ROWS + 2) + 5);
    
    dc.SetFont(wxFont(wxSize(9,9), wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    dc.DrawText(wxString("\u03B8*"), BOX_SIZE*12 + 3, BOX_SIZE * (ROWS + 2) + 5);
    
	if(hovering)
	{
        //int x = hoveringNode % (COLS+1);
        //int y = (hoveringNode-x)/(COLS+1);
        //int hN2 = x+y*COLS;
        int hN2 = hoveringNode;
        
		stringstream aStarInfostrm, tStarInfostrm;
		string displayString;
        
        dc.SetFont(wxFont(wxSize(10,10), wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
        
        // A*
        if(aStarTrial->f_val.count(hN2)==0 || aStarTrial->f_val[hN2]>MAX_VAL - 1)
        {
            aStarInfostrm << "f=" << "\u221E" << "\n"
                     << "g=" << "\u221E" << "\n"
                     << "h=" << aStarTrial->h(hoveringNode % (COLS + 1), hoveringNode / (COLS + 1)) << "\n";
        }
        
        else
        {
            aStarInfostrm << "f=" << aStarTrial->f_val[hN2] << "\n"
                     << "g=" << aStarTrial->g_val[hN2] << "\n" 
                     << "h=" << aStarTrial->h(hoveringNode % (COLS + 1), hoveringNode / (COLS + 1)) << "\n";
        }
		
		displayString = aStarInfostrm.str();
		
		dc.DrawText(wxString(displayString), BOX_SIZE*3 + 3, BOX_SIZE * (ROWS + 2) + 5);
        
        // Theta*
        if(thetaStarTrial->f_val.count(hN2)==0 || thetaStarTrial->f_val[hN2]>MAX_VAL - 1)
        {
            tStarInfostrm << "f=" << "\u221E" << "\n"
            << "g=" << "\u221E" << "\n"
            << "h=" << thetaStarTrial->h(hoveringNode % (COLS + 1), hoveringNode / (COLS + 1)) << "\n";
        }
        
        else
        {
            tStarInfostrm << "f=" << thetaStarTrial->f_val[hN2] << "\n"
            << "g=" << thetaStarTrial->g_val[hN2] << "\n" 
            << "h=" << thetaStarTrial->h(hoveringNode % (COLS + 1), hoveringNode / (COLS + 1)) << "\n";
        }
		
		displayString = tStarInfostrm.str();
		
		dc.DrawText(wxString(displayString), BOX_SIZE*14 + 3, BOX_SIZE * (ROWS + 2) + 5);
	}
	else
	{
        dc.SetFont(wxFont(wxSize(10,10), wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
        dc.DrawText(wxString("f=\ng=\nh="), BOX_SIZE*14 + 3, BOX_SIZE * (ROWS + 2) + 5);
		dc.DrawText(wxString("f=\ng=\nh="), BOX_SIZE*3 + 3, BOX_SIZE * (ROWS + 2) + 5);
	}
}

void SearchPanel::newConfigEvent(wxCommandEvent& event)
{	
	configure();
	
	Update();
	Refresh();
}

void SearchPanel::configure()
{	
    if(myGrid!=NULL)
    {
        delete myGrid;
        delete aStarTrial;
        delete thetaStarTrial;
    }
    
    myGrid = new Grid(ROWS, COLS, 10); 
    
    hovering = false;
    
    
	start.x = rand() % COLS;
	start.y = rand() % ROWS;
    
	do
	{
		goal.x = rand() % COLS;
		goal.y = rand() % ROWS;
	} while(goal.x==start.x && goal.y==start.y);
    
    ////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////
    cout << "start: " << start.x << " " << start.y << endl
    << "goal: " << goal.x << " " << goal.y << endl;
    
    
    aStarTrial = new A_Star(myGrid, start, goal);
    aStarTrial->computePath();
    pathAStar = aStarTrial->getPath();
    
/*    
     // show visibility graph vs theta*
    Node goalNode;
    goalNode.x = goal.x;
    goalNode.y = goal.y;
    
    testVG = new VisibilityGraph(ROWS,COLS,10, goalNode);
    testVG->obstacles = myGrid->obstacles;
    
    aStarTrial = new A_Star_ED(testVG, start, goal);
    aStarTrial->computePath();
    pathAStar = aStarTrial->getPath();
  */  
    
    thetaStarTrial = new Theta_Star(myGrid, start, goal);
    thetaStarTrial->computePath();
    pathThetaStar = thetaStarTrial->getPath();
    
	cout << "Grid generated." << endl << endl << endl << endl;
    
}
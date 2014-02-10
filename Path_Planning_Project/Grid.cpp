// srand must be initialized outside
Grid::Grid(int width, int height, int percent)
{
    gridRows = width;
    gridCols = height;
    gridPoints = (gridRows+1) * (gridCols+1);
    
    // process obstacles
	for(int i=0; percent!=0 && i<gridRows*gridCols; i++) 
	{
		if(rand() % 100 <= percent)
		{
            obstacles[i] = true;
		}
	}
}

bool Grid::isBlocked(int x, int y)
{
    return obstacles[x+y*gridCols];
}

bool Grid::isBlocked(double x, double y)
{
    return obstacles[round(x)+round(y)*gridCols];
}

int Grid::round(double x)
{
    if(x-floor(x)>=0.5)
    {
        return ceil(x);
    }
    else
    {
        return floor(x);
    }
}

int Grid::getRows()
{
    return gridRows;
}

int Grid::getCols()
{
    return gridCols;
}

int Grid::getPoints()
{
    return gridPoints;
}

list<wxPoint> Grid::getSucc(Node current)
{
    bool top_blocked, bot_blocked, left_blocked, right_blocked;
    bool  top_right_blocked, top_left_blocked, bot_left_blocked, bot_right_blocked;
    bool on_top, on_bot, on_left, on_right;
    list<wxPoint> offsets;
    
    on_top = current.y == 0;
    on_bot = current.y == getRows();
    on_left = current.x == 0;
    on_right = current.x == getCols();
    
    top_right_blocked = isBlocked(current.x, current.y-1);
    top_left_blocked = isBlocked(current.x-1, current.y-1);
    bot_left_blocked = isBlocked(current.x-1, current.y);
    bot_right_blocked = isBlocked(current.x, current.y);
    
    top_blocked = top_right_blocked && top_left_blocked;
    bot_blocked = bot_right_blocked && bot_left_blocked;
    left_blocked = top_left_blocked && bot_left_blocked;
    right_blocked = top_right_blocked && bot_right_blocked;
    
    // up
    if(!on_top && !top_blocked)
        offsets.push_front(wxPoint(current.x+0,current.y-1));
    
    // up right
    if(!on_top && !on_right && !top_right_blocked)
        offsets.push_front(wxPoint(current.x+1,current.y-1));
    
    // up left
    if(!on_top && !on_left && !top_left_blocked)
        offsets.push_front(wxPoint(current.x-1,current.y-1));
    
    // right
    if(!on_right && !right_blocked)
        offsets.push_front(wxPoint(current.x+1,current.y+0));
    
    // down right
    if(!on_bot && !on_right && !bot_right_blocked)
        offsets.push_front(wxPoint(current.x+1,current.y+1));
    
    // down
    if(!on_bot && !bot_blocked)
        offsets.push_front(wxPoint(current.x+0,current.y+1));
    
    // down left
    if(!on_bot && !on_left && !bot_left_blocked)
        offsets.push_front(wxPoint(current.x-1,current.y+1));
    
    // left
    if(!on_left && !left_blocked)
        offsets.push_front(wxPoint(current.x-1,current.y+0));
    
    // return points to go to
    return offsets;
}
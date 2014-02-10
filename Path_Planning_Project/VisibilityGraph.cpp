VisibilityGraph::VisibilityGraph(int width, int height, int percent, Node goalVal) : Grid(width, height, percent)
{
    goal = goalVal;
}

bool VisibilityGraph::lineOfSight(Node current, Node &succ)
{   
    //cout << current.x << " " << current.y << endl;
    
    int x_0 = current.x;
    int y_0 = current.y;
    int x_1 = succ.x;
    int y_1 = succ.y;
    
    double f = 0;
    
    int d_y = y_1 - y_0;
    int d_x = x_1 - x_0;
    
    if(d_y<0)
    {
        d_y *= -1;
        current.y = -1;
    }
    else
    {
        current.y = 1;
    }
    
    if(d_x<0)
    {
        d_x *= -1;
        current.x = -1;
    }
    else
    {
        current.x = 1;
    }
    
    if(d_x >= d_y)
    {
        while(x_0 != x_1)
        {
            f += d_y;
            if(f >= d_x)
            {
                if(isBlocked(x_0 + ((current.x-1)/2.0),y_0+((current.y-1)/2.0)))
                {
                    return false;
                }
                y_0 += current.y;
                f -= d_x;
            }
            
            if(f != 0 && isBlocked(x_0 + ((current.x-1)/2.0),y_0+((current.y-1)/2.0)))
            {
                return false;
            }
            
            if(d_y == 0 && isBlocked(x_0+((current.x-1)/2.0),y_0+0.0) && isBlocked(x_0+((current.x-1)/2.0), y_0-1.0))
            {
                return false;
            }
            
            x_0 += current.x;
        }
    }
    
    else
    {
        while(y_0 != y_1)
        {
            f += d_x;
            if(f >= d_y)
            {
                if(isBlocked(x_0 + ((current.x-1)/2.0),y_0+((current.y-1)/2.0)))
                {
                    return false;
                }
                x_0 += current.x;
                f -= d_y;
            }
            
            if(f != 0 && isBlocked(x_0 + ((current.x-1)/2.0),y_0+((current.y-1)/2.0)))
            {
                return false;
            }
            
            if(d_x == 0 && isBlocked(x_0+0.0,y_0+((current.y-1)/2.0)) && isBlocked(x_0-1.0,y_0+((current.y-1)/2.0)))
            {
                return false;
            }
            
            y_0 += current.y;
        }
    }
    
    return true;
}


list<wxPoint> VisibilityGraph::getSucc(Node current)
{
    list<wxPoint> children;
    
    Node temp;
    int x,y;
    
    map<int, bool>::iterator it;
    for(it=obstacles.begin(); it != obstacles.end(); it++)
    {
        x = it->first % getCols();
        y = (it->first - x) / getCols();
        
        temp.x = x;
        temp.y = y;
        if(lineOfSight(temp, current))
            children.push_front(wxPoint(temp.x, temp.y));
        
        temp.x = x+1;
        temp.y = y;
        if(lineOfSight(temp, current))
            children.push_front(wxPoint(temp.x, temp.y));
        
        temp.x = x;
        temp.y = y+1;
        if(lineOfSight(temp, current))
            children.push_front(wxPoint(temp.x, temp.y));
        
        temp.x = x+1;
        temp.y = y+1;
        if(lineOfSight(temp, current))
            children.push_front(wxPoint(temp.x, temp.y));
        
        if(lineOfSight(goal, current))
            children.push_front(wxPoint(goal.x, goal.y));
    }
    
    return children;
}

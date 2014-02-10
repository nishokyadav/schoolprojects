Theta_Star::Theta_Star(Grid *sourceGrid, wxPoint startVal, wxPoint goalVal) : A_Star(sourceGrid, startVal, goalVal)
{}

void Theta_Star::updateVertex(PriorityQueue &fringe, Node current, Node &succ, Node *vertex)
{
    if(current.parent != NULL && lineOfSight(*current.parent, succ))
    {
        // Path 2 
        double g_current = g_val[current.parent->x+current.parent->y*(blocked->getCols()+1)];
        double cost = sqrt( (succ.x-current.parent->x) * (succ.x-current.parent->x) 
                           + (succ.y-current.parent->y) * (succ.y-current.parent->y) );
        if(cost + g_current < g_val[succ.x+succ.y*(blocked->getCols()+1)])
        {
            g_val[succ.x+succ.y*(blocked->getCols()+1)] = cost + g_current;
            
            succ.h = h(succ.x,succ.y);
            succ.f = g_val[succ.x+succ.y*(blocked->getCols()+1)]+succ.h;
            f_val[succ.x+succ.y*(blocked->getCols()+1)] = succ.f;
            
            succ.parent = vertex->parent;
            
            // we didn't do this before either -> remove from fringe
            fringe.enqueue(succ);
        }
        
    }
    
    else
    {
        
        double g_current = g_val[current.x+current.y*(blocked->getCols()+1)];
        double cost = sqrt( (succ.x-current.x) * (succ.x-current.x) 
                           + (succ.y-current.y) * (succ.y-current.y) );
        
        if(g_current+cost < g_val[succ.x+succ.y*(blocked->getCols()+1)])
        {
            g_val[succ.x+succ.y*(blocked->getCols()+1)] = g_current+cost;
            
            succ.h = h(succ.x,succ.y);
            succ.f = g_val[succ.x+succ.y*(blocked->getCols()+1)]+succ.h;
            
            succ.parent = vertex;
            
            //cout << succ.f << endl; 
            
            if(succ.f < f_val[succ.x+succ.y*(blocked->getCols()+1)])
            {
                fringe.enqueue(succ);
                //cout << "enqueued " << succ.x << " " << succ.y << endl;
                f_val[succ.x+succ.y*(blocked->getCols()+1)] = succ.f;
            }
        }
        
        
    }
}

bool Theta_Star::lineOfSight(Node current, Node &succ)
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
                if(blocked->isBlocked(x_0 + ((current.x-1)/2.0),y_0+((current.y-1)/2.0)))
                {
                    return false;
                }
                y_0 += current.y;
                f -= d_x;
            }
            
            if(f != 0 && blocked->isBlocked(x_0 + ((current.x-1)/2.0),y_0+((current.y-1)/2.0)))
            {
                return false;
            }
            
            if(d_y == 0 && blocked->isBlocked(x_0+((current.x-1)/2.0),y_0+0.0) && blocked->isBlocked(x_0+((current.x-1)/2.0), y_0-1.0))
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
                if(blocked->isBlocked(x_0 + ((current.x-1)/2.0),y_0+((current.y-1)/2.0)))
                {
                    return false;
                }
                x_0 += current.x;
                f -= d_y;
            }
            
            if(f != 0 && blocked->isBlocked(x_0 + ((current.x-1)/2.0),y_0+((current.y-1)/2.0)))
            {
                return false;
            }
            
            if(d_x == 0 && blocked->isBlocked(x_0+0.0,y_0+((current.y-1)/2.0)) && blocked->isBlocked(x_0-1.0,y_0+((current.y-1)/2.0)))
            {
                return false;
            }
            
            y_0 += current.y;
        }
    }
    
    return true;
}

double Theta_Star::h(int x, int y)
{
    return sqrt( (goal.x-x)*(goal.x-x) + (goal.y-y)*(goal.y-y) );
}
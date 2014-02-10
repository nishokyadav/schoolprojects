A_Star::A_Star(Grid *sourceGrid, wxPoint startVal, wxPoint goalVal)
{
    blocked = sourceGrid;
    start = startVal;
    goal = goalVal;
    fringe = new PriorityQueue(blocked->getPoints());
}

void A_Star::computePath()
{
    // The Solution to All of our Problems
    list<Node*> allocedNodes;
    
    Node* vertex;
    
    Node current, succ;
    current.x = start.x;
    current.y = start.y;
    current.g = 0;
    current.f = h(current.x, current.y);
    current.h = h(current.x, current.y);
    
    current.h = h(current.x, current.y);
    
    current.f = current.g + current.h;
    current.parent = NULL;
    
    PriorityQueue fringe(blocked->getPoints());
    
    fringe.enqueue(current);
    
    
    f_val[current.x+current.y*(blocked->getCols()+1)] = current.f;
    g_val[current.x+current.y*(blocked->getCols()+1)] = current.g;
    
    list<wxPoint> offsets;
    
    closed.clear();
    
    // check if endpoint is surrounded
    Node goalNode;
    goalNode.x = goal.x;
    goalNode.y = goal.y;
    
    offsets = blocked->getSucc(goalNode);
    
    if(offsets.empty())
    {
        cout << "PATH NOT FOUND" << endl;
        return;
    }
    
    // end checking for surrounded goal
    
    
    while(!fringe.isEmpty())
    {
        fringe.dequeue(current);
        
        vertex = new Node;
        *vertex = current;
        allocedNodes.push_front(vertex);
        
        if(current.x == goal.x && current.y == goal.y)
        {
            list<wxPoint> blank;
            
            cout << "FOUND" << endl;
            
            while(current.x!=start.x || current.y!=start.y)
            {
                cout << current.x << " " << current.y << endl;
                blank.push_front(wxPoint(current.x, current.y));
                current = *(current.parent);
            }
            
            blank.push_front(wxPoint(current.x, current.y));
            
            cout << "PROCESSED" << endl;
            
            closed.clear();
            
            // dealloc memory
            list<Node*>::iterator it;
            for(it=allocedNodes.begin(); it != allocedNodes.end(); it++)
            {
                delete *it;
            }
            
            path = blank;
            return;
        }
        
        closed.insert(current);
        
        // determine valid successors
        offsets = blocked->getSucc(current);
        
        // go through offsets and set default f and g values
        
        list<wxPoint>::iterator it;
        
        for(it=offsets.begin(); it!=offsets.end(); it++)
        {
            if(f_val.count(it->x+(it->y)*(blocked->getCols()+1))==0)
            {
                f_val[it->x+(it->y)*(blocked->getCols()+1)] = MAX_VAL;
                g_val[it->x+(it->y)*(blocked->getCols()+1)] = MAX_VAL;
            }
        }
        
        // foreach successor
        while(!offsets.empty())
        {            
            succ = current;
            succ.x = offsets.front().x;
            succ.y = offsets.front().y;
            
            // if s' is not in closed
            if(!isInClosed(succ))
            {
                // if s' is not in fringe
                if(!fringe.find(succ))
                {
                    succ.g = MAX_VAL;
                }
                
                updateVertex(fringe, current, succ, vertex);
            }
            
            offsets.pop_front();
        }
        
    }    
    
    cout << "PATH NOT FOUND" << endl;
}

A_Star::~A_Star()
{
    delete fringe;
}

list<wxPoint> A_Star::getPath()
{
    return path;
}

void A_Star::updateVertex(PriorityQueue &fringe, Node current, Node &succ, Node *vertex)
{
    double cost = sqrt( (succ.x-current.x) * (succ.x-current.x) 
                       + (succ.y-current.y) * (succ.y-current.y) );
    if(current.g+cost < succ.g)
    {
        succ.g = current.g+cost;
        
        succ.h = h(succ.x,succ.y);
        succ.f = succ.g + succ.h;
        
        succ.parent = vertex;
        
        /*
         if(fringe.find(succ))
         {
         Node x;
         // removed from suspect list by FBI. Still on CIA list. consider other methods.
         fringe.dequeue(x);
         }
         */
        
        if(succ.f < f_val[succ.x+succ.y*(blocked->getCols()+1)])
        {
            fringe.enqueue(succ);
            f_val[succ.x+succ.y*(blocked->getCols()+1)] = succ.f;
            
            // not part of a*, just helps us display
            g_val[succ.x+succ.y*(blocked->getCols()+1)] = succ.g;
        }
    }
}

double A_Star::h(int x, int y)
{
    double h_val = 0;
    
    h_val += sqrt(2) * min(abs(x-goal.x),abs(y-goal.y));
    h_val += max(abs(x-goal.x),abs(y-goal.y));
    h_val -= min(abs(x-goal.x),abs(y-goal.y));
    
    return h_val;
}

int A_Star::min(int a, int b)
{
    if(a<b)
    {
        return a;
    }
    else
    {
        return b;
    }
}

int A_Star::max(int a, int b)
{
    if(a>b)
    {
        return a;
    }
    else
    {
        return b;
    }
}

bool A_Star::isInClosed(Node query)
{
    set<Node>::iterator it;
    
    for(it=closed.begin(); it!=closed.end(); it++)
    {
        if(it->x == query.x && it->y == query.y)
        {
            return true;
        }
    }
    
    return false;
}
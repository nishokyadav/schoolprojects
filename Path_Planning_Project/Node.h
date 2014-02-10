#ifndef NODE_STRUCT
#define NODE_STRUCT

#include<list>
#include <cmath>
#include <set>

using namespace std;

struct Node
{
    int x, y;

    double f, g, h;

    bool operator >=( const Node &rhs );  
    bool operator > ( const Node &rhs );  
    bool operator < ( const Node &rhs ) const;
    
    bool operator ==( const Node &rhs );
    
    Node* parent; 
};

bool Node::operator>=( const Node &rhs )
{
   return f >= rhs.f;
}

bool Node::operator>( const Node &rhs )
{
   return f > rhs.f;
}

bool Node::operator<( const Node &rhs ) const
{
    return f < rhs.f;
}

bool Node::operator==( const Node &rhs )
{
    return x==rhs.x && y==rhs.y;
}

#endif

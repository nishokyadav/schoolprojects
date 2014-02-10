#ifndef HEAP_H
#define HEAP_H

// This is a min-heap

using namespace std;

#include "Node.h"

class Heap
{
  public:
     Heap(int);
     ~Heap();
     void reheapDown(int, int);
     void reheapUp(int, int);  
     void swap(int, int);

     bool operator>=(const Node &rhs);
     bool operator>(const Node &rhs);
     Node* elements; 
     int numElements;
};

#include "Heap.cpp"

#endif

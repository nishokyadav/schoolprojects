#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

using namespace std;

// This is a min-priority queue

#include "Heap.h"
#include <cmath>

class PriorityQueue
{
   public:
      PriorityQueue(int);
      ~PriorityQueue();
      void makeEmpty();
      bool isEmpty() const;
      bool isFull() const;
      void enqueue( Node );
      void dequeue( Node& );
      bool find( const Node& );
   private:
      int numItems;
      Heap* heap;
      int maxItems;
};

#include "PriorityQueue.cpp"

#endif
 
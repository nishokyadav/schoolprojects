PriorityQueue::PriorityQueue(int size)
{
   maxItems = size;
   heap = new Heap(size);
   numItems = 0;
}

PriorityQueue::~PriorityQueue()
{
   delete heap;
}

void PriorityQueue::makeEmpty()
{
  numItems = 0;
    delete heap;
    heap = new Heap(maxItems);
}

bool PriorityQueue::isEmpty() const
{
   return( numItems == 0 );
}

bool PriorityQueue::isFull() const
{
   return numItems == maxItems;
}

void PriorityQueue::enqueue( Node useMe )
{
 numItems++;
 heap->elements[numItems-1] = useMe;
 heap->reheapUp(0, numItems-1);
}

void PriorityQueue::dequeue( Node &useMe )
{
 useMe = heap->elements[0];
 heap->elements[0] = heap->elements[numItems-1];
 numItems--;
 heap->reheapDown(0, numItems-1);
}

bool PriorityQueue::find(const Node &query)
{
    for(int i=0; i<heap->numElements; i++)
    {
        if(heap->elements[i].x == query.x && 
           heap->elements[i].y == query.y)
        {
            return true;
        }
    }
    return false;
}
Heap::Heap(int size)
{
    elements = new Node[size];  
    numElements = 0;
}

Heap::~Heap()
{
    delete [] elements;
}

void Heap::reheapDown(int root, int bottom)
{
   int minChild, rightChild, leftChild;

   leftChild = 2*root+1;
   rightChild = 2*root+2;

   if(leftChild <= bottom)
   {  // left child is part of the heap
       if(leftChild == bottom) // only one child
            minChild = leftChild;
       else 
       { // two children
            if(elements[leftChild] >= elements[rightChild])
                 minChild = rightChild;
            else
                 minChild = leftChild;
       }
   if(elements[root] > elements[minChild]) 
   {
       swap(root, minChild);
       reheapDown(minChild, bottom);
   }
  }
} 

void Heap::reheapUp(int root, int bottom)
{
   int parent;

   if(bottom > root)
   {
    parent = (bottom - 1)/2;
    
    if(elements[parent] > elements[bottom])
    {
     swap(parent, bottom);
     reheapUp(root, parent);
    }
   }
}

void Heap::swap(int top, int bottom)
{
   Node tmp;

   tmp = elements[top];
   elements[top] = elements[bottom];
   elements[bottom] = tmp;
}



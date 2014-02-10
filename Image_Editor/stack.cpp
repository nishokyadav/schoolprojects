using namespace std;

#include "stack.h"

template<class ItemType>
StackType<ItemType>::StackType()
{
 top = -1;
}

template<class ItemType>
void StackType<ItemType>::MakeEmpty()
{	
 top = -1;
}

template<class ItemType>
bool StackType<ItemType>::IsEmpty() const
{
 return (top == -1);
}

template<class ItemType>
bool StackType<ItemType>::IsFull() const
{
 return (top == MAX_ITEMS-1);
}

template<class ItemType>
void StackType<ItemType>::Push(ItemType newItem)
{
 top++;
 items[top] = newItem;
}

template<class ItemType>
void StackType<ItemType>::Pop(ItemType& item)
{
 item = items[top];
 top--;
}



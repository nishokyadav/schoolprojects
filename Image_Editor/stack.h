#ifndef STACK_H
#define STACK_H

int MAX_ITEMS;

template<class ItemType>
class StackType
{
 public:
    StackType();
    void MakeEmpty();
	 bool IsEmpty() const;
    bool IsFull() const;
    void Push(ItemType);
    void Pop(ItemType&);
private:
    int top;
    ItemType items[MAX_ITEMS];
};     

#endif

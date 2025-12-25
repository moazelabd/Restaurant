#ifndef LINKED_QUEUE_
#define LINKED_QUEUE_


#include "Node.h"
#include "QueueADT.h"
#include <iostream>
using namespace std;
template <typename T>
class LinkedQueue :public QueueADT<T>
{
private:
	Node<T>* backPtr;
	Node<T>* frontPtr;
	int count;
public:
	LinkedQueue();
    LinkedQueue& operator=(LinkedQueue& other);
    // Transfer ownership of nodes from other to this
    void moveFrom(LinkedQueue<T>& other) {
        // clear current
        T tmp;
        while (dequeue(tmp));
        // steal
        frontPtr = other.frontPtr;
        backPtr = other.backPtr;
        count = other.count;
        other.frontPtr = nullptr;
        other.backPtr = nullptr;
        other.count = 0;
    }
	bool isEmpty() const;
	bool enqueue(const T& newEntry);
	bool dequeue(T& frntEntry);
	void circularQueueOnce();
	void display() const;
	bool peek(T& frntEntry)  const;
	int GetCount() const;


	~LinkedQueue();
};
/////////////////////////////////////////////////////////////////////////////////////////

/*
Function: Queue()
The constructor of the Queue class.

*/
template <typename T>
LinkedQueue<T> queueToStack(LinkedQueue<T>& q)
{
	LinkedQueue<T> stackLikeQueue;
	LinkedQueue<T> temp;
	T item;

	while (q.dequeue(item)) {
		stackLikeQueue.enqueue(item);
		temp.enqueue(item);
	}


	while (temp.dequeue(item)) {
		q.enqueue(item);
	}


	return stackLikeQueue;
}

template <typename T>
void LinkedQueue<T>::display() const
{
	Node<T>* current = frontPtr;
	cout << "Queue contents (front ? rear): ";
	while (current != nullptr)
	{
		cout << current->getItem() << " ";
		current = current->getNext();
	}
	cout << endl;
}


template <typename T>
void LinkedQueue<T>::circularQueueOnce() {
	if (isEmpty())
		return;

	T frontItem;
	dequeue(frontItem);   // remove from front
	enqueue(frontItem);   // add to rear
}


template <typename T>
LinkedQueue<T>::LinkedQueue()
{
	backPtr = nullptr;
	frontPtr = nullptr;
	count = 0;
}

// Move-assignment: transfer ownership from other to this
template <typename T>
LinkedQueue<T>& LinkedQueue<T>::operator=(LinkedQueue<T>& other)
{
    if (this == &other) return *this;
    // clear current
    T tmp;
    while (dequeue(tmp));
    // steal pointers
    frontPtr = other.frontPtr;
    backPtr = other.backPtr;
    count = other.count;
    other.frontPtr = nullptr;
    other.backPtr = nullptr;
    other.count = 0;
    return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////

/*
Function: isEmpty
Sees whether this queue is empty.

Input: None.
Output: True if the queue is empty; otherwise false.
*/
template <typename T>
bool LinkedQueue<T>::isEmpty() const
{
	return (frontPtr == nullptr);
}

/////////////////////////////////////////////////////////////////////////////////////////

/*Function:enqueue
Adds newEntry at the back of this queue.

Input: newEntry .
Output: True if the operation is successful; otherwise false.
*/

template <typename T>
bool LinkedQueue<T>::enqueue(const T& newEntry)
{
	Node<T>* newNodePtr = new Node<T>(newEntry);
	if (isEmpty())
		frontPtr = newNodePtr;
	else
		backPtr->setNext(newNodePtr);
	backPtr = newNodePtr;
	count++; // increment count
	return true;
}
// end enqueue


/////////////////////////////////////////////////////////////////////////////////////////////////////////

/*Function: dequeue
Removes the front of this queue. That is, removes the item that was added
earliest.

Input: None.
Output: True if the operation is successful; otherwise false.
*/

template <typename T>
bool LinkedQueue<T>::dequeue(T& frntEntry)
{
	if (isEmpty())
		return false;
	Node<T>* nodeToDeletePtr = frontPtr;
	frntEntry = frontPtr->getItem();
	frontPtr = frontPtr->getNext();
	if (nodeToDeletePtr == backPtr)
		backPtr = nullptr;
	delete nodeToDeletePtr;
	count--; // decrement count
	return true;
}




/////////////////////////////////////////////////////////////////////////////////////////

/*
Function: peek
copies the front of this queue to the passed param. The operation does not modify the queue.

Input: None.
Output: The front of the queue.
*/

template <typename T>
bool LinkedQueue<T>::peek(T& frntEntry) const
{
	if (isEmpty())
		return false;

	frntEntry = frontPtr->getItem();
	return true;

}
///////////////////////////////////////////////////////////////////////////////////

template <typename T>
LinkedQueue<T>::~LinkedQueue()
{
	//Note that the cout statements here is just for learning purpose
	//They should be normally removed the destructor
	cout << "\nStarting LinkedQueue destructor...";
	cout << "\nFreeing all nodes in the queue...";

	//Free all nodes in the queue
	T temp;
	while (dequeue(temp));

	cout << "\n Is LinkedQueue Empty now?? ==> " << boolalpha << isEmpty();
	cout << "\nEnding LinkedQueue destructor..." << endl;
}

template <typename T>
int LinkedQueue<T>::GetCount() const
{
	return count;
}
#endif
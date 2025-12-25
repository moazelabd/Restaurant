#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "Node.h"
#include "order.h"
#include <iostream>
using namespace std;


inline bool isHigherPriority(Order* a, Order* b) {
   

    double priceWeight = 2.0;   
    double sizeWeight = 0.5;    
    double timeWeight = 0.1;    

    double scoreA = priceWeight * a->getOrderPrice()
        - sizeWeight * a->getOrderSize()
        - timeWeight * a->getArraivalTime();

    double scoreB = priceWeight * b->getOrderPrice()
        - sizeWeight * b->getOrderSize()
        - timeWeight * b->getArraivalTime();

    return scoreA > scoreB;   
}


template <typename T>
class PriorityQueue {
private:
    Node<T>* front;   
    int count;

public:
    PriorityQueue() : front(nullptr), count(0) {}

    ~PriorityQueue() {
        clear();
    }

    bool isEmpty() const {
        return front == nullptr;
    }

    int GetCount() const {
        return count;
    }


    bool enqueue(const T& newItem) {
        Node<T>* newNode = new Node<T>(newItem);

        if (!front) {
            front = newNode;
        }
        else {

            T item = front->getItem();
            bool insertAtFront = false;


            Order* oa = (Order*)newItem;
            Order* ob = (Order*)item;
            if (isHigherPriority(oa, ob)) {
                insertAtFront = true;
            }

            if (insertAtFront) {
                newNode->setNext(front);
                front = newNode;
            }
            else {
                Node<T>* prev = nullptr;
                Node<T>* curr = front;
                while (curr != nullptr) {
                    Order* oc = (Order*)curr->getItem();
                    if (isHigherPriority(oa, oc))
                        break;
                    prev = curr;
                    curr = curr->getNext();
                }
                newNode->setNext(curr);
                if (prev)
                    prev->setNext(newNode);
                else
                    front = newNode;
            }
        }
        count++;
        return true;
    }

    bool dequeue(T& removedItem) {
        if (isEmpty())
            return false;
        Node<T>* temp = front;
        removedItem = temp->getItem();
        front = front->getNext();
        delete temp;
        count--;
        return true;
    }

    bool peek(T& frontItem) const {
        if (isEmpty())
            return false;
        frontItem = front->getItem();
        return true;
    }

    // Move-assignment to transfer node ownership
    PriorityQueue<T>& operator=(PriorityQueue<T>& other) {
        if (this == &other) return *this;
        // clear current
        T tmp;
        while (dequeue(tmp));
        front = other.front;
        count = other.count;
        other.front = nullptr;
        other.count = 0;
        return *this;
    }
    void moveFrom(PriorityQueue<T>& other) {
        T tmp;
        while (dequeue(tmp));
        front = other.front;
        count = other.count;
        other.front = nullptr;
        other.count = 0;
    }

    void clear() {
        T x;
        while (dequeue(x));
    }

    void Print() const {
        Node<T>* cur = front;
        while (cur) {
 
            cout << cur->getItem() << " ";
            cur = cur->getNext();
        }
        cout << endl;
    }
};

#endif

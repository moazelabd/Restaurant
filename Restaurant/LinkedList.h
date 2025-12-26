#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "Node.h"
#include <iostream>
using namespace std;

template <typename T>
class LinkedList {
private:
    Node<T>* Head;
    int count;
public:
    LinkedList() : Head(nullptr), count(0) {}

    // Move-assignment: transfer ownership from other to this
    LinkedList<T>& operator=(LinkedList<T>& other) {
        if (this == &other) return *this;
        // clear current
        T tmp;
        while (DeleteFirst(tmp));
        // steal pointers
        Head = other.Head;
        count = other.count;
        other.Head = nullptr;
        other.count = 0;
        return *this;
    }
    void moveFrom(LinkedList<T>& other) {
        T tmp;
        while (DeleteFirst(tmp));
        Head = other.Head;
        count = other.count;
        other.Head = nullptr;
        other.count = 0;
    }

    ~LinkedList() {
        clear();
    }
    Node<T>* getHead() const { return Head; }

    bool isEmpty() const {
        return Head == nullptr;
    }

    void InsertBeg(const T& data) {
        Node<T>* newNode = new Node<T>(data);
        newNode->setNext(Head);
        Head = newNode;
        count++; // counter
    }

    void InsertEnd(const T& data) {
        Node<T>* newNode = new Node<T>(data);
        if (!Head) {
            Head = newNode;
            count++; // counter
            return;
        }
        Node<T>* temp = Head;
        while (temp->getNext() != nullptr) {
            temp = temp->getNext();
        }
        temp->setNext(newNode);
        count++; 
    }


    bool DeleteFirst(T& removedItem) {
        if (!Head)
            return false;
        Node<T>* temp = Head;
        removedItem = temp->getItem();
        Head = Head->getNext();
        delete temp;
        count--; // the counter
        return true;
    }

    bool DeleteLast(T& removedItem) {
        if (!Head)
            return false;
        if (Head->getNext() == nullptr) {
            removedItem = Head->getItem();
            delete Head;
            Head = nullptr;
            count--; 
            return true;
        }
        Node<T>* prev = nullptr;
        Node<T>* curr = Head;
        while (curr->getNext() != nullptr) {
            prev = curr;
            curr = curr->getNext();
        }
        removedItem = curr->getItem();
        delete curr;
        prev->setNext(nullptr);
        count--; 
        return true;
    }
    bool Find(const T& key) const {
        Node<T>* temp = Head;

        while (temp != nullptr) {
            if (temp->getItem() == key)
                return true;
            temp = temp->getNext();
        }

        return false;
    }

    bool DeleteNode(const T& value) {
        if (!Head)
            return false;
        if (Head->getItem() == value) {
            Node<T>* temp = Head;
            Head = Head->getNext();
            delete temp;
            count--; 
            return true;
        }
        Node<T>* prev = nullptr;
        Node<T>* curr = Head;
        while (curr != nullptr) {
            if (curr->getItem() == value) {
                prev->setNext(curr->getNext());
                delete curr;
                count--;
                return true;
            }
            prev = curr;
            curr = curr->getNext();
        }
        return false;
    }

    void clear() {
        T x;
        while (DeleteFirst(x));
    }

    void PrintList() const {
        Node<T>* temp = Head;
        while (temp) {
            cout << temp->getItem() << " ";
            temp = temp->getNext();
        }
        cout << endl;
    }
    int GetCount() const { return count; }


    void InsertAfter(Node<T>* prev, T item) {
        if (prev == nullptr) {
            InsertBeg(item);
            return;
        }

        Node<T>* newNode = new Node<T>(item);

        newNode->setNext(prev->getNext());
        prev->setNext(newNode);
    }


};

#endif

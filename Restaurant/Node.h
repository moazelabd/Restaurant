#ifndef NODE_H
#define NODE_H
template <typename T>
class Node {
private:
    T item;          // Data item
    Node<T>* next;   // Pointer to next node

public:
    Node() : next(nullptr) {}

    Node(const T& newItem) : item(newItem), next(nullptr) {}

    void setItem(const T& newItem) {
        item = newItem;
    }

    void setNext(Node<T>* nextNodePtr) {
        next = nextNodePtr;
    }

    T getItem() const {
        return item;
    }

    Node<T>* getNext() const {
        return next;
    }
};

#endif

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

// ============================================================================
// CS32 P01 Part 2: a singly linked list
//
// This class already works: it is correct and does not leak. But much of it
// does the same job as code in your Vector, and now in Sequence. Your job is to
// make LinkedList<T> derive from Sequence<T> and delete everything that
// Sequence now provides. Do not change how the list stores its nodes.
// ============================================================================

#include <cstddef>
#include <initializer_list>
#include <ostream>
#include <stdexcept>

template <typename T>
class LinkedList {
public:
    LinkedList();                              // empty list
    LinkedList(std::initializer_list<T> il);   // LinkedList<int> l{1, 2, 3};
    LinkedList(const LinkedList& other);       // deep copy
    LinkedList& operator=(const LinkedList& other);
    ~LinkedList();

    std::size_t size() const;
    T& at(std::size_t i);                      // throws std::out_of_range if i >= size()
    const T& at(std::size_t i) const;
    void insert(std::size_t index, const T& value); // throws std::out_of_range if index > size()
    void erase(std::size_t index);             // throws std::out_of_range if index >= size()

    bool empty() const;
    T& front();                                // throws std::out_of_range if empty
    T& back();                                 // throws std::out_of_range if empty
    void push_back(const T& value);
    void pop_back();                           // throws std::out_of_range if empty
    bool contains(const T& value) const;
    int indexOf(const T& value) const;         // -1 if not found
    void print(std::ostream& os) const;        // prints [1, 2, 3]
    bool operator==(const LinkedList& other) const;   // same size and same elements
    bool operator!=(const LinkedList& other) const;

private:
    struct Node {
        T value;
        Node* next;
    };

    Node* head_;        // first node, or nullptr if the list is empty
    std::size_t size_;  // number of nodes

    Node* node_at(std::size_t i) const;        // walks i steps from head_; i must be < size_
    void append_all(const LinkedList& other);  // appends a copy of each of other's elements
    void free_all();                           // deletes every node; the list becomes empty
};


// Templates must be visible wherever they are used, so instead of compiling
// LinkedList.cpp separately, we include it here.
#include "LinkedList.cpp"

#endif

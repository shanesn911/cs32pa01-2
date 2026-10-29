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
class LinkedList : public Sequence<T> {
public:
    LinkedList();                              // empty list
    LinkedList(std::initializer_list<T> il);   // LinkedList<int> l{1, 2, 3};
    LinkedList(const LinkedList& other);       // deep copy
    LinkedList& operator=(const LinkedList& other);
    ~LinkedList();

    std::size_t size() const override;
    T& at(std::size_t i) override;                      // throws std::out_of_range if i >= size()
    const T& at(std::size_t i) const override;
    void insert(std::size_t index, const T& value) override; // throws std::out_of_range if index > size()
    void erase(std::size_t index) override;             // throws std::out_of_range if index >= size()

   
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

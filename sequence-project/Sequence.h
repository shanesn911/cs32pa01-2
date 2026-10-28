#ifndef SEQUENCE_H
#define SEQUENCE_H

// ============================================================================
// CS32 P01 Part 2: the Sequence base class
//
// You write this file and Sequence.cpp. Sequence<T> is the base class of
// Vector<T> and LinkedList<T>: everything the two classes have in common
// belongs here, written once. This file holds the class declaration, where
// your design decisions go; Sequence.cpp holds the function bodies.
//
// Right now every function is a stub (in Sequence.cpp) that throws "not
// implemented", and nothing is virtual. Deciding what should be virtual, what
// should be pure virtual, and what should stay non-virtual is your job. Your
// design must meet these requirements (the tests check each one):
//
//   1. No one can create a plain Sequence object. Only derived classes, like
//      Vector and LinkedList, can be created.
//   2. A new kind of sequence only has to write the five primitives below.
//      Every shared operation then works on it with no extra code.
//   3. Sequence must let a future derived class replace push_back, contains,
//      and indexOf with faster versions of its own. If one does, code that
//      holds a Sequence& or Sequence* must get the derived class's version.
//   4. Deleting any sequence through a Sequence* frees all of its memory.
//   5. Each shared operation is written once, in Sequence. Vector and
//      LinkedList do not declare their own empty, front, back, pop_back,
//      contains, indexOf, print, operator==, or operator!=. (They may keep
//      their own push_back.)
//
// Do not change the names, parameters, or return types of the functions below.
// You may add a destructor.
// ============================================================================

#include <cstddef>
#include <ostream>
#include <stdexcept>

template <typename T>
class Sequence {
public:
    // ---- The five primitives ----
    std::size_t size() const;
    T& at(std::size_t i);
    const T& at(std::size_t i) const;
    void insert(std::size_t index, const T& value);
    void erase(std::size_t index);

    // ---- Shared operations ----
    bool empty() const;
    T& front();
    T& back();
    void push_back(const T& value);
    void pop_back();
    bool contains(const T& value) const;
    int indexOf(const T& value) const;
    void print(std::ostream& os) const;
};

template <typename T>
bool operator==(const Sequence<T>& a, const Sequence<T>& b);

template <typename T>
bool operator!=(const Sequence<T>& a, const Sequence<T>& b);


// Templates must be visible wherever they are used, so instead of compiling
// Sequence.cpp separately, we include it here.
#include "Sequence.cpp"

#endif

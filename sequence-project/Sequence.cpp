// ============================================================================
// CS32 P01 Part 2: Sequence implementation
//
// This file is #included at the bottom of Sequence.h rather than compiled on
// its own (templates must be visible wherever they are used). See Sequence.h
// for the requirements your design must meet.
// ============================================================================

#ifndef SEQUENCE_CPP
#define SEQUENCE_CPP

#include "Sequence.h"

// ---- The five primitives ----
//
// Each kind of sequence stores its elements differently: Vector in an array,
// LinkedList in a chain of nodes. Sequence has no data members of its own, so
// it cannot do any of these five. Only a derived class knows how.
//
// What they must do (Vector and LinkedList already do exactly this):
//   size()             the number of elements
//   at(i)              element i; throws std::out_of_range if i >= size()
//   insert(index, v)   inserts v so that afterwards at(index) == v;
//                      throws std::out_of_range if index > size()
//   erase(index)       removes element index;
//                      throws std::out_of_range if index >= size()

template <typename T>
std::size_t Sequence<T>::size() const {
    throw std::logic_error("Sequence::size() not implemented");
}

template <typename T>
T& Sequence<T>::at(std::size_t) {
    throw std::logic_error("Sequence::at() not implemented");
}

template <typename T>
const T& Sequence<T>::at(std::size_t) const {
    throw std::logic_error("Sequence::at() const not implemented");
}

template <typename T>
void Sequence<T>::insert(std::size_t, const T&) {
    throw std::logic_error("Sequence::insert() not implemented");
}

template <typename T>
void Sequence<T>::erase(std::size_t) {
    throw std::logic_error("Sequence::erase() not implemented");
}

// ---- Shared operations ----
//
// Write each of these using only the five primitives (and each other).

// Returns true if the sequence has no elements.
template <typename T>
bool Sequence<T>::empty() const {
    // TODO
    throw std::logic_error("empty() not implemented");
}

// Returns a reference to the first element.
// Throws std::out_of_range if the sequence is empty.
template <typename T>
T& Sequence<T>::front() {
    // TODO
    throw std::logic_error("front() not implemented");
}

// Returns a reference to the last element.
// Throws std::out_of_range if the sequence is empty.
template <typename T>
T& Sequence<T>::back() {
    // TODO
    throw std::logic_error("back() not implemented");
}

// Appends a copy of value to the end.
template <typename T>
void Sequence<T>::push_back(const T& value) {
    // TODO
    throw std::logic_error("push_back() not implemented");
}

// Removes the last element.
// Throws std::out_of_range if the sequence is empty.
template <typename T>
void Sequence<T>::pop_back() {
    // TODO
    throw std::logic_error("pop_back() not implemented");
}

// Returns true if some element compares equal (==) to value.
template <typename T>
bool Sequence<T>::contains(const T& value) const {
    // TODO
    throw std::logic_error("contains() not implemented");
}

// Returns the index of the first element that compares equal (==) to value,
// or -1 if there is none.
template <typename T>
int Sequence<T>::indexOf(const T& value) const {
    // TODO
    throw std::logic_error("indexOf() not implemented");
}

// Writes the elements to os in this exact format, with no newline:
//     [1, 2, 3]
// An empty sequence prints [].
//
// Hint: os is an output stream, and you write to it exactly like std::cout:
// os << "[" or os << value. The caller chooses where the output goes, e.g.
// s.print(std::cout) prints to the terminal, and the tests pass a stream that
// collects the text so they can check it. LinkedList::print in LinkedList.cpp
// shows the idea (it walks nodes, which Sequence can't, but the os << parts
// are what you need).
template <typename T>
void Sequence<T>::print(std::ostream& os) const {
    // TODO
    throw std::logic_error("print() not implemented");
}

// Returns true if a and b have the same size and every pair of elements
// compares equal with ==. a and b may be different kinds of sequence, so a
// Vector can equal a LinkedList.
template <typename T>
bool operator==(const Sequence<T>& a, const Sequence<T>& b) {
    // TODO
    throw std::logic_error("operator== not implemented");
}

// Returns true if a and b are not equal.
template <typename T>
bool operator!=(const Sequence<T>& a, const Sequence<T>& b) {
    // TODO
    throw std::logic_error("operator!= not implemented");
}

#endif

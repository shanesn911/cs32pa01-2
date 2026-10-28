// ============================================================================
// CS32 P01 Part 2: LinkedList implementation
//
// This file is #included at the bottom of LinkedList.h rather than compiled on
// its own (templates must be visible wherever they are used).
// ============================================================================

#ifndef LINKEDLIST_CPP
#define LINKEDLIST_CPP

#include "LinkedList.h"

// ---- Constructors and the Big 3 ----

template <typename T>
LinkedList<T>::LinkedList() : head_(nullptr), size_(0) {}

template <typename T>
LinkedList<T>::LinkedList(std::initializer_list<T> il) : LinkedList() {
    for (const T& value : il) {
        insert(size_, value);
    }
}

template <typename T>
LinkedList<T>::LinkedList(const LinkedList& other) : LinkedList() {
    append_all(other);
}

template <typename T>
LinkedList<T>& LinkedList<T>::operator=(const LinkedList& other) {
    if (this != &other) {
        free_all();
        append_all(other);
    }
    return *this;
}

template <typename T>
LinkedList<T>::~LinkedList() {
    free_all();
}

// ---- Private helpers ----

template <typename T>
typename LinkedList<T>::Node* LinkedList<T>::node_at(std::size_t i) const {
    Node* node = head_;
    for (std::size_t step = 0; step < i; ++step) {
        node = node->next;
    }
    return node;
}

template <typename T>
void LinkedList<T>::append_all(const LinkedList& other) {
    for (Node* node = other.head_; node != nullptr; node = node->next) {
        insert(size_, node->value);
    }
}

template <typename T>
void LinkedList<T>::free_all() {
    while (head_ != nullptr) {
        Node* next = head_->next;
        delete head_;
        head_ = next;
    }
    size_ = 0;
}

// ---- Size, access, insert, erase ----

template <typename T>
std::size_t LinkedList<T>::size() const {
    return size_;
}

template <typename T>
T& LinkedList<T>::at(std::size_t i) {
    if (i >= size_) {
        throw std::out_of_range("LinkedList::at: index out of range");
    }
    return node_at(i)->value;
}

template <typename T>
const T& LinkedList<T>::at(std::size_t i) const {
    if (i >= size_) {
        throw std::out_of_range("LinkedList::at: index out of range");
    }
    return node_at(i)->value;
}

template <typename T>
void LinkedList<T>::insert(std::size_t index, const T& value) {
    if (index > size_) {
        throw std::out_of_range("LinkedList::insert: index out of range");
    }
    if (index == 0) {
        head_ = new Node{value, head_};
    } else {
        Node* prev = node_at(index - 1);
        prev->next = new Node{value, prev->next};
    }
    ++size_;
}

template <typename T>
void LinkedList<T>::erase(std::size_t index) {
    if (index >= size_) {
        throw std::out_of_range("LinkedList::erase: index out of range");
    }
    Node* victim;
    if (index == 0) {
        victim = head_;
        head_ = head_->next;
    } else {
        Node* prev = node_at(index - 1);
        victim = prev->next;
        prev->next = victim->next;
    }
    delete victim;
    --size_;
}

// ---- Everything else ----

template <typename T>
bool LinkedList<T>::empty() const {
    return head_ == nullptr;
}

template <typename T>
T& LinkedList<T>::front() {
    if (head_ == nullptr) {
        throw std::out_of_range("LinkedList::front: list is empty");
    }
    return head_->value;
}

template <typename T>
T& LinkedList<T>::back() {
    if (head_ == nullptr) {
        throw std::out_of_range("LinkedList::back: list is empty");
    }
    Node* last = head_;
    while (last->next != nullptr) {
        last = last->next;
    }
    return last->value;
}

template <typename T>
void LinkedList<T>::push_back(const T& value) {
    Node* node = new Node{value, nullptr};
    if (head_ == nullptr) {
        head_ = node;
    } else {
        Node* last = head_;
        while (last->next != nullptr) {
            last = last->next;
        }
        last->next = node;
    }
    ++size_;
}

template <typename T>
void LinkedList<T>::pop_back() {
    if (head_ == nullptr) {
        throw std::out_of_range("LinkedList::pop_back: list is empty");
    }
    if (head_->next == nullptr) {
        delete head_;
        head_ = nullptr;
    } else {
        Node* prev = head_;   // stop at the second-to-last node
        while (prev->next->next != nullptr) {
            prev = prev->next;
        }
        delete prev->next;
        prev->next = nullptr;
    }
    --size_;
}

template <typename T>
bool LinkedList<T>::contains(const T& value) const {
    for (Node* node = head_; node != nullptr; node = node->next) {
        if (node->value == value) {
            return true;
        }
    }
    return false;
}

template <typename T>
int LinkedList<T>::indexOf(const T& value) const {
    int index = 0;
    for (Node* node = head_; node != nullptr; node = node->next) {
        if (node->value == value) {
            return index;
        }
        ++index;
    }
    return -1;
}

template <typename T>
void LinkedList<T>::print(std::ostream& os) const {
    os << "[";
    for (Node* node = head_; node != nullptr; node = node->next) {
        if (node != head_) {
            os << ", ";
        }
        os << node->value;
    }
    os << "]";
}

template <typename T>
bool LinkedList<T>::operator==(const LinkedList& other) const {
    if (size_ != other.size_) {
        return false;
    }
    // Walk both lists side by side.
    Node* mine = head_;
    Node* theirs = other.head_;
    while (mine != nullptr) {
        if (!(mine->value == theirs->value)) {
            return false;
        }
        mine = mine->next;
        theirs = theirs->next;
    }
    return true;
}

template <typename T>
bool LinkedList<T>::operator!=(const LinkedList& other) const {
    return !(*this == other);
}

#endif

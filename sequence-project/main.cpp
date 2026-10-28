// Scratch space for trying out your classes. Build and run with:
//     make main && ./main
// This file is not graded.

#include <iostream>
#include "LinkedList.h"
#include "Vector.h"

int main() {
    LinkedList<int> list{1, 2, 3};
    list.print(std::cout);
    std::cout << "\n";

    // Once Vector and LinkedList derive from Sequence, code like this works
    // with either one:
    //     Sequence<int>& s = list;
    //     s.push_back(4);
    //     s.print(std::cout);
    return 0;
}

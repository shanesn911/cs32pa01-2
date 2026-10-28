// P01 Part 2, Subpart A test suite (the Sequence hierarchy). These are the
// same tests Gradescope runs. Subpart B's tests are in experiment_tests.cpp.
//
// Run these tests:       make test-sequence
// Run one test:          python3 run_tests.py sequence 15
// List test IDs:         ./sequence_tests --list
// Run one test directly: ./sequence_tests 15
//   (with a debugger:    gdb --args ./sequence_tests 15   or   lldb -- ./sequence_tests 15)
//
// A test's ID is its position in this file, starting at 1. Tests are grouped
// by the writeup's checkpoints, in the order you do the work.
//
// Many of these tests check your class design rather than one function's
// output. They use a few small test-only classes that derive from Sequence
// (MinimalSeq and friends, below) and some compile-time checks. Each check is
// written so that it fails a test instead of breaking the build, so the
// starter code compiles and you can work through the checkpoints in order.

#include "harness.h"
#include "../Sequence.h"
#include "../Vector.h"
#include "../LinkedList.h"

#include <algorithm>
#include <sstream>
#include <string>
#include <type_traits>

// ============================================================================
// Test-only classes that derive from Sequence<int>
// ============================================================================

// MinimalSeq writes ONLY the five primitives, storing up to 64 ints in a plain
// array. Everything else it can do must come from Sequence. If the tests that
// use it pass, your shared operations really are built on the primitives.
class MinimalSeq : public Sequence<int> {
public:
    std::size_t size() const { return n_; }
    int& at(std::size_t i) {
        if (i >= n_) throw std::out_of_range("MinimalSeq::at");
        return data_[i];
    }
    const int& at(std::size_t i) const {
        if (i >= n_) throw std::out_of_range("MinimalSeq::at");
        return data_[i];
    }
    void insert(std::size_t index, const int& value) {
        if (index > n_ || n_ == CAP) throw std::out_of_range("MinimalSeq::insert");
        for (std::size_t i = n_; i > index; --i) data_[i] = data_[i - 1];
        data_[index] = value;
        ++n_;
    }
    void erase(std::size_t index) {
        if (index >= n_) throw std::out_of_range("MinimalSeq::erase");
        for (std::size_t i = index; i + 1 < n_; ++i) data_[i] = data_[i + 1];
        --n_;
    }

private:
    static constexpr std::size_t CAP = 64;
    int data_[CAP] = {};
    std::size_t n_ = 0;
};

// FastSeq is a MinimalSeq that brings its own push_back, contains, and
// indexOf, and counts how often each one runs.
class FastSeq : public MinimalSeq {
public:
    inline static int push_back_calls = 0;
    inline static int contains_calls = 0;
    inline static int indexOf_calls = 0;

    void push_back(const int& value) { ++push_back_calls; insert(size(), value); }
    bool contains(const int&) const { ++contains_calls; return true; }
    int indexOf(const int&) const { ++indexOf_calls; return 42; }
};

// HeapSeq is a MinimalSeq that also owns a heap allocation, which only its
// own destructor frees.
class HeapSeq : public MinimalSeq {
public:
    HeapSeq() : extra_(new int[100]) {}
    HeapSeq(const HeapSeq&) = delete;
    HeapSeq& operator=(const HeapSeq&) = delete;
    ~HeapSeq() { delete[] extra_; }

private:
    int* extra_;
};

// Each of these writes four of the five primitives and leaves one out. If
// that primitive is pure virtual in Sequence, the class is abstract.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
struct NoSize : Sequence<int> {
    int x = 0;
    int& at(std::size_t) { return x; }
    const int& at(std::size_t) const { return x; }
    void insert(std::size_t, const int&) {}
    void erase(std::size_t) {}
};
struct NoAt : Sequence<int> {
    int x = 0;
    std::size_t size() const { return 0; }
    const int& at(std::size_t) const { return x; }
    void insert(std::size_t, const int&) {}
    void erase(std::size_t) {}
};
struct NoConstAt : Sequence<int> {
    int x = 0;
    std::size_t size() const { return 0; }
    int& at(std::size_t) { return x; }
    void insert(std::size_t, const int&) {}
    void erase(std::size_t) {}
};
struct NoInsert : Sequence<int> {
    int x = 0;
    std::size_t size() const { return 0; }
    int& at(std::size_t) { return x; }
    const int& at(std::size_t) const { return x; }
    void erase(std::size_t) {}
};
struct NoErase : Sequence<int> {
    int x = 0;
    std::size_t size() const { return 0; }
    int& at(std::size_t) { return x; }
    const int& at(std::size_t) const { return x; }
    void insert(std::size_t, const int&) {}
};
#pragma GCC diagnostic pop

// ============================================================================
// Helpers
// ============================================================================

// Creates an S (one of the classes above) holding values, and runs body on it.
// If S is abstract, the test fails instead of the build.
template <typename S, typename F>
void with_seq(std::initializer_list<int> values, F body) {
    if constexpr (std::is_abstract_v<S>) {
        (void)values; (void)body;
        FAIL("MinimalSeq writes only the five primitives, but it is still abstract. Either "
             "Sequence has a pure virtual function besides the five primitives, or a "
             "primitive's parameters or return type differ from the starter Sequence.h.");
    } else {
        S s;
        for (int x : values) s.insert(s.size(), x);
        body(s);
    }
}

// SEQ(T, obj) gives obj (a Vector<T> or LinkedList<T>) as a Sequence<T>&. If
// obj's class does not derive from Sequence<T> yet, the test fails instead of
// the build.
template <typename T, typename D>
Sequence<T>& as_sequence(D& obj, const char* name, const char* file, int line) {
    if constexpr (std::is_convertible_v<D*, Sequence<T>*>) {
        (void)name; (void)file; (void)line;
        return obj;
    } else {
        (void)obj;
        harness::fail(file, line, std::string(name) + " cannot be used as a Sequence: its class "
                                  "does not derive (publicly) from Sequence<T> yet");
    }
}
#define SEQ(T, obj) as_sequence<T>(obj, #obj, __FILE__, __LINE__)

// Deletes obj through a Sequence<T>* (or fails the test if it can't).
template <typename T, typename D>
void delete_as_sequence(D* obj, const char* file, int line) {
    if constexpr (std::is_convertible_v<D*, Sequence<T>*>) {
        Sequence<T>* s = obj;
        delete s;
    } else {
        delete obj;
        harness::fail(file, line, "cannot delete through a Sequence*: the class does not derive "
                                  "(publicly) from Sequence<T> yet");
    }
}
#define DELETE_AS_SEQ(T, obj) delete_as_sequence<T>(obj, __FILE__, __LINE__)

// Deletes a HeapSeq through a Sequence<int>* and checks that its destructor
// freed its buffer. (A template only so that it compiles even if HeapSeq is
// abstract.)
template <typename S = HeapSeq>
void check_delete_heapseq() {
    if constexpr (std::is_abstract_v<S>) {
        FAIL("HeapSeq writes all five primitives but is still abstract");
    } else {
        alloc::Snapshot s;
        Sequence<int>* p = new S();   // HeapSeq's constructor allocates a buffer
        delete p;   // must run ~HeapSeq, which frees that buffer
        CHECK_EQ(s.live_delta(), 0);
    }
}

template <typename S>
static std::string printed(const S& s) {
    std::ostringstream os;
    s.print(os);
    return os.str();
}

// ---- Compile-time checks: does a class declare its own version of f? ----
//
// If class D does NOT declare a member named f, then &D::f names the f that D
// inherited from Sequence<T>, and it has type  R (Sequence<T>::*)(...).
// If D declares its own f (even one that overrides Sequence's), &D::f has type
// R (D::*)(...) instead, which does not convert to the Sequence version.
// inherits_f<D, T>::value is true only in the first case.
template <typename P> void take(P);

#define INHERITS_CHECK(f, ...)                                                       \
    template <typename D, typename T, typename = void>                              \
    struct inherits_##f : std::false_type {};                                       \
    template <typename D, typename T>                                               \
    struct inherits_##f<D, T, std::void_t<decltype(take<__VA_ARGS__>(&D::f))>>      \
        : std::true_type {};

INHERITS_CHECK(empty,    bool (Sequence<T>::*)() const)
INHERITS_CHECK(front,    T& (Sequence<T>::*)())
INHERITS_CHECK(back,     T& (Sequence<T>::*)())
INHERITS_CHECK(pop_back, void (Sequence<T>::*)())
INHERITS_CHECK(contains, bool (Sequence<T>::*)(const T&) const)
INHERITS_CHECK(indexOf,  int (Sequence<T>::*)(const T&) const)
INHERITS_CHECK(print,    void (Sequence<T>::*)(std::ostream&) const)

// Does class D declare its own member operator== (or !=)? Sequence's are
// non-members, so any member version found in D is a leftover duplicate.
template <typename P> struct member_class {};
template <typename R, typename C, typename... A> struct member_class<R (C::*)(A...)> { using type = C; };
template <typename R, typename C, typename... A> struct member_class<R (C::*)(A...) const> { using type = C; };

#define MEMBER_OP_CHECK(name, op)                                                    \
    template <typename D, typename = void>                                          \
    struct name : std::false_type {};                                               \
    template <typename D>                                                           \
    struct name<D, std::void_t<typename member_class<decltype(&D::operator op)>::type>> \
        : std::is_same<typename member_class<decltype(&D::operator op)>::type, D> {};

MEMBER_OP_CHECK(has_member_eq, ==)
MEMBER_OP_CHECK(has_member_ne, !=)

// Lists the shared operations that class D (e.g. Vector<int>) declares for
// itself instead of inheriting them from Sequence<T>.
template <typename D, typename T>
std::string redeclared_shared_operations() {
    std::string found;
    if (!inherits_empty<D, T>::value) found += " empty()";
    if (!inherits_front<D, T>::value) found += " front()";
    if (!inherits_back<D, T>::value) found += " back()";
    if (!inherits_pop_back<D, T>::value) found += " pop_back()";
    if (!inherits_contains<D, T>::value) found += " contains()";
    if (!inherits_indexOf<D, T>::value) found += " indexOf()";
    if (!inherits_print<D, T>::value) found += " print()";
    if (has_member_eq<D>::value) found += " operator==";
    if (has_member_ne<D>::value) found += " operator!=";
    return found;
}

// ---- Compile-time check: is there exactly one operator== (and !=)? ----
//
// &::operator==<T> only compiles if exactly one function template named
// operator== exists at global scope. If Vector or LinkedList still has its
// own, the name is ambiguous.
#define ONLY_ONE_CHECK(name, op)                                                     \
    template <typename T, typename = void>                                          \
    struct name : std::false_type {};                                               \
    template <typename T>                                                           \
    struct name<T, std::void_t<decltype(&::operator op<T>)>>                       \
        : std::is_same<decltype(&::operator op<T>),                                \
                       bool (*)(const Sequence<T>&, const Sequence<T>&)> {};

ONLY_ONE_CHECK(only_sequence_eq, ==)
ONLY_ONE_CHECK(only_sequence_ne, !=)

// ============================================================================
// Subpart A, Checkpoint A1: the Sequence base class (tested with MinimalSeq and friends)
// ============================================================================

TEST(seq_abstract, "[Sequence] Sequence is abstract (cannot be created on its own)", 2) {
    CHECK(std::is_abstract_v<Sequence<int>>);
}


TEST(seq_primitives, "[Sequence] Exactly the five primitives are pure virtual", 4) {
    std::string not_pure;
    if (!std::is_abstract_v<NoSize>) not_pure += " size()";
    if (!std::is_abstract_v<NoAt>) not_pure += " at()";
    if (!std::is_abstract_v<NoConstAt>) not_pure += " at() const";
    if (!std::is_abstract_v<NoInsert>) not_pure += " insert()";
    if (!std::is_abstract_v<NoErase>) not_pure += " erase()";
    if (!not_pure.empty())
        FAIL("a class that leaves out one of these primitives can still be created, so it is "
             "not pure virtual in Sequence:" + not_pure);
    if (std::is_abstract_v<MinimalSeq>)
        FAIL("MinimalSeq writes all five primitives but is still abstract: some other Sequence "
             "function is pure virtual, or a primitive's signature differs from the starter");
}


TEST(seq_empty_front_back, "[Sequence] empty, front, back work on a class that writes only the primitives", 3) {
    with_seq<MinimalSeq>({}, [](Sequence<int>& s) {
        CHECK(s.empty());
    });
    with_seq<MinimalSeq>({4, 5, 6}, [](Sequence<int>& s) {
        CHECK(!s.empty());
        CHECK_EQ(s.front(), 4);
        CHECK_EQ(s.back(), 6);
        s.front() = 40;   // front() and back() return references
        s.back() = 60;
        CHECK_EQ(s.at(0), 40);
        CHECK_EQ(s.at(2), 60);
    });
}


TEST(seq_throws, "[Sequence] front, back, pop_back throw std::out_of_range when empty", 3) {
    with_seq<MinimalSeq>({}, [](Sequence<int>& s) {
        CHECK_THROWS(s.front(), std::out_of_range);
        CHECK_THROWS(s.back(), std::out_of_range);
        CHECK_THROWS(s.pop_back(), std::out_of_range);
        CHECK_EQ(s.size(), 0);
    });
}


TEST(seq_push_pop, "[Sequence] push_back and pop_back", 3) {
    with_seq<MinimalSeq>({}, [](Sequence<int>& s) {
        s.push_back(1);
        s.push_back(2);
        s.push_back(3);
        CHECK_EQ(s.size(), 3);
        CHECK_EQ(s.at(0), 1);
        CHECK_EQ(s.at(2), 3);
        s.pop_back();
        CHECK_EQ(s.size(), 2);
        CHECK_EQ(s.back(), 2);
        s.pop_back();
        s.pop_back();
        CHECK(s.empty());
    });
}


TEST(seq_search, "[Sequence] contains and indexOf", 3) {
    with_seq<MinimalSeq>({7, 8, 9, 8}, [](Sequence<int>& s) {
        CHECK(s.contains(7));
        CHECK(s.contains(8));
        CHECK(s.contains(9));
        CHECK(!s.contains(10));
        CHECK_EQ(s.indexOf(7), 0);
        CHECK_EQ(s.indexOf(8), 1);   // the FIRST match
        CHECK_EQ(s.indexOf(9), 2);
        CHECK_EQ(s.indexOf(10), -1);
    });
    with_seq<MinimalSeq>({}, [](Sequence<int>& s) {
        CHECK(!s.contains(0));
        CHECK_EQ(s.indexOf(0), -1);
    });
}


TEST(seq_print, "[Sequence] print writes [a, b, c]", 3) {
    with_seq<MinimalSeq>({}, [](Sequence<int>& s) {
        CHECK_EQ(printed(s), std::string("[]"));
    });
    with_seq<MinimalSeq>({5}, [](Sequence<int>& s) {
        CHECK_EQ(printed(s), std::string("[5]"));
    });
    with_seq<MinimalSeq>({1, 2, 3}, [](Sequence<int>& s) {
        CHECK_EQ(printed(s), std::string("[1, 2, 3]"));
    });
}


TEST(seq_equality, "[Sequence] == and != compare size and elements", 3) {
    with_seq<MinimalSeq>({1, 2, 3}, [](Sequence<int>& a) {
        with_seq<MinimalSeq>({1, 2, 3}, [&](Sequence<int>& b) {
            CHECK(a == b);
            CHECK(!(a != b));
        });
        with_seq<MinimalSeq>({1, 2, 4}, [&](Sequence<int>& b) {
            CHECK(!(a == b));
            CHECK(a != b);
        });
        with_seq<MinimalSeq>({1, 2}, [&](Sequence<int>& b) {   // a prefix is not equal
            CHECK(!(a == b));
            CHECK(!(b == a));
            CHECK(a != b);
        });
        CHECK(a == a);
    });
    with_seq<MinimalSeq>({}, [](Sequence<int>& a) {
        with_seq<MinimalSeq>({}, [&](Sequence<int>& b) {
            CHECK(a == b);
        });
    });
}


TEST(seq_override, "[Sequence] A derived class's push_back, contains, indexOf are used through a Sequence&", 4) {
    with_seq<FastSeq>({}, [](Sequence<int>& s) {
        s.push_back(1);
        CHECK_EQ(FastSeq::push_back_calls, 1);
        CHECK_EQ(s.size(), 1);
        CHECK(s.contains(99));               // FastSeq::contains always says true
        CHECK_EQ(FastSeq::contains_calls, 1);
        CHECK_EQ(s.indexOf(99), 42);         // FastSeq::indexOf always says 42
        CHECK_EQ(FastSeq::indexOf_calls, 1);
    });
}


TEST(seq_virtual_dtor, "[Sequence] Deleting a derived object through a Sequence* runs its destructor", 2) {
    CHECK(std::has_virtual_destructor_v<Sequence<int>>);
    check_delete_heapseq();
}

// ============================================================================
// Subpart A, Checkpoint A2: LinkedList derives from Sequence
// ============================================================================

TEST(list_derives, "[LinkedList] LinkedList<T> derives from Sequence<T>", 2) {
    CHECK((std::is_base_of_v<Sequence<int>, LinkedList<int>>));
    CHECK((std::is_convertible_v<LinkedList<int>*, Sequence<int>*>));   // public inheritance
}


TEST(list_no_dupes, "[LinkedList] LinkedList inherits the shared operations instead of declaring its own", 4) {
    if (!std::is_base_of_v<Sequence<int>, LinkedList<int>>)
        FAIL("LinkedList<T> does not derive from Sequence<T> yet");
    std::string found = redeclared_shared_operations<LinkedList<int>, int>();
    std::string msg = "LinkedList still declares its own version of:" + found +
                      ". Delete it from LinkedList.h and LinkedList.cpp so that Sequence's is used.";
    if (!found.empty()) FAIL(msg);
}


TEST(list_shared, "[LinkedList] insert, erase, at, and the shared operations work on a LinkedList", 6) {
    if (!std::is_convertible_v<LinkedList<int>*, Sequence<int>*>)
        FAIL("LinkedList<T> does not derive (publicly) from Sequence<T> yet");

    // The primitives still work after the refactor.
    LinkedList<int> p;
    CHECK_EQ(p.size(), 0);
    p.insert(0, 2);        // [2]
    p.insert(0, 1);        // [1, 2]
    p.insert(2, 4);        // [1, 2, 4]
    p.insert(2, 3);        // [1, 2, 3, 4]
    CHECK_EQ(p.size(), 4);
    for (int i = 0; i < 4; ++i) CHECK_EQ(p.at(i), i + 1);
    p.erase(0);            // [2, 3, 4]
    p.erase(1);            // [2, 4]
    CHECK_EQ(p.size(), 2);
    CHECK_EQ(p.at(0), 2);
    CHECK_EQ(p.at(1), 4);
    CHECK_THROWS(p.at(2), std::out_of_range);
    CHECK_THROWS(p.insert(3, 0), std::out_of_range);
    CHECK_THROWS(p.erase(2), std::out_of_range);
    const LinkedList<int>& c = p;
    CHECK_EQ(c.at(1), 4);

    // The shared operations, now inherited from Sequence.
    LinkedList<int> l{1, 2, 3, 2};
    Sequence<int>& s = SEQ(int, l);
    CHECK(!s.empty());
    CHECK_EQ(s.front(), 1);
    CHECK_EQ(s.back(), 2);
    CHECK(s.contains(3));
    CHECK(!s.contains(4));
    CHECK_EQ(s.indexOf(2), 1);
    CHECK_EQ(s.indexOf(4), -1);
    s.pop_back();
    s.push_back(5);
    CHECK_EQ(printed(s), std::string("[1, 2, 3, 5]"));
    CHECK_EQ(l.size(), 4);

    LinkedList<int> empty;
    Sequence<int>& e = SEQ(int, empty);
    CHECK(e.empty());
    CHECK_THROWS(e.front(), std::out_of_range);
    CHECK_THROWS(e.pop_back(), std::out_of_range);
    CHECK_EQ(printed(e), std::string("[]"));

    LinkedList<std::string> words{"ab", "cd"};
    CHECK_EQ(printed(SEQ(std::string, words)), std::string("[ab, cd]"));
}


TEST(list_memory, "[LinkedList] Copies are deep, nothing leaks, and deleting through a Sequence* frees every node", 6) {
    if (!std::is_convertible_v<LinkedList<int>*, Sequence<int>*>)
        FAIL("LinkedList<T> does not derive (publicly) from Sequence<T> yet");

    // The Big 3 still work after the refactor.
    alloc::Snapshot s;
    {
        LinkedList<int> a{1, 2, 3};
        LinkedList<int> b(a);
        b.at(0) = 100;
        CHECK_EQ(a.at(0), 1);
        LinkedList<int> c{9};
        c = a;
        c.at(2) = 300;
        CHECK_EQ(a.at(2), 3);
        CHECK_EQ(c.size(), 3);
        LinkedList<int>& same = c;
        c = same;   // self-assignment
        CHECK_EQ(c.size(), 3);
        CHECK_EQ(c.at(0), 1);
    }
    CHECK_EQ(s.live_delta(), 0);

    // Deleting through a Sequence* runs ~LinkedList.
    LinkedList<Tracker>* l = new LinkedList<Tracker>{1, 2, 3};
    l->insert(1, Tracker(7));
    DELETE_AS_SEQ(Tracker, l);
    CHECK_EQ(Tracker::alive, 0);
    CHECK_EQ(s.live_delta(), 0);
}

// ============================================================================
// Subpart A, Checkpoint A3: Vector derives from Sequence
// ============================================================================

TEST(vec_derives, "[Vector] Vector<T> derives from Sequence<T>", 2) {
    CHECK((std::is_base_of_v<Sequence<int>, Vector<int>>));
    CHECK((std::is_convertible_v<Vector<int>*, Sequence<int>*>));   // public inheritance
}


TEST(vec_no_dupes, "[Vector] Vector inherits the shared operations instead of declaring its own", 4) {
    if (!std::is_base_of_v<Sequence<int>, Vector<int>>)
        FAIL("Vector<T> does not derive from Sequence<T> yet");
    std::string found = redeclared_shared_operations<Vector<int>, int>();
    std::string msg = "Vector still declares its own version of:" + found +
                      ". Delete it from Vector.h and Vector.cpp so that Sequence's is used.";
    if (!found.empty()) FAIL(msg);
}


TEST(vec_sizeof, "[Vector] sizeof(Vector<int>) is 3 pointers plus a vtable pointer", 2) {
    // Part 1: three pointers (24 bytes). A class with virtual functions also
    // stores a hidden pointer to its vtable, so now it should be 32 bytes.
    CHECK_EQ(sizeof(Vector<int>), 4 * sizeof(int*));
}


TEST(vec_growth, "[Vector] push_back and insert through a Sequence& still grow by growth_factor", 3) {
    Vector<int>::growth_factor = 2.0;
    Vector<int> v;
    Sequence<int>& s = SEQ(int, v);
    const std::size_t caps[] = {1, 2, 4, 4, 8, 8, 8, 8};
    for (int i = 0; i < 8; ++i) {
        s.push_back(i);
        CHECK_EQ(v.capacity(), caps[i]);
    }
    s.insert(3, 99);   // full at 8: grows to 16
    CHECK_EQ(v.capacity(), 16);
    CHECK_EQ(v[3], 99);
    CHECK_EQ(v[4], 3);
    s.erase(3);
    CHECK_EQ(v[3], 3);
    CHECK_EQ(v.size(), 8);
    CHECK_EQ(v.capacity(), 16);

    Vector<int>::growth_factor = 1.5;
    Vector<int> w;
    Sequence<int>& t = SEQ(int, w);
    const std::size_t caps15[] = {1, 2, 3, 4, 6, 6, 9};
    for (int i = 0; i < 7; ++i) {
        t.push_back(i);
        CHECK_EQ(w.capacity(), caps15[i]);
    }
    Vector<int>::growth_factor = 2.0;
}


TEST(vec_shared, "[Vector] Shared operations work on a Vector", 3) {
    Vector<int> v{1, 2, 3, 2};
    Sequence<int>& s = SEQ(int, v);
    const int* buffer = v.data();
    CHECK(!s.empty());
    CHECK_EQ(s.front(), 1);
    CHECK_EQ(s.back(), 2);
    CHECK(s.contains(3));
    CHECK(!s.contains(4));
    CHECK_EQ(s.indexOf(2), 1);
    CHECK_EQ(s.indexOf(4), -1);
    s.pop_back();
    CHECK_EQ(v.size(), 3);
    CHECK_EQ(v.capacity(), 4);   // pop_back never shrinks the buffer
    CHECK(v.data() == buffer);
    CHECK_EQ(printed(s), std::string("[1, 2, 3]"));

    const Sequence<int>& c = s;
    CHECK_EQ(c.at(2), 3);        // Vector's new const at()
    CHECK_THROWS(c.at(3), std::out_of_range);

    Vector<int> empty;
    Sequence<int>& e = SEQ(int, empty);
    CHECK(e.empty());
    CHECK_THROWS(e.front(), std::out_of_range);
    CHECK_THROWS(e.back(), std::out_of_range);
    CHECK_THROWS(e.pop_back(), std::out_of_range);
    CHECK_EQ(printed(e), std::string("[]"));

    Vector<std::string> words{"ab", "cd"};
    CHECK_EQ(printed(SEQ(std::string, words)), std::string("[ab, cd]"));
}


TEST(vec_delete_base, "[Vector] Deleting a Vector through a Sequence* frees its buffer", 2) {
    alloc::Snapshot snap;
    Vector<Tracker>* v = new Vector<Tracker>();
    Sequence<Tracker>& s = SEQ(Tracker, *v);
    for (int i = 0; i < 5; ++i) s.push_back(Tracker(i));
    DELETE_AS_SEQ(Tracker, v);
    CHECK_EQ(Tracker::alive, 0);
    CHECK_EQ(snap.live_delta(), 0);
}


TEST(eq_once, "[Equality] operator== and operator!= are written once, for Sequence", 2) {
    if (!only_sequence_eq<int>::value)
        FAIL("there is more than one operator== template (or Sequence's is missing). Delete "
             "the ones for Vector and LinkedList; Sequence's handles both.");
    if (!only_sequence_ne<int>::value)
        FAIL("there is more than one operator!= template (or Sequence's is missing). Delete "
             "the ones for Vector and LinkedList; Sequence's handles both.");
}


TEST(eq_mixed, "[Equality] A Vector and a LinkedList with the same elements are equal", 2) {
    Vector<int> v{1, 2, 3};
    LinkedList<int> l{1, 2, 3};
    Sequence<int>& sv = SEQ(int, v);
    Sequence<int>& sl = SEQ(int, l);
    CHECK(sv == sl);
    CHECK(sl == sv);
    CHECK(!(sv != sl));
    sl.push_back(4);
    CHECK(!(sv == sl));
    CHECK(sv != sl);
    sv.push_back(5);
    CHECK(sv != sl);
    Vector<int> v2{1, 2, 3, 5};
    CHECK(sv == SEQ(int, v2));   // Vector == Vector still works
}

int main(int argc, char** argv) { return harness_main(argc, argv); }

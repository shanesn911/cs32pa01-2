// P01 Part 2, Subpart B test suite (the growth-factor experiment). These are
// the same tests Gradescope runs. Subpart A's tests are in sequence_tests.cpp.
//
// Run these tests:       make test-experiment
// Run one test:          python3 run_tests.py experiment 3
// List test IDs:         ./experiment_tests --list
// Run one test directly: ./experiment_tests 3
//   (with a debugger:    gdb --args ./experiment_tests 3   or   lldb -- ./experiment_tests 3)
//
// A test's ID is its position in this file, starting at 1.

#include "harness.h"
#include "../Vector.h"
#include "../growth_experiment.h"

#include <algorithm>

// What measure_growth should return: simulates the growth formula from
// Vector.h. (Your measure_growth must watch a real Vector instead.)
static GrowthStats expected_growth(double factor, std::size_t n) {
    GrowthStats s;
    std::size_t size = 0, cap = 0;
    for (std::size_t i = 0; i < n; ++i) {
        if (size == cap) {
            cap = std::max(cap + 1, static_cast<std::size_t>(cap * factor));
            ++s.reallocations;
            s.elements_copied += size;
        }
        ++size;
    }
    s.final_capacity = cap;
    return s;
}

#define CHECK_GROWTH(factor, n)                                                      \
    do {                                                                            \
        GrowthStats _got = measure_growth(factor, n);                               \
        GrowthStats _want = expected_growth(factor, n);                             \
        CHECK_EQ(_got.reallocations, _want.reallocations);                          \
        CHECK_EQ(_got.elements_copied, _want.elements_copied);                      \
        CHECK_EQ(_got.final_capacity, _want.final_capacity);                        \
    } while (0)

// ============================================================================
// Subpart B: the growth-factor experiment
// ============================================================================

TEST(growth_tiny, "[Experiment] measure_growth with n = 0, 1, 2, 3", 1) {
    CHECK_GROWTH(2.0, 0);   // nothing pushed: 0 reallocations, capacity 0
    CHECK_GROWTH(2.0, 1);   // one reallocation (0 -> 1), nothing to copy
    CHECK_GROWTH(2.0, 2);
    CHECK_GROWTH(2.0, 3);
}


TEST(growth_double, "[Experiment] measure_growth with factor 2.0", 3) {
    CHECK_GROWTH(2.0, 8);
    CHECK_GROWTH(2.0, 9);
    CHECK_GROWTH(2.0, 1000);
}


TEST(growth_other, "[Experiment] measure_growth with factors 1.5 and 3.0", 4) {
    CHECK_GROWTH(1.5, 10);
    CHECK_GROWTH(1.5, 1000);
    CHECK_GROWTH(3.0, 37);
    CHECK_GROWTH(3.0, 1000);
}


TEST(growth_one, "[Experiment] measure_growth with factor 1.0 (grows by one each time)", 2) {
    CHECK_GROWTH(1.0, 5);
    CHECK_GROWTH(1.0, 300);
}


TEST(growth_four, "[Experiment] measure_growth with factor 4.0", 2) {
    CHECK_GROWTH(4.0, 5);
    CHECK_GROWTH(4.0, 1000);
}


TEST(growth_clean, "[Experiment] measure_growth uses a real Vector, frees it, and restores growth_factor", 2) {
    Vector<int>::growth_factor = 1.7;
    alloc::Snapshot s;
    GrowthStats got = measure_growth(3.0, 100);
    CHECK_EQ(Vector<int>::growth_factor, 1.7);
    // A real Vector<int> allocates a new buffer for each reallocation.
    CHECK(s.allocs() >= static_cast<long long>(got.reallocations));
    CHECK_EQ(s.live_delta(), 0);
}

int main(int argc, char** argv) { return harness_main(argc, argv); }

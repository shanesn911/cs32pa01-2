// Prints a table of your measure_growth results for several growth factors.
// Build and run with:
//     make experiment && ./experiment
// This file is not graded, but you need its output for the written questions.

#include <chrono>
#include <cstdio>
#include <exception>
#include "growth_experiment.h"

int main() {
    const std::size_t N = 100000;
    const double factors[] = {1.0, 1.1, 1.5, 2.0, 3.0, 4.0};

    std::printf("push_back %zu ints into an empty Vector<int>\n\n", N);
    std::printf("%6s  %13s  %15s  %15s  %14s  %10s  %9s\n", "factor", "reallocations",
                "elements_copied", "copies/element", "final_capacity", "unused", "time (ms)");
    for (double factor : factors) {
        try {
            auto start = std::chrono::steady_clock::now();
            GrowthStats s = measure_growth(factor, N);
            auto stop = std::chrono::steady_clock::now();
            double ms = std::chrono::duration<double, std::milli>(stop - start).count();
            double unused = s.final_capacity ? 100.0 * (s.final_capacity - N) / s.final_capacity : 0;
            std::printf("%6.1f  %13zu  %15zu  %15.2f  %14zu  %9.1f%%  %9.1f\n", factor,
                        s.reallocations, s.elements_copied, double(s.elements_copied) / N,
                        s.final_capacity, unused, ms);
        } catch (const std::exception& e) {
            std::printf("%6.1f  error: %s\n", factor, e.what());
        }
    }
    return 0;
}

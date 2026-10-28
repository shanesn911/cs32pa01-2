# P01 Part 2: Growth Factors and a Sequence Hierarchy

Two subparts, in this order:

- **Subpart A, Hierarchy:** write the `Sequence<T>` base class in `Sequence.h` and `Sequence.cpp`, make `LinkedList<T>`
  and your `Vector<T>` derive from it, and delete the code that Sequence now provides.
- **Subpart B, Experiment:** implement `measure_growth` in `growth_experiment.cpp`, then run
  `make experiment && ./experiment` to see how the growth factor changes the cost of `push_back`.

**First, copy your `Vector.cpp` from Part 1 into this folder.** Nothing builds without it.

## Files

| File | What it is |
|---|---|
| `Sequence.h` | The base class declaration. Nothing is virtual yet. **Your design goes here.** |
| `Sequence.cpp` | Stub bodies for every Sequence function. **You implement them.** |
| `Vector.h`, `Vector.cpp` | Your Vector from Part 1 (`Vector.h` is now yours to edit). |
| `LinkedList.h`, `LinkedList.cpp` | A working singly linked list. You refactor it. |
| `growth_experiment.h` | The `GrowthStats` struct and `measure_growth` declaration. Do not edit. |
| `growth_experiment.cpp` | **You implement `measure_growth` here.** |
| `experiment_main.cpp` | Prints a table of your `measure_growth` results. Not graded. |
| `main.cpp` | Scratch space. Not graded. |
| `tests/sequence_tests.cpp` | Subpart A tests (same as Gradescope). |
| `tests/experiment_tests.cpp` | Subpart B tests (same as Gradescope). |
| `tests/harness.h` | Test framework: counts heap allocations, defines `Tracker`. |
| `run_tests.py` | Builds and runs each test in its own process. |

You submit seven files: `Sequence.h`, `Sequence.cpp`, `Vector.h`, `Vector.cpp`, `LinkedList.h`, `LinkedList.cpp`,
`growth_experiment.cpp`.

## Building and testing

```bash
make test-sequence                   # Subpart A tests
make test-experiment                 # Subpart B tests
make test-all                        # all tests, exactly as Gradescope grades them
python3 run_tests.py sequence 15     # run only Subpart A's test 15 (or: experiment 3)
make warnings                        # show compiler warnings in your files (there must be none)
./sequence_tests --list              # list Subpart A's test IDs, points, and names
./sequence_tests 15                  # run test 15 directly (Subpart B: ./experiment_tests)
gdb --args ./sequence_tests 15       # debug test 15 (on a Mac: lldb -- ./sequence_tests 15)
make experiment && ./experiment      # print the growth-factor table
make main && ./main                  # run your scratch program
```

Subpart A and Subpart B are separate test programs, so unfinished Subpart B code never gets in
the way of Subpart A's tests. As in Part 1, the build uses AddressSanitizer when it is available, and falls back to valgrind
otherwise. If you move between machines, run `make clean` first.

## Rules

- Same as Part 1: allocate with `new`/`new[]`, free with `delete`/`delete[]`, and do not use
  `std::vector`, `std::deque`, `std::list`, `std::array`, or smart pointers in any file you submit.
- Do not change the names, parameters, or return types of the functions in the starter `Sequence.h`.
- Do not change how `LinkedList` stores its nodes.

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -g -O0

# AddressSanitizer + UndefinedBehaviorSanitizer catch use-after-free, double
# free, and out-of-bounds accesses the moment they happen. Some machines have
# g++ but not the sanitizer libraries, so we only turn them on if a tiny test
# program links with them. Without them, the tests run under valgrind instead.
# To turn them off yourself:  make clean && make SANITIZE=
SANITIZER_FLAGS = -fsanitize=address,undefined -fno-omit-frame-pointer
SANITIZE ?= $(shell tmp=$$(mktemp) && \
    echo 'int main() { return 0; }' | $(CXX) -x c++ $(SANITIZER_FLAGS) - -o $$tmp >/dev/null 2>&1 && \
    echo '$(SANITIZER_FLAGS)'; rm -f $$tmp)

HEADERS = Sequence.h Sequence.cpp Vector.h Vector.cpp LinkedList.h LinkedList.cpp

all: sequence_tests experiment_tests experiment main

# Subpart A: the Sequence hierarchy
sequence_tests: tests/sequence_tests.cpp tests/harness.h $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SANITIZE) -o $@ tests/sequence_tests.cpp

# Subpart B: the growth-factor experiment
experiment_tests: tests/experiment_tests.cpp tests/harness.h growth_experiment.cpp growth_experiment.h $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SANITIZE) -o $@ tests/experiment_tests.cpp growth_experiment.cpp

# Built with optimizations and no sanitizers, so the timings mean something.
experiment: experiment_main.cpp growth_experiment.cpp growth_experiment.h $(HEADERS)
	$(CXX) -std=c++17 -O2 -o $@ experiment_main.cpp growth_experiment.cpp

main: main.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SANITIZE) -o $@ main.cpp

test-sequence:
	python3 run_tests.py sequence

test-experiment:
	python3 run_tests.py experiment

# Everything, exactly as Gradescope grades it.
test-all:
	python3 run_tests.py all

test:
	@echo "Use one of:"
	@echo "  make test-sequence     Subpart A tests (the Sequence hierarchy)"
	@echo "  make test-experiment   Subpart B tests (the growth-factor experiment)"
	@echo "  make test-all          all tests, the same as Gradescope"

# Used by run_tests.py to decide whether to fall back to valgrind.
print-sanitize:
	@echo '$(SANITIZE)'

# Check your code for compiler warnings without building anything. They always
# re-check, even if nothing changed. The tests run these as graded checks.
warnings-sequence:
	$(CXX) $(CXXFLAGS) -fsyntax-only tests/sequence_tests.cpp

warnings-experiment:
	$(CXX) $(CXXFLAGS) -fsyntax-only growth_experiment.cpp

warnings: warnings-sequence warnings-experiment

clean:
	rm -rf sequence_tests experiment_tests experiment main *.dSYM

.PHONY: all test test-sequence test-experiment test-all clean print-sanitize \
        warnings warnings-sequence warnings-experiment

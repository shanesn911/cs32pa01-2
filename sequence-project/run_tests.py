#!/usr/bin/env python3
"""
Runs the P01 Part 2 tests. This is the same script Gradescope runs.

    python3 run_tests.py sequence           Subpart A tests  (make test-sequence)
    python3 run_tests.py experiment         Subpart B tests  (make test-experiment)
    python3 run_tests.py all                all tests, as Gradescope grades them (make test-all)
    python3 run_tests.py sequence 15        run only Subpart A's test 15
    python3 run_tests.py sequence -v        also show output of passing tests

Every test has an ID (shown in the output). To run one test by itself, for
example in a debugger:  ./sequence_tests 15   or   gdb --args ./sequence_tests 15
(Subpart B's tests are in ./experiment_tests.)

Each test runs in its own process, so a crash (e.g. a double free caught by
AddressSanitizer) only fails that one test. Subpart A and Subpart B are built
as separate programs, so a compile error in one does not affect the other.

If this machine's g++ cannot use AddressSanitizer (the Makefile checks), each
test runs under valgrind instead, so memory errors still fail the test.
"""
import shutil
import argparse
import json
import os
import re
import subprocess
import sys

HIERARCHY_FILES = ['Sequence.h', 'Sequence.cpp', 'Vector.h', 'Vector.cpp', 'LinkedList.h', 'LinkedList.cpp']

SUITES = {
    'sequence': {
        'label': 'A',
        'title': 'Subpart A: the Sequence hierarchy',
        'binary': './sequence_tests',
        'source': 'tests/sequence_tests.cpp',
        'files': HIERARCHY_FILES,                  # must exist and are banned-checked
        'warnings_target': 'warnings-sequence',
        'warnings_files': HIERARCHY_FILES,
        'warnings_name': '[Build] Your Subpart A files compile without warnings',
        'warnings_points': 2,
    },
    'experiment': {
        'label': 'B',
        'title': 'Subpart B: the growth-factor experiment',
        'binary': './experiment_tests',
        'source': 'tests/experiment_tests.cpp',
        'files': HIERARCHY_FILES + ['growth_experiment.cpp'],
        'warnings_target': 'warnings-experiment',
        'warnings_files': ['growth_experiment.cpp'],
        'warnings_name': '[Build] growth_experiment.cpp compiles without warnings',
        'warnings_points': 1,
    },
}

TIMEOUT_SECONDS = 10
VALGRIND_TIMEOUT_SECONDS = 60
VALGRIND_ERROR_EXIT = 99
# nouserintercepts: let the test harness keep its own operator new/delete
# (it counts allocations); valgrind still checks every access to that memory.
VALGRIND = ['valgrind', '-q', f'--error-exitcode={VALGRIND_ERROR_EXIT}', '--leak-check=no',
            '--soname-synonyms=somalloc=nouserintercepts']
MAX_OUTPUT_LINES = 40

BANNED = [
    (r'#\s*include\s*[<"]\s*(vector|deque|list|forward_list|array|memory|bits/stdc\+\+\.h)\s*[>"]',
     'including a standard container or <memory> header'),
    (r'\bstd\s*::\s*(vector|deque|list|forward_list|array|unique_ptr|shared_ptr)\b',
     'using a standard container or smart pointer'),
]

SAN_ENV = {
    'ASAN_OPTIONS': 'detect_leaks=0:allocator_may_return_null=0',
    'UBSAN_OPTIONS': 'print_stacktrace=1:halt_on_error=1',
}

LINK_ERROR_HINT = (
    'Your code compiled, but linking failed: a function is declared but never defined.\n'
    'The lines below name each missing function. If a line mentions a vtable (on macOS:\n'
    '"referenced from: vtable for X"; on Linux, the line contains _ZTV, the linker\'s name for a\n'
    'vtable), class X still declares a virtual function whose body you deleted (or never wrote).\n'
    'Delete that declaration from the header too, or give it a body.')


def strip_comments(src):
    src = re.sub(r'/\*.*?\*/', '', src, flags=re.S)
    return re.sub(r'//[^\n]*', '', src)


def check_files(names):
    """Returns an error message if a file is missing or uses a banned container."""
    missing = [name for name in names if not os.path.isfile(name)]
    if missing:
        return ('Missing ' + ', '.join(missing) + '. ' +
                ('Copy your Vector.cpp from Part 1 into this folder. ' if 'Vector.cpp' in missing else '') +
                'These files are needed: ' + ', '.join(names) + '.')
    for name in names:
        with open(name) as f:
            src = strip_comments(f.read())
        for pattern, why in BANNED:
            m = re.search(pattern, src)
            if m:
                return (f'{name} is not allowed to use the standard library containers '
                        f'or smart pointers (found: "{m.group(0)}", {why}).')
    return None


def build(suite):
    """Returns None on success, or an error message."""
    target = suite['binary'][2:]
    proc = subprocess.run(['make', target], capture_output=True, text=True)
    if proc.returncode == 0:
        return None
    output = proc.stdout + proc.stderr
    if re.search(r'Undefined symbols|undefined reference to', output):
        return LINK_ERROR_HINT + '\n\n' + output
    return 'Compilation failed:\n\n' + output


def suite_points(suite):
    """Total points of a suite, read from its test source (used when it doesn't build)."""
    with open(suite['source']) as f:
        points = sum(int(p) for p in re.findall(r'^TEST\(\w+,\s*".*",\s*(\d+)\)', f.read(), re.M))
    return points + suite['warnings_points']


def check_warnings(suite):
    """Graded check: the suite's student files must compile without warnings (-Wall -Wextra)."""
    proc = subprocess.run(['make', '-s', suite['warnings_target']], capture_output=True, text=True)
    pattern = re.compile(r'(?:^|/)((?:' + '|'.join(re.escape(n) for n in suite['warnings_files']) +
                         r'):\d+:\d+: warning:.*)')
    found = []
    for line in (proc.stdout + proc.stderr).splitlines():
        m = pattern.search(line)
        if m and m.group(1) not in found:   # templates repeat each warning
            found.append(m.group(1))
    if not found:
        return True, 'No warnings.'
    return False, ('Your code compiles, but with warnings. Fix each one below; '
                   f'run `make {suite["warnings_target"]}` to see them in full.\n\n' + '\n'.join(found))


def checker():
    """Returns 'asan', 'valgrind', or None (no memory checking available)."""
    flags = subprocess.run(['make', '-s', 'print-sanitize'], capture_output=True, text=True).stdout
    if 'address' in flags:
        return 'asan'
    if shutil.which('valgrind'):
        return 'valgrind'
    return None


def list_tests(binary):
    out = subprocess.run([binary, '--list'], capture_output=True, text=True, check=True).stdout
    tests = []
    for line in out.splitlines()[1:]:   # first line is the header
        test_id, points, name = line.split(None, 2)
        tests.append((int(test_id), name, int(points)))
    return tests


def trim(output):
    lines = output.strip().splitlines()
    if len(lines) > MAX_OUTPUT_LINES:
        lines = lines[:MAX_OUTPUT_LINES] + [f'... ({len(lines) - MAX_OUTPUT_LINES} more lines)']
    return '\n'.join(lines)


def run_test(binary, test_id, mode):
    env = dict(os.environ, **SAN_ENV)
    cmd = [binary, str(test_id)]
    timeout = TIMEOUT_SECONDS
    if mode == 'valgrind':
        cmd = VALGRIND + cmd
        timeout = VALGRIND_TIMEOUT_SECONDS
    try:
        proc = subprocess.run(cmd, capture_output=True, text=True, timeout=timeout, env=env)
    except subprocess.TimeoutExpired:
        return False, f'Timed out after {timeout} seconds (infinite loop?)'
    output = proc.stdout + proc.stderr
    passed = proc.returncode == 0 and proc.stdout.strip().endswith('PASS')
    if not passed and proc.returncode < 0:
        output += f'\nProcess killed by signal {-proc.returncode} (crash).'
    if mode == 'valgrind' and proc.returncode == VALGRIND_ERROR_EXIT:
        # The test's own checks may have passed; the memory error is what failed it.
        stdout = '' if proc.stdout.strip() == 'PASS' else proc.stdout
        output = ('valgrind detected a memory error. Read the first lines below: they say '
                  'what went wrong and which line of your code did it.\n\n' + proc.stderr + stdout)
    elif 'AddressSanitizer' in output:
        output = ('AddressSanitizer detected a memory error. Read the first lines below: '
                  'they say what went wrong and which line of your code did it.\n\n' + output)
    return passed, trim(output)


def run_suite(suite, mode, test_id, verbose, show_label):
    """Builds and runs one suite. Returns a list of Gradescope test results, or
    None if the requested test does not exist."""
    label = suite['label']
    error = build(suite)
    if error:
        print(error)
        return [{
            'name': f'{label}. {suite["title"]}: did not build',
            'score': 0,
            'max_score': suite_points(suite),
            'status': 'failed',
            'output': trim(error),
            'visibility': 'visible',
        }]

    tests = list_tests(suite['binary'])
    warnings_id = len(tests) + 1   # the warnings check comes after the C++ tests
    tests.append((warnings_id, suite['warnings_name'], suite['warnings_points']))
    if test_id is not None:
        tests = [t for t in tests if t[0] == test_id]
        if not tests:
            print(f'No test with ID {test_id}. IDs are 1 to {warnings_id}; '
                  f'run {suite["binary"]} --list to see tests 1 to {warnings_id - 1}.')
            return None

    results = []
    for tid, name, points in tests:
        if tid == warnings_id:
            passed, output = check_warnings(suite)
        else:
            passed, output = run_test(suite['binary'], tid, mode)
        earned = points if passed else 0
        shown_id = f'{label}{tid}' if show_label else str(tid)
        print(f'{"PASS" if passed else "FAIL"} [{earned}/{points}] #{shown_id} {name}')
        if not passed or verbose:
            for line in output.splitlines():
                print('      ' + line)
        if not passed and tid == warnings_id:
            print(f'      (check again: make {suite["warnings_target"]})')
        elif not passed:
            print(f'      (run just this test: {suite["binary"]} {tid})')
        results.append({
            'name': f'{label}{tid}. {name}',
            'score': earned,
            'max_score': points,
            'status': 'passed' if passed else 'failed',
            'output': 'Passed' if passed else output,
            'visibility': 'visible',
        })
    return results


def write_json(path, score, max_score, output, tests):
    result = {'score': score, 'output': output, 'tests': tests}
    if max_score is not None:
        result['max_score'] = max_score
    with open(path, 'w') as f:
        json.dump(result, f, indent=2)


def summary(results):
    passed = sum(r['status'] == 'passed' for r in results)
    score = sum(r['score'] for r in results)
    max_score = sum(r['max_score'] for r in results)
    return score, max_score, f'Passed {passed}/{len(results)} tests. Score: {score}/{max_score}'


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('suite', choices=['sequence', 'experiment', 'all'],
                        help='sequence = Subpart A, experiment = Subpart B, all = both (as Gradescope)')
    parser.add_argument('test_id', nargs='?', type=int,
                        help='run only this test (IDs are listed by ./sequence_tests --list '
                             'or ./experiment_tests --list)')
    parser.add_argument('-v', '--verbose', action='store_true')
    parser.add_argument('--json', metavar='PATH', help='write Gradescope results.json')
    args = parser.parse_args()
    if args.suite == 'all' and args.test_id is not None:
        parser.error('to run one test, name its subpart: run_tests.py sequence ID or run_tests.py experiment ID')

    names = ['sequence', 'experiment'] if args.suite == 'all' else [args.suite]
    files = []
    for name in names:
        files += [f for f in SUITES[name]['files'] if f not in files]
    error = check_files(files)
    if error:
        print(error)
        if args.json:
            write_json(args.json, 0, None, error, [])
        sys.exit(1)

    mode = checker()
    if mode == 'valgrind':
        print('Note: AddressSanitizer is not available on this machine, so each test runs under\n'
              'valgrind instead. Memory errors are still caught, but tests run slower.\n')
    elif mode is None:
        print('WARNING: neither AddressSanitizer nor valgrind is available on this machine.\n'
              'Memory errors (use-after-free, double free, ...) may go unnoticed here but\n'
              'will fail on Gradescope. Test on CSIL or another Linux machine.\n')

    results = []
    for name in names:
        suite = SUITES[name]
        if len(names) > 1:
            print(f'===== {suite["title"]} =====')
        suite_results = run_suite(suite, mode, args.test_id, args.verbose, show_label=len(names) > 1)
        if suite_results is None:
            sys.exit(1)
        if len(names) > 1:
            print(summary(suite_results)[2] + '\n')
        results += suite_results

    score, max_score, text = summary(results)
    if len(names) > 1:
        text = 'Total: ' + text
    print(('' if len(names) > 1 else '\n') + text)
    if args.json:
        write_json(args.json, score, max_score, text, results)


if __name__ == '__main__':
    main()

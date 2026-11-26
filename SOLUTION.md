# Scope simulator

The implementation follows the specification at
https://github.com/ACMClassCourse-2025/Scope-2025.

Each name maps to a stack of active declarations. The top is the visible
binding. Each scope records the bindings it introduced so that leaving a scope
restores shadowed declarations and removes names that are no longer visible.
All validation occurs before mutation, so invalid operations leave state intact.
Strings are read from the remainder of the command line, preserving whitespace
inside their quotes. Addition evaluates both operands before assigning its result.

Hash-table lookup is expected constant time. Exiting a scope costs time linear
in its declarations, and each declaration is removed at most once. Parsing and
string operations additionally cost time proportional to the characters handled.
Memory is proportional to active declarations, their values, and the input line.

Build with `cmake . && make`. Run focused tests with
`python3 tests/regression.py ./code`.

## Validation

- All ten upstream public input/output pairs matched byte for byte.
- The largest upstream case completed in approximately 0.22 seconds locally.
- Focused regressions cover invalid operations without mutation, shadowing,
  inaccessible variables, integer boundaries, string whitespace, empty strings,
  and aliased addition.
- The sample-data path in the task README was absent; public samples were
  retrieved from the linked specification repository instead.

## Online judge result

- Submission: `879630` (first attempt, ACMOJ problem `2902`).
- Evaluated commit: `2114488b691c2250a67583e5c190f44da8e49c86`.
- Result: Accepted, 100/100; all 20 test points passed.
- Maximum individual test time: 363 ms; peak memory: 7,528,448 bytes.

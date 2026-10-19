# Package Delivery Tracking System

A C++17 terminal application that accepts packages, processes them in arrival order,
looks up tracking numbers, advances delivery status, and produces reports and summaries.
The project demonstrates a sequential collection, a FIFO queue, a hash table, linear
search, and a manually implemented insertion sort.

## Build and run

Requirements: a C++17 compiler (Clang, GCC, or MSVC). There are no third-party C++ dependencies.
Run these commands from the repository root on macOS or Linux:

```bash
mkdir -p build
c++ -std=c++17 -Wall -Wextra -Wpedantic src/main.cpp src/tracking.cpp -o build/package-tracker
./build/package-tracker
```

With MinGW on Windows, run the same compiler command with `g++` and output
`build/package-tracker.exe`. With MSVC, use a Developer Command Prompt:

```bat
mkdir build
cl /std:c++17 /EHsc /W4 src\main.cpp src\tracking.cpp /Fe:build\package-tracker.exe
build\package-tracker.exe
```

The Clang build was tested on macOS arm64. Windows instructions are provided but were
not executed in the recorded test environment. Both `.cpp` source files must be compiled.

## Menu and features

```text
PACKAGE DELIVERY TRACKING SYSTEM
--------------------------------
1. Add Package
2. Process Next Package
3. Find Package by Tracking Number
4. Update Package Status
5. Display Waiting Packages
6. Display All Packages
7. Display Sorted Package Report
8. Display System Summary
9. Exit
```

- Add a unique tracking number, recipient, destination, and positive finite weight in kg.
- Process the oldest waiting package first and change its status to `Processed`.
- Search by tracking number using `std::unordered_map`.
- Update only `Processed -> In Transit` or `In Transit -> Delivered` through option 4.
- Display waiting packages without removing them, and display all records in arrival order.
- Sort a copy by tracking number or weight, both ascending, using hand-written insertion sort.
- Show total and per-status counts, total/average weight, and the heaviest package.
- Safely handle empty data, invalid input, duplicate numbers, missing records, and end-of-input.
- Provide a separate linear search tested against hash lookup.

## Data structures and their purposes

| Structure | Contents | Purpose |
| --- | --- | --- |
| `Package` | Tracking number, recipient, destination, weight, status | One complete record |
| `std::vector<Package>` | All records in arrival order | Authoritative storage; traversal and statistics |
| `std::queue<std::size_t>` | Indices of waiting records | FIFO processing without copying mutable records |
| `std::unordered_map<std::string, std::size_t>` | Tracking number to vector index | Expected average O(1) exact lookup; worst O(N) |

The vector is append-only. Reports sort copies. Tracking numbers cannot be edited.
Thus an existing vector index keeps referring to the same package, including after vector
reallocation. Status exists in one authoritative record. See [design](docs/design.md)
for consistency rules and [analysis](docs/analysis.md) for complexity assumptions.

## Example session

Add these three packages in the listed order:

| Tracking | Recipient | Destination | Weight (kg) |
| --- | --- | --- | ---: |
| PK003 | Alice | San Diego | 2 |
| PK001 | Bob | Los Angeles | 5 |
| PK002 | Carol | Seattle | 3 |

Option 5 shows `PK003, PK001, PK002`. Option 7 then 1 shows `PK001, PK002, PK003`;
option 7 then 2 shows `PK003, PK002, PK001`. Processing still starts with `PK003`.
The total weight is 10.00 kg, the average is 3.33 kg, and PK001 is the heaviest.

## Testing

The application does not require Python. The automated menu tests require Python 3;
the convenience runner also requires Bash.

```bash
bash tests/run_tests.sh
```

The runner compiles with C++17 and warnings treated as errors, executes the C++ tests,
and sends scripted input to the real terminal program. A failed check causes a nonzero exit.
There are 47 core checks and 20 CLI checks. Some test-case IDs have multiple assertions.
For Clang/GCC environments supporting AddressSanitizer and UndefinedBehaviorSanitizer:

```bash
SANITIZE=1 bash tests/run_tests.sh
```

Run the C++ tests alone if Python or Bash is unavailable:

```bash
c++ -std=c++17 -Wall -Wextra -Wpedantic tests/test_tracking.cpp src/tracking.cpp -o build/test-tracking
./build/test-tracking
```

See [test plan and actual results](docs/test-plan.md) and the captured
[test output](docs/test-results.txt).

## Repository contents

```text
package-delivery-tracking/
├── README.md
├── .gitignore
├── src/
│   ├── main.cpp                 # Terminal menu, validation and display
│   ├── tracking.h               # Package, Summary and PackageSystem declarations
│   └── tracking.cpp             # Data operations, searching, sorting and summary
├── docs/
│   ├── design.md
│   ├── pseudocode.md
│   ├── analysis.md
│   ├── test-plan.md
│   ├── test-results.txt
│   ├── video-guide.md
│   └── submission-checklist.md
└── tests/
    ├── test_tracking.cpp
    ├── test_cli.py
    └── run_tests.sh
```

## Limitations and behavior choices

- Records live in memory only and disappear when the program exits. No database, files,
  networking, GUI, deletion, or tracking-number editing are implemented.
- Tracking numbers are case-sensitive strings. Leading/trailing ASCII whitespace is trimmed;
  internal spaces are retained. Tracking-number sorting is lexicographic, so `PK10` precedes `PK2`.
- Weight uses `double`, calculations accumulate in `long double`, and display rounds to two
  decimal places. All positive finite input weights are accepted; very small positive values
  may display as 0.00. This is a classroom numeric model, not arbitrary-precision accounting.
- Equal weights retain arrival order in sorted reports. The first arrival wins a heaviest tie.
- Insertion sort is suitable for demonstrating the course algorithm on small datasets; its
  average and worst-case O(N^2) growth makes it unsuitable for very large reports.
- The program is single-threaded and assumes available system memory. It does not recover
  interactively from resource exhaustion. No custom queue or custom hash table is required.

## References

Resources actually consulted for the requirements and algorithm explanations:

1. **Package Delivery Tracking System**, instructor-provided assignment PDF, dated October 18,
   2026: required features, status rules, deliverables, testing, and video requirements.
2. Dr. Khan, [Queues](https://d-khan.github.io/cisc-courses/data-structures/lectures/queues/):
   FIFO semantics and queue operations.
3. Supplied local course study notes, **Insertion Sort and Algorithm Efficiency** (`sorting-ii.md`):
   sorted-prefix invariant, shifts, stability, and average/worst-case analysis.
4. Supplied local course study notes, **Linear and Binary Search** (`searching.md`):
   sequential search and the distinction between comparisons and running time.
5. Supplied local course study notes, **Hash Tables and Linear Probing** (`hash-tables.md`):
   hash keys, collisions, and lookup behavior. The application uses `std::unordered_map`,
   not the notes' custom linear-probing implementation.

The local study notes are identified by title and original filename; they are not included
as runtime dependencies. No measured timing advantage is claimed.

## Submission

Submit the private GitHub repository link and the required 3-5 minute Zoom video in Canvas.
Grant the instructor collaborator access. The [video guide](docs/video-guide.md) and
[submission checklist](docs/submission-checklist.md) explain the remaining personal steps.
Understand the implementation and follow the course's rules on assistance and attribution.

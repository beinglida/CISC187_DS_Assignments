# Adaptive Sorting Strategy

## Part A — Adaptive Sorting Selection

### Strategy and measurable thresholds

The program stores exactly 50 integers in a `std::array<int, 50>`. Before changing the array, it examines the 49 adjacent pairs and counts **descents**, where `A[i] > A[i + 1]`. Equal neighbors are treated as ordered. Let `d` be the descent count and `r = d / 49` be the descent ratio.

| Practical category | Ratio rule | Exact rule for 50 integers | Selected algorithm |
|---|---|---|---|
| Best/Nearly Sorted | r <= 0.10 | 0 <= d <= 4 | Insertion Sort |
| Average/Partially Ordered | 0.10 < r < 0.90 | 5 <= d <= 44 | Selection Sort |
| Worst/Highly Reverse-Ordered | r >= 0.90 | 45 <= d <= 49 | Selection Sort |

The rules are exhaustive and do not overlap. The code uses integer cross-multiplication to avoid floating-point boundary issues. For a general input of N >= 2, the same ratio uses N - 1 adjacent pairs. Inputs of length zero or one can be classified as already sorted without pair comparisons.

These are practical categories, not formal theoretical best-, average-, and worst-case definitions. In particular, a low adjacent-descent count is only a heuristic for low insertion-sort cost.

### Complete C++ implementation for Parts A and B

The program has two entry modes sharing exactly the same classification function. The user chooses whether to sort or only classify, but never chooses the sorting algorithm. In `sort` mode, the classification automatically determines the sorting algorithm. In `classify` mode, the program returns before reaching either sorting function.

```cpp
#include <array>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>

constexpr int SIZE = 50;
using Data = std::array<int, SIZE>;
enum class Category { Nearly, Partial, Reverse };
struct Analysis { int descents; Category category; };
struct Stats { long long comparisons = 0, shifts = 0, swaps = 0; };

Analysis classify(const Data& a) {
    int d = 0;
    for (int i = 0; i < SIZE - 1; ++i)
        if (a[i] > a[i + 1]) ++d;
    // d / 49 <= 10%, or d / 49 >= 90%; integer arithmetic.
    Category c = Category::Partial;
    if (10 * d <= SIZE - 1) c = Category::Nearly;
    else if (10 * d >= 9 * (SIZE - 1)) c = Category::Reverse;
    return {d, c};
}
const char* label(Category c) {
    if (c == Category::Nearly) return "Best/Nearly Sorted";
    if (c == Category::Reverse) return "Worst/Highly Reverse-Ordered";
    return "Average/Partially Ordered";
}
Stats insertionSort(Data& a) {
    Stats s;
    for (int i = 1; i < SIZE; ++i) {
        int key = a[i], j = i - 1;
        while (j >= 0) {
            ++s.comparisons;  // Count true and false element comparisons.
            if (a[j] <= key) break;
            a[j + 1] = a[j];
            ++s.shifts;
            --j;
        }
        a[j + 1] = key;
    }
    return s;
}
Stats selectionSort(Data& a) {
    Stats s;
    for (int i = 0; i < SIZE - 1; ++i) {
        int minimum = i;
        for (int j = i + 1; j < SIZE; ++j) {
            ++s.comparisons;
            if (a[j] < a[minimum]) minimum = j;
        }
        if (minimum != i) {
            std::swap(a[i], a[minimum]);
            ++s.swaps;
        }
    }
    return s;
}
void print(const Data& a) {
    for (int i = 0; i < SIZE; ++i) {
        if (i) std::cout << ' ';
        std::cout << a[i];
    }
    std::cout << '\n';
}
bool readData(Data& a) {
    std::cout << "Enter exactly 50 integers on one line:\n";
    std::string line;
    if (!std::getline(std::cin, line)) return false;
    std::istringstream input(line);
    for (int& value : a) if (!(input >> value)) return false;
    std::string extra;
    return !(input >> extra);
}
int main(int argc, char* argv[]) {
    if (argc != 2 || (std::string(argv[1]) != "sort" &&
                      std::string(argv[1]) != "classify")) {
        std::cerr << "Usage: adaptive_sorting sort|classify\n";
        return 1;
    }
    Data a{};
    if (!readData(a)) {
        std::cerr << "Error: enter exactly 50 valid integers on one line.\n";
        return 1;
    }
    const Analysis result = classify(a);
    std::cout << "Input classification: " << label(result.category) << '\n';
    std::cout << "Adjacent descents: " << result.descents << " / 49\n";
    if (std::string(argv[1]) == "classify") {
        std::cout << "Original array (unchanged): ";
        print(a);
        return 0;  // No sorting function is called in Part B.
    }
    std::cout << "Array before sorting: "; print(a);
    const bool useInsertion = result.category == Category::Nearly;
    std::cout << "Selected algorithm: "
              << (useInsertion ? "Insertion Sort" : "Selection Sort") << '\n';
    const Stats stats = useInsertion ? insertionSort(a) : selectionSort(a);
    std::cout << "Array after sorting: "; print(a);
    std::cout << "Sort comparisons: " << stats.comparisons
              << "; shifts: " << stats.shifts << "; swaps: " << stats.swaps << '\n';
}
```

### Compile and run

Save the C++ code block as `adaptive_sorting.cpp` in your working directory. With a C++17 compiler available, run:

```sh
c++ -std=c++17 -Wall -Wextra -Wpedantic adaptive_sorting.cpp -o adaptive_sorting
./adaptive_sorting sort
```

Enter exactly 50 whitespace-separated integers on one line. The program rejects too few values, extra tokens, non-integer tokens, and values outside the supported `int` range. Mode selection is through the command-line argument, not an extra integer in the array.

Example input:

```text
1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50
```

Observed output for this input:

```text
Enter exactly 50 integers on one line:
Input classification: Best/Nearly Sorted
Adjacent descents: 0 / 49
Array before sorting: 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50
Selected algorithm: Insertion Sort
Array after sorting: 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50
Sort comparisons: 49; shifts: 0; swaps: 0
```

The optional counters measure element comparisons, shifts, and swaps during sorting only. Both successful and unsuccessful element comparisons count here. Loop checks and final key placements are not counted, and these different operation types are reported separately rather than treated as equivalent units of runtime.

## Part B — Case Classification Without Sorting

Run the same program in its classification-only mode:

```sh
./adaptive_sorting classify
```

Enter 50 integers on one line. The program reports their category, descent count, and original array. It does not call Selection Sort or Insertion Sort. The classifier takes a `const Data&`, reads adjacent values, and does not modify them. The output of the original array also allows its order to be checked directly.

Part B uses the exact same `classify` function as Part A, so there is no duplicated threshold logic that could become inconsistent. The user is not asked to pre-sort the input.

## Part C — Complexity of the Classification

For N elements, the algorithm compares exactly N - 1 adjacent pairs. Each iteration makes one element comparison and may increment a counter. Category selection then uses only a constant number of arithmetic operations and comparisons.

Thus the work has the form `a(N - 1) + b` for fixed constants a and b, making classification **O(N)** time and **O(1)** auxiliary space. For this assignment's 50-element array, it makes exactly 49 pair comparisons. Although 50 is fixed in the implementation, asymptotic analysis considers how the strategy would scale with N.

Input reading and displaying an array also take O(N) time under the fixed-width integer model. Pre-analysis does not change the overall worst-case bound:

```text
O(N) classification + O(N²) sorting = O(N²)
```

For already sorted input, the classifier selects Insertion Sort, whose work is O(N), so classification plus sorting is still O(N). For inputs with only O(N) inversions, insertion sorting also takes O(N) time. However, the practical nearly-sorted label does not guarantee this inversion bound, so that category is not guaranteed to sort in linear time.

## Part D — Documentation and Analysis

### Threshold justification

A 10% lower cutoff requires at least 90% of adjacent relationships to be nondecreasing, allowing a small number of local breaks rather than requiring perfect order. A 90% upper cutoff identifies data whose neighboring pairs overwhelmingly descend. The broad middle interval covers inputs with mixed local order. These cutoffs are simple, reproducible design choices rather than benchmark-proven optimal settings.

For uniformly random distinct elements, each adjacent pair has probability one half of descending, giving an expected 24.5 descents for 50 elements. This helps explain why typical random data tends toward the middle category. Duplicate-heavy inputs can behave differently because equal neighbors do not count as descents.

### Why each algorithm is selected

- **Best/Nearly Sorted: Insertion Sort.** It can exploit existing order and avoid unnecessary shifts. Fully sorted input needs only 49 element comparisons and zero shifts in this implementation.
- **Average/Partially Ordered: Selection Sort.** This is a conservative policy favoring a bounded number of swaps when local ordering provides no strong evidence of near-sortedness. Selection Sort does at most 49 swaps, although it still makes 1,225 element comparisons. This choice is not a claim that Selection Sort is always faster on partially ordered integer arrays.
- **Worst/Highly Reverse-Ordered: Selection Sort.** Reverse order leads to many insertion-sort shifts. Selection Sort limits swaps to at most N - 1, trading fixed quadratic comparison work for fewer data movements. A swap involves multiple assignments, so swaps and insertion shifts are not directly interchangeable operation units.

This program implements the requested adaptive policy, not a universal optimal sorter. Timing measurements and workload characteristics would be needed to tune the policy for real applications.

### Input order and sorting complexity

**Selection Sort:** On every input, it scans all remaining candidates to select each minimum. Its element comparisons total `(N - 1) + (N - 2) + ... + 1 = N(N - 1)/2`, so its best-, average-, and worst-case time are O(N²). Skipping unnecessary swaps does not eliminate these comparisons.

**Insertion Sort:** In already sorted data, each key requires just one stopping comparison and no shifts, giving O(N) time. More generally, its work is Θ(N + I), where I is the number of inversions, or out-of-order index pairs. Each shift removes one inversion. If I = O(N), its time remains O(N). For uniformly random distinct elements, the expected inversion count is N(N - 1)/4; for strictly descending elements, it is N(N - 1)/2. These yield O(N²) average and worst cases.

**Practical differences:** Equal worst-case Big-O bounds do not mean equal comparison counts, data movements, or elapsed time on a particular input. Selection Sort favors few swaps; Insertion Sort can exploit existing order and is stable because it does not move equal elements past each other. The ordinary swapping Selection Sort used here is not generally stable.

### Limitation of adjacent-pair classification

The array `[26, 27, ..., 50, 1, 2, ..., 25]` has only one adjacent descent, but every element in its first half exceeds every element in its second half. It therefore has 25 × 25 = 625 inversions. The classifier labels it Best/Nearly Sorted and chooses Insertion Sort, which actually performs 625 shifts.

For larger even N, this construction has only one adjacent descent but N²/4 inversions. Consequently, a small descent ratio does not prove small global disorder or linear sorting time. This limitation affects performance prediction, not correctness: both implemented sorts still produce a sorted array.

## Testing and Observed Results

The delivered C++ code was compiled with `clang++ -std=c++17 -Wall -Wextra -Wpedantic`; compilation succeeded with no diagnostic output. Tests checked the full output array against an independently sorted expected array and checked the classification against its threshold rule.

| Test input | Descents | Category | Selected algorithm | Sort comparisons | Shifts | Swaps |
|---|---:|---|---|---:|---:|---:|
| Ascending | 0 | Best/Nearly Sorted | Insertion Sort | 49 | 0 | 0 |
| One adjacent swap | 1 | Best/Nearly Sorted | Insertion Sort | 49 | 1 | 0 |
| Descending | 49 | Worst/Highly Reverse-Ordered | Selection Sort | 1225 | 0 | 25 |
| All equal | 0 | Best/Nearly Sorted | Insertion Sort | 49 | 0 | 0 |
| Rotated halves | 1 | Best/Nearly Sorted | Insertion Sort | 673 | 625 | 0 |
| Boundary d=4 | 4 | Best/Nearly Sorted | Insertion Sort | 55 | 10 | 0 |
| Boundary d=5 | 5 | Average/Partially Ordered | Selection Sort | 1225 | 0 | 3 |
| Boundary d=44 | 44 | Average/Partially Ordered | Selection Sort | 1225 | 0 | 22 |
| Boundary d=45 | 45 | Worst/Highly Reverse-Ordered | Selection Sort | 1225 | 0 | 23 |

### Reproducing the test inputs

- Ascending: integers 1 through 50.
- One adjacent swap: `[2, 1, 3, 4, ..., 50]`.
- Descending: integers 50 through 1.
- All equal: fifty copies of 7.
- Rotated halves: integers 26 through 50 followed by 1 through 25.
- Boundary d=k: the first k+1 integers in descending order, followed by the remaining integers in ascending order. This creates exactly k descents and tests both sides of each threshold.

All 9 listed inputs and 100 additional random arrays were tested in both modes, for **218 successful valid-input program runs**. The random arrays used Python's `random.Random(42)` and 50 calls to `randint(-100, 100)` per array, covering negative values and duplicates. Every sorting run matched the expected ascending sequence, including multiplicities. Every classification-only run reproduced the original sequence exactly and printed no sorting statistics or algorithm selection.

Four additional invalid-input tests checked 49 integers, 51 integers, 49 integers followed by `abc`, and 49 integers followed by an out-of-range integer. All four were rejected with exit status 1 and the documented error message. Valid runs returned exit status 0. These tests provide evidence of the implementation's behavior; they do not establish that the threshold policy is fastest for all inputs.

## Analysis and Reflection

### What can be determined without sorting?

A read-only scan can determine whether adjacent pairs are ordered, count local descents, and identify breaks between nondecreasing runs. Other scans could determine minimum, maximum, or equality of all elements. Local descent count alone does not reveal the total inversion count or precisely predict sorting cost, as the rotated-halves test demonstrates.

### What does examining the input cost?

It adds 49 pair comparisons for this assignment, or O(N) work in general. This is small relative to quadratic work on large inputs but is not free. For the sorted test, classification adds 49 comparisons before the 49 sorting comparisons; directly running Insertion Sort would avoid that first scan.

### Is adaptation always preferable?

No. It adds analysis overhead and can select an algorithm using an imperfect predictor. The rotated-halves test was classified as nearly sorted despite requiring 625 shifts. When the workload is already known to be nearly sorted, selecting Insertion Sort in advance may be simpler and avoid the classification pass.

### How do dataset size and ordering affect selection?

At only 50 integers, constant factors and implementation overhead matter. Already sorted inputs strongly favor Insertion Sort's linear behavior, while reverse order causes many shifts. For substantially larger disordered inputs, both quadratic algorithms may be poor choices compared with an appropriate O(N log N) sorting algorithm; that extension is outside this lab's two-algorithm design.

### Why is Big-O alone insufficient?

The tests show zero shifts for sorted input but 625 shifts for a different input with only one descent. Selection Sort always makes 1,225 element comparisons for 50 elements, while its swap count varies. Big-O does not show these exact costs, stability, or hardware-dependent timing. This strategy therefore uses measured input characteristics while acknowledging that operation counts and empirical timing provide information beyond asymptotic bounds.

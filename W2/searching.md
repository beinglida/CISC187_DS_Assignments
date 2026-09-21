# Linear and Binary Search

## Comparison-counting convention

In the paper-and-pencil questions, one comparison means examining one array element against the target and determining its relationship to the target. This is one **probe**, or conceptual three-way comparison.

The C++ program also reports the actual number of element-to-target Boolean comparisons. For Binary Search, `data[mid] == target` and `data[mid] < target` are separate evaluations. A failed equality test is followed by an ordering test. Reporting both probes and Boolean comparisons avoids confusing the textbook 17-step bound with the number of C++ relational evaluations. Loop-control checks and index bookkeeping are not element-to-target comparisons.

## 1. Linear Search

Given `[2, 4, 6, 8, 10, 12, 13]`, finding `8` using Linear Search takes **4 comparisons**:

| Comparison | Element | Result |
|---|---:|---|
| 1 | 2 | Not equal to 8 |
| 2 | 4 | Not equal to 8 |
| 3 | 6 | Not equal to 8 |
| 4 | 8 | Match; stop |

Linear Search begins at the first element and examines one element at a time, so it reaches 8 in the fourth position.

## 2. Binary Search

Finding `8` in the same array takes **1 comparison**. With zero-based indices, `low = 0`, `high = 6`, and `mid = 0 + (6 - 0)/2 = 3`. The middle element is already `A[3] = 8`, so the first probe succeeds.

## 3. Binary Search on a Large Dataset

The maximum is **17 probes (conceptual element comparisons)** for a sorted array of 100,000 elements using standard midpoint Binary Search.

Each unsuccessful probe eliminates the midpoint and about half of the remaining candidates. For N >= 1, the maximum number of probes is:

```text
floor(log2(N)) + 1
= ceil(log2(N + 1))
```

Since `2^16 = 65,536 < 100,000 < 131,072 = 2^17`, the answer is 17. The logarithmic count follows from repeatedly halving the search space rather than examining every element.

In the equality-then-ordering C++ implementation below, a successful search using 17 probes performs 33 Boolean element-to-target comparisons. An unsuccessful 17-probe search performs 34. These are implementation-level counts, not a contradiction of the 17-probe textbook answer.

## 4. Linear Search vs. Binary Search

### Dataset and implementation

The program creates a vector containing the distinct sorted integers `1` through `100000`. It accepts an integer target and searches for exactly that target with all three algorithms. The data is already sorted by construction; no unreported sorting step is needed for Binary Search.

The following single, complete C++ program implements Question 4 and Question 5 Part C. It uses only the three headers permitted for randomized search and does not use `set`, `unordered_set`, or `map`.

```cpp
#include <vector>
#include <random>
#include <iostream>

struct Result {
    int index = -1;
    int probes = 0;       // Number of array elements examined.
    int comparisons = 0; // Actual element-to-target == and < evaluations.
};
Result linearSearch(const std::vector<int>& data, int target) {
    Result r;
    for (int i = 0; i < static_cast<int>(data.size()); ++i) {
        ++r.probes;
        ++r.comparisons;
        if (data[i] == target) { r.index = i; return r; }
    }
    return r;
}
Result binarySearch(const std::vector<int>& data, int target) {
    Result r;
    int low = 0, high = static_cast<int>(data.size()) - 1;
    while (low <= high) {
        const int mid = low + (high - low) / 2;
        ++r.probes;
        ++r.comparisons;
        if (data[mid] == target) { r.index = mid; return r; }
        ++r.comparisons;
        if (data[mid] < target) low = mid + 1;
        else high = mid - 1;
    }
    return r;
}
Result randomizedSearch(const std::vector<int>& data, int target,
                        std::mt19937& generator) {
    const int n = static_cast<int>(data.size());
    // This O(N) initialization is included in whole-function complexity.
    std::vector<int> remaining(n);
    for (int i = 0; i < n; ++i) remaining[i] = i;
    Result r;
    for (int count = n; count > 0; --count) {
        std::uniform_int_distribution<int> pick(0, count - 1);
        const int slot = pick(generator);
        const int index = remaining[slot];
        // Remove this index from the active prefix in constant time.
        remaining[slot] = remaining[count - 1];
        ++r.probes;
        ++r.comparisons;
        if (data[index] == target) { r.index = index; return r; }
    }
    return r;
}
void show(const char* algorithm, const Result& r) {
    std::cout << algorithm << ": found=" << (r.index >= 0 ? "yes" : "no")
              << ", index=" << r.index << ", probes=" << r.probes
              << ", comparisons=" << r.comparisons << '\n';
}
int main() {
    constexpr int N = 100000;
    std::vector<int> data(N);
    for (int i = 0; i < N; ++i) data[i] = i + 1;
    int target;
    std::cout << "Enter an integer target:\n";
    if (!(std::cin >> target)) {
        std::cerr << "Error: expected an integer target.\n";
        return 1;
    }
    std::mt19937 generator(42); // Fixed seed for repeatable local tests.
    show("Linear", linearSearch(data, target));
    show("Binary", binarySearch(data, target));
    show("Randomized", randomizedSearch(data, target, generator));
}
```

Save the code as `searching.cpp`, compile with a C++17 compiler, and run it:

```sh
c++ -std=c++17 -Wall -Wextra -Wpedantic searching.cpp -o searching
./searching
```

Enter a target such as `2`, `99999`, or `100001`. Run the program again for another target. Indices in the output are zero-based; `-1` means not found.

### Actual test observations

The code was compiled and executed with a fixed random-engine seed of 42. The following counts were observed locally:

| Target | Algorithm | Found? | Index | Element probes | Boolean comparisons |
|---|---|---|---:|---:|---:|
| 2 | Linear | yes | 1 | 2 | 2 |
| 2 | Binary | yes | 1 | 17 | 33 |
| 2 | Randomized | yes | 1 | 36940 | 36940 |
| 50000 | Linear | yes | 49999 | 50000 | 50000 |
| 50000 | Binary | yes | 49999 | 1 | 1 |
| 50000 | Randomized | yes | 49999 | 8265 | 8265 |
| 99999 | Linear | yes | 99998 | 99999 | 99999 |
| 99999 | Binary | yes | 99998 | 16 | 31 |
| 99999 | Randomized | yes | 99998 | 26869 | 26869 |
| 100001 | Linear | no | -1 | 100000 | 100000 |
| 100001 | Binary | no | -1 | 17 | 34 |
| 100001 | Randomized | no | -1 | 100000 | 100000 |

The randomized counts are observations from this build, not expected values or universal results. A fixed engine seed supports repeatability on the same implementation, but `uniform_int_distribution` may map engine outputs differently across standard-library implementations.

### Complexity analysis

**Linear Search: O(N) worst case.** Each failed comparison eliminates only one candidate. The remaining candidate count decreases from N to N - 1, then N - 2, and so on. If the target is absent or at the end, all N elements are examined. The best case is O(1), and a uniformly positioned present target requires `(N + 1)/2` probes on average, giving O(N) average time. Linear Search does not require ordering because it does not discard unexamined elements based on their values.

**Binary Search: O(log N) worst case.** After examining the midpoint, the ordering comparison discards roughly half the remaining candidates. The candidate count shrinks approximately as `N, N/2, N/4, ...`. After k steps the scale is `N/2^k`; reaching a constant-size range takes about `log2(N)` steps. Its best case is O(1), and its average successful-search depth for uniformly positioned distinct keys is O(log N).

Binary Search requires sorted data because a comparison with the midpoint must tell us which entire half can be discarded. On an unsorted array, a smaller target might appear to the right of the midpoint, so discarding that half could discard the answer. Random access, as provided by a vector, also makes midpoint access efficient.

## 5. Randomized Search

### Part A — Pseudocode

```text
RANDOMIZED_SEARCH(data, target, generator):
    N = length(data)
    remaining = vector of length N
    for i = 0 to N - 1:
        remaining[i] = i

    comparisons = 0
    count = N
    while count > 0:
        slot = uniformly random integer from 0 through count - 1
        index = remaining[slot]
        remaining[slot] = remaining[count - 1]
        count = count - 1

        comparisons = comparisons + 1
        if data[index] == target:
            return FOUND, index, comparisons

    return NOT_FOUND, -1, comparisons
```

The active prefix of `remaining` contains exactly the unexamined original indices. Selecting a slot chooses one of those indices. Replacing it with the final active entry and shrinking the prefix removes the selected index in constant time. This is a partial Fisher-Yates sampling strategy, without requiring a full shuffle before searching.

The random **slot** can repeat because the active vector changes, but the original data **index** selected from it never repeats. No repeated-index rejection loop is needed. If the target is absent, the active count reaches zero after exactly N comparisons and the search terminates. If the target is present, the function returns immediately at its first match. The original data vector is never modified.

### Part B — Complexity analysis

Assume random bounded-index generation has constant expected cost under the usual algorithm-analysis model. Because sampling is uniform without replacement, a fixed present target is equally likely to occupy any of the N positions in the randomized examination order.

**Best search-loop case: O(1).** The target is the first element examined, giving one comparison.

**Average search-loop case: O(N).** For a present target, the expected number of comparisons is:

```text
(1 + 2 + ... + N)/N = (N + 1)/2
```

For 100,000 distinct elements, this is 50,000.5 comparisons. An absent target always requires 100,000 comparisons. Including absent queries with any fixed probability preserves linear expected growth.

**Worst search-loop case: O(N).** The target is last in the randomized order or does not exist. Each index is examined exactly once, so there are N comparisons. Random-number generation is treated as constant-cost in this conventional model, not as a claim about a strict bound on internal random-engine draws.

**Important initialization distinction:** This implementation allocates and fills an N-element index vector at the start of every call. That setup takes O(N) time even if the first probe finds the target. Therefore, the **entire implemented randomized-search function has O(N) best, average, and worst time**, while its element-comparison/search-loop best case is O(1). Its auxiliary memory is O(N). Dataset construction in `main` is a separate O(N) setup cost common to all three searches.

### Part C — C++ implementation and constraints

The complete program in Question 4 includes `randomizedSearch`. It meets the requested constraints:

- The dataset is a C++ vector holding exactly 100,000 distinct integers.
- Only `<vector>`, `<random>`, and `<iostream>` are included.
- A second vector stores the unexamined indices; no associative containers are used.
- `uniform_int_distribution<int>` selects a random active slot.
- Removing each selected index prevents repeated examination.
- The result reports success or failure and the exact number of comparisons.
- The active count decreases on every unsuccessful step, ensuring termination for absent targets.

The fixed seed makes local demonstrations reproducible. Different seeds can be used to explore variability; changing the seed does not change the asymptotic analysis.

### Part D — Comparison

The search-phase average cases below assume a present target is uniformly distributed over data positions, or over the randomized examination order. Input preparation is discussed separately.

| Property | Linear Search | Binary Search | Randomized Search used here |
|---|---|---|---|
| Best search phase | O(1) | O(1) | O(1) |
| Average search phase | O(N) | O(log N) | O(N) |
| Worst search phase | O(N) | O(log N) | O(N) |
| Full function best case | O(1) | O(1) | O(N), including index-vector setup |
| Sorted data required? | No | Yes | No |
| Auxiliary memory | O(1) | O(1), iterative version | O(N) index vector |
| Missing target, 100,000 elements | 100,000 probes | At most 17 probes | 100,000 probes |
| Bookkeeping | Current index | Low, high, midpoint | Active indices, active count, random engine |

**Linear Search advantages and limitations:** It is simple, needs no preparation, and has sequential memory access. It is suitable for small or unsorted data, a single lookup, or workloads where likely targets occur near the beginning. Its limitation is linear work for distant or absent targets.

**Binary Search advantages and limitations:** On this already sorted, random-access dataset it needs at most 17 probes, vastly fewer than the other methods in their worst cases. It is especially useful for repeated lookups in sorted data. Its limitation is the sorted-input requirement: sorting an initially unsorted dataset just for one lookup may cost more than simply scanning it. Sorting or maintaining order must be considered separately when applicable.

**Randomized Search advantages and limitations:** It does not require sorted data and removes a fixed deterministic preference for early physical positions. A target near the end of storage might be found early in a particular randomized trial. It can be useful when randomized examination order or unbiased sampling during exploration is itself desired. However, it adds random-generation overhead, O(N) index storage, preparation cost, and less predictable memory access. It is generally not preferable to Binary Search for this sorted dataset or to a simple sequential scan for routine one-off unsorted membership queries.

Randomness does not make the search logarithmic: a failed probe excludes only the examined element, not half of the remaining candidates. The target still occupies a uniformly random rank with expected value `(N + 1)/2`, and proving absence still requires N probes. Randomization changes the examination order, not the asymptotic amount of information eliminated per failure.

## Verification Summary

- Compiled the delivered program using C++17 with `-Wall -Wextra -Wpedantic`; compilation succeeded without diagnostic output.
- Tested targets `1`, `2`, `50000`, `99999`, `100000`, `0`, `100001`, and `-5`. All three algorithms returned the expected found status and original index in all 24 results.
- Verified exact linear counts and the Binary Search relationship between probes and Boolean comparisons; every binary run used at most 17 probes.
- Verified Question 1's four comparisons and Question 2's one probe on the seven-element array.
- Instrumented the randomized function in a separate test harness with a vector of visited-index counters. For 20 seeds, absent-target searches each examined all 100,000 indices without any repeated index and terminated after exactly 100,000 probes.
- Checked empty-vector behavior: all three algorithms returned not found without examining an element.

The extra assertion header used by the independent test harness is not part of the submitted implementation. The C++ block above contains only the three permitted headers. These tests check correctness and counting behavior; they are not elapsed-time benchmarks.

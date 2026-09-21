# Insertion Sort and Algorithm Efficiency

## 1. Average-Case Analysis of Insertion Sort

Consider an array of `N` elements. Insertion sort maintains a sorted prefix and an unsorted suffix. Initially, the first element alone forms the sorted prefix. At iteration `i`, the algorithm saves `A[i]` as `key`, compares it with elements in the prefix from right to left, shifts larger elements one position to the right, and inserts the key into the resulting gap.

### Average work per iteration

Assume the elements are distinct and all input permutations are equally likely. At iteration `i`, the sorted prefix contains `i` elements. On average, half of these elements are greater than the key, so approximately `i/2` elements must be shifted. Each shift has a corresponding successful element comparison. There may also be one unsuccessful element comparison to stop the search; this adds at most constant work per iteration.

The expected total number of shifts is:

```text
Sum for i = 1 through N - 1 of i/2
= (1 + 2 + ... + (N - 1))/2
= N(N - 1)/4
```

Counting both successful comparisons and shifts doubles this quantity. Other work, including key placement and any final unsuccessful comparisons, contributes at most linear additional work. The dominant term is proportional to `N²`, so the average-case time complexity is **O(N²)** (and more tightly, Θ(N²)).

### Figure: comparisons, shifts, and insertion

The following example shows iteration `i = 3`. The underscore represents the logical gap after saving the key; it is not an additional array element.

```text
Initial array:    [ 2, 4, 7 | 3, 5 ]
                   sorted   unsorted
                              ^
                         key = 3, i = 3

Save key = 3:    [ 2, 4, 7 | _, 5 ]

Compare 7 > 3: true
Shift A[3] = A[2] (move 7 right):
                 [ 2, 4, _, 7, 5 ]

Compare 4 > 3: true
Shift A[2] = A[1] (move 4 right):
                 [ 2, _, 4, 7, 5 ]

Compare 2 > 3: false; stop shifting.
Insert key at A[1]:
                 [ 2, 3, 4, 7 | 5 ]
                      ^
               final insertion position
                   sorted      unsorted
```

The sorted prefix grows by one element after each iteration. As this prefix becomes longer, inserting a new key requires more comparisons and shifts on average, producing quadratic total growth.

## 2. Changing the Starting Position of Insertion Sort

Each trace starts independently from the same descending array:

```text
N = 5
A = [5, 4, 3, 2, 1]
```

Only the outer loop's starting index changes. The usual insertion-sort body still compares backward, shifts larger elements, and finally places the key in the gap.

### Counting rules

- Count each successful element comparison `A[j] > key` as one operation.
- Count each shift `A[j + 1] = A[j]` as one operation.
- Do not count unsuccessful comparisons, loop-condition checks, index updates, saving the key, or final key placement.
- Array indices are zero-based. The trace lists comparisons and their corresponding shifts in execution order; each successful comparison is immediately followed by its matching shift.

### Part A — Start at `i = 1`

| i | Key | Successful comparisons, in order | Shifts, in order | Key placement (not counted) | Array after iteration | Comparisons | Shifts | Total |
|---|---|---|---|---|---|---|---|---|
| 1 | 4 | `A[0] = 5 > 4` | `A[1] = A[0]` (move 5) | `A[0] = 4` | `[4, 5, 3, 2, 1]` | 1 | 1 | 2 |
| 2 | 3 | `A[1] = 5 > 3`<br>`A[0] = 4 > 3` | `A[2] = A[1]` (move 5)<br>`A[1] = A[0]` (move 4) | `A[0] = 3` | `[3, 4, 5, 2, 1]` | 2 | 2 | 4 |
| 3 | 2 | `A[2] = 5 > 2`<br>`A[1] = 4 > 2`<br>`A[0] = 3 > 2` | `A[3] = A[2]` (move 5)<br>`A[2] = A[1]` (move 4)<br>`A[1] = A[0]` (move 3) | `A[0] = 2` | `[2, 3, 4, 5, 1]` | 3 | 3 | 6 |
| 4 | 1 | `A[3] = 5 > 1`<br>`A[2] = 4 > 1`<br>`A[1] = 3 > 1`<br>`A[0] = 2 > 1` | `A[4] = A[3]` (move 5)<br>`A[3] = A[2]` (move 4)<br>`A[2] = A[1]` (move 3)<br>`A[1] = A[0]` (move 2) | `A[0] = 1` | `[1, 2, 3, 4, 5]` | 4 | 4 | 8 |

**Totals:** 10 successful comparisons + 10 shifts = **20 counted operations**.

**Final array:** `[1, 2, 3, 4, 5]`. The array is fully sorted.

### Part B — Start at `i = 2`

| i | Key | Successful comparisons, in order | Shifts, in order | Key placement (not counted) | Array after iteration | Comparisons | Shifts | Total |
|---|---|---|---|---|---|---|---|---|
| 2 | 3 | `A[1] = 4 > 3`<br>`A[0] = 5 > 3` | `A[2] = A[1]` (move 4)<br>`A[1] = A[0]` (move 5) | `A[0] = 3` | `[3, 5, 4, 2, 1]` | 2 | 2 | 4 |
| 3 | 2 | `A[2] = 4 > 2`<br>`A[1] = 5 > 2`<br>`A[0] = 3 > 2` | `A[3] = A[2]` (move 4)<br>`A[2] = A[1]` (move 5)<br>`A[1] = A[0]` (move 3) | `A[0] = 2` | `[2, 3, 5, 4, 1]` | 3 | 3 | 6 |
| 4 | 1 | `A[3] = 4 > 1`<br>`A[2] = 5 > 1`<br>`A[1] = 3 > 1`<br>`A[0] = 2 > 1` | `A[4] = A[3]` (move 4)<br>`A[3] = A[2]` (move 5)<br>`A[2] = A[1]` (move 3)<br>`A[1] = A[0]` (move 2) | `A[0] = 1` | `[1, 2, 3, 5, 4]` | 4 | 4 | 8 |

**Totals:** 9 successful comparisons + 9 shifts = **18 counted operations**.

**Final array:** `[1, 2, 3, 5, 4]`. The array is not fully sorted.

### Part C — Start at `i = 3`

| i | Key | Successful comparisons, in order | Shifts, in order | Key placement (not counted) | Array after iteration | Comparisons | Shifts | Total |
|---|---|---|---|---|---|---|---|---|
| 3 | 2 | `A[2] = 3 > 2`<br>`A[1] = 4 > 2`<br>`A[0] = 5 > 2` | `A[3] = A[2]` (move 3)<br>`A[2] = A[1]` (move 4)<br>`A[1] = A[0]` (move 5) | `A[0] = 2` | `[2, 5, 4, 3, 1]` | 3 | 3 | 6 |
| 4 | 1 | `A[3] = 3 > 1`<br>`A[2] = 4 > 1`<br>`A[1] = 5 > 1`<br>`A[0] = 2 > 1` | `A[4] = A[3]` (move 3)<br>`A[3] = A[2]` (move 4)<br>`A[2] = A[1]` (move 5)<br>`A[1] = A[0]` (move 2) | `A[0] = 1` | `[1, 2, 5, 4, 3]` | 4 | 4 | 8 |

**Totals:** 7 successful comparisons + 7 shifts = **14 counted operations**.

**Final array:** `[1, 2, 5, 4, 3]`. The array is not fully sorted.

### Part D — Correctness

| Starting index | Successful comparisons | Shifts | Counted operations | Final array | Fully sorted? |
|---|---:|---:|---:|---|---|
| `i = 1` | 10 | 10 | 20 | `[1, 2, 3, 4, 5]` | Yes |
| `i = 2` | 9 | 9 | 18 | `[1, 2, 3, 5, 4]` | No |
| `i = 3` | 7 | 7 | 14 | `[1, 2, 5, 4, 3]` | No |

Insertion sort assumes that the prefix `A[0...i-1]` is already sorted at the beginning of each iteration. Starting at `i = 1` establishes this invariant because a one-element prefix is always sorted. Inserting the next key into that prefix then preserves the invariant.

Starting at `i = 2` skips sorting the first two elements. In this example, the initial prefix `[5, 4]` is not sorted. Later insertions place smaller keys before it without correcting the relative order of `5` and `4`. The final array still contains that inversion.

Starting at `i = 3` assumes that the initial prefix `[5, 4, 3]` is sorted, which is also false. The remaining insertions move `2` and `1` before this prefix but leave its internal descending order unchanged.

Starting at `i = 2` or `i = 3` would be valid if the skipped prefix were already known to be sorted. Without that guarantee, neither change guarantees a sorted result. Fewer operations are not a valid improvement when they remove steps needed for correctness.

## 3. Improving a Search Algorithm

Let `N` be the length of the input string. The target is the capital letter `"X"`; lowercase `"x"` does not match.

### Part A — Original implementation

The original function sets a flag when it finds `"X"` but does not break out of the loop or return immediately. Setting the flag does not change the loop's continuation condition, so the loop continues until all characters have been examined.

| Location of `"X"` | Behavior | Character comparisons |
|---|---|---:|
| First character | Sets the flag immediately, then scans the rest | N |
| Somewhere in the middle | Sets the flag when it reaches X, then continues | N |
| Final character | Finds X only on the last iteration | N |
| Absent | Scans the whole string and returns false | N |

For nonempty strings, the original implementation has **O(N)** best-case, average-case, and worst-case time complexity. Its actual number of character comparisons is always `N`. An empty string requires no character comparisons and returns false in constant time.

The supplied code also assigns to `foundX` without declaring it. In strict-mode JavaScript that can cause an error; the analysis above assumes the intended local-flag behavior. A direct repair would use `let foundX = false`. The improved version below avoids this variable altogether.

### Part B — Improved implementation

```javascript
function containsX(string) {
    for (let i = 0; i < string.length; i++) {
        if (string[i] === "X") {
            return true;
        }
    }
    return false;
}
```

The function returns true as soon as it finds the target. It returns false only after examining the entire string without a match. It also returns false for an empty string.

### Part C — Complexity of the improved version

**Best case: O(1).** If the first character is `"X"`, the function makes one character comparison and returns immediately, regardless of the total string length.

**Average case: O(N), under a stated input model.** Suppose there is exactly one `"X"` and its position is uniformly distributed among the `N` positions. The expected number of comparisons is:

```text
(1 + 2 + ... + N)/N = (N + 1)/2
```

This is approximately half the original function's comparisons but still grows linearly. If no-match inputs are also included with a fixed probability, those inputs require `N` comparisons and the average remains linear under this model. Average-case results depend on the assumed distribution of inputs.

**Worst case: O(N).** If `"X"` is absent or occurs only in the last position, all `N` characters must be checked.

| Case | Original | Improved |
|---|---|---|
| Best, for nonempty strings | O(N) | O(1) |
| Average, using the model above | O(N) | O(N) |
| Worst | O(N) | O(N) |

Early exit reduces the actual work whenever the first match occurs before the final character. Under the uniform-position model, it reduces expected comparisons from `N` to `(N + 1)/2`, even though both average-case bounds remain O(N). The worst case is unchanged because the function still needs to inspect every character when there is no match.

### Example checks

| Input | Expected result | Original comparisons | Improved comparisons |
|---|---|---:|---:|
| `"Xabcd"` | true | 5 | 1 |
| `"abXcd"` | true | 5 | 3 |
| `"abcdX"` | true | 5 | 5 |
| `"abcde"` | false | 5 | 5 |
| `"x"` | false | 1 | 1 |
| `""` | false | 0 | 0 |

## Analysis and Reflection

### Why does insertion sort exhibit quadratic average- and worst-case behavior?

In the average case for uniformly random distinct elements, inserting a key shifts approximately half of the sorted prefix. Summing these growing amounts gives `N(N - 1)/4` expected shifts. In the descending worst case, every element of the prefix must shift, giving `1 + 2 + ... + (N - 1) = N(N - 1)/2` shifts. Both expressions have quadratic dominant terms.

### Why is the starting position i = 1 important?

It establishes the sorted-prefix invariant without needing any preliminary sorting. A single element is already sorted. Skipping ahead assumes a larger prefix is sorted, which is not generally true, as the traces demonstrate.

### How does reducing operations differ from reducing Big-O complexity?

Reducing operations can change a constant factor without changing the growth class. For example, reducing average search comparisons from `N` to about `N/2` leaves linear complexity. Reducing the growth class changes how work scales with input size, such as changing a particular case from O(N) to O(1). Either kind of improvement must preserve correctness.

### Why can implementations with the same worst-case Big-O perform differently?

Worst-case Big-O does not describe every input or every constant factor. Two implementations may have different best-case behavior, average comparison counts, and implementation overhead. Both search versions have O(N) worst-case time, but the early-return version often performs less work because it stops at the first match.

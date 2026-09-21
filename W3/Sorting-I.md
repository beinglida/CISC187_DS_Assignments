# Sorting Algorithms and Big-O Analysis

## 1. Linear Complexity

The algorithm takes approximately `4N + 16` steps for an input of size `N`. Its time complexity is **O(N)**.

The term `4N` grows linearly with the input size. The constant multiplier `4` changes the amount of work per element, but it does not change the linear growth pattern. The constant term `16` does not grow with `N` and becomes relatively insignificant as `N` increases. Therefore, the expression simplifies to O(N).

## 2. Quadratic Complexity

The algorithm takes approximately `2N²` steps. Its time complexity is **O(N²)** because the constant multiplier `2` does not change the quadratic growth rate.

When `N` doubles, the number of operations becomes four times as large:

```text
T(N) = 2N²
T(2N) = 2(2N)² = 8N² = 4T(N)
```

For example, an input of 10 elements requires approximately 200 steps, while an input of 20 elements requires approximately 800 steps. If `N` triples, the number of operations increases by a factor of nine.

## 3. Analyzing Multiple Sequential Loops

The `double_then_sum` function has two sequential loops:

1. The first loop executes `N` times. Each iteration doubles one number and appends it to `doubled_array`.
2. The second loop also executes `N` times because `doubled_array` contains one element for each original element. Each iteration adds one doubled value to `sum`.

The total number of loop iterations is:

```text
N + N = 2N
```

With constant-time arithmetic and amortized constant-time array append operations, the total work can be expressed as `aN + bN + c`, where `a` and `b` represent the work per iteration and `c` represents fixed setup and return work.

The final time complexity is **O(N)**. The loops run one after the other, so their work is added rather than multiplied. Having two sequential loops increases the amount of work by a constant factor but does not make the algorithm quadratic.

## 4. Multiple Constant-Time Operations

The `multiple_cases` function loops through an array containing `N` strings, so the loop executes `N` times.

Each iteration executes three conversion-and-print statements:

1. Convert the string to uppercase and print it.
2. Convert the string to lowercase and print it.
3. Capitalize the string and print it.

Following the assignment's assumption, each string operation is treated as a single constant-time operation independent of string length. Counting each conversion-and-print statement as one unit gives approximately `3N` units of work. Counting conversions and prints separately would still give a fixed number of operations per iteration and would not change the result.

The final time complexity is **O(N)**. A fixed number of constant-time operations inside each iteration changes only the constant multiplier, not the linear growth rate.

## 5. Analyzing Nested Iteration

The outer loop of `every_other` executes `N` times, once for each array element. The condition `index.even?` is checked during every outer-loop iteration.

Array indices start at zero, so the even indices are `0, 2, 4, ...`. The condition is true approximately `N/2` times. More precisely, it is true `ceil(N/2)` times, where `ceil` means rounding up to the nearest integer.

Each time the condition is true, the inner loop executes `N` times. Therefore, the total number of executions of the inner-loop body is:

```text
N × ceil(N/2) ≈ N²/2
```

For example, when `N = 5`, the even indices are `0`, `2`, and `4`. The inner loop runs five times for each of these three indices, giving `3 × 5 = 15` executions of its body.

Including the outer-loop checks, the total work consists of a quadratic term and a linear term. The final time complexity is **O(N²)** because the quadratic term dominates as `N` grows.

Processing only every other outer-loop element reduces the inner-loop work by approximately one half. This is a constant-factor reduction, so it does not change the quadratic Big-O classification.

## Analysis and Reflection

### Why are constants normally ignored in Big-O notation?

Big-O describes how the amount of work grows as the input size becomes large, rather than the exact number of steps or seconds an algorithm takes. Constant multipliers and fixed extra work do not change the growth class. For example, `N` and `4N + 16` both grow linearly. Constants can still matter for actual performance even though Big-O ignores them.

### What is the difference between O(N) and O(N²) growth?

For linear work proportional to `N`, doubling the input approximately doubles the work. For quadratic work proportional to `N²`, doubling the input approximately quadruples the work. Thus, quadratic growth becomes much more expensive as the input grows.

### Why can sequential loops and nested loops have different complexities?

Sequential loops complete one after another, so their amounts of work are added. In `double_then_sum`, two loops of `N` iterations give `N + N = 2N`, which is O(N).

In nested loops, the inner loop repeats for each qualifying outer iteration, so the relevant iteration counts are multiplied. In `every_other`, the inner loop executes `N` times for approximately `N/2` outer iterations, giving approximately `N²/2`, which is O(N²). The actual loop bounds matter; not every nested loop is automatically quadratic.

### Why does time complexity become more important for larger datasets?

Algorithms with different growth rates may both finish quickly on small datasets. As the dataset grows, their performance can diverge substantially. Increasing the input size by a factor of 100 increases linear work by approximately 100 times, but quadratic work by approximately 10,000 times. Understanding time complexity helps predict scalability and choose algorithms that remain practical for large inputs.

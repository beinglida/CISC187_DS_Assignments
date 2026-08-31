# Array Data Structure

Answers for the intro arrays assignment. I used C++ for all the code parts.

---

## Task 1: Create an array of 100 elements

To make an array of 100 elements in C++, you just pick a type, a name, and put the size in brackets.

I used `int`:

```cpp
#include <iostream>
using namespace std;

int main() {
    int arr[100];
    cout << "Array of 100 ints created." << endl;
    return 0;
}
```

`int arr[100];` means 100 integers sitting next to each other in memory. The first one is `arr[0]` and the last one is `arr[99]`. Nothing fancy — that’s the whole declaration.

---

## Task 2: Size of each element

The size of one element depends on the type. For `int` it’s usually 4 bytes, but the safe way is to ask the compiler with `sizeof`.

```cpp
#include <iostream>
using namespace std;

int main() {
    int arr[100];

    cout << "Size of one element: " << sizeof(arr[0]) << " bytes" << endl;
    cout << "Total size of the array: " << sizeof(arr) << " bytes" << endl;

    return 0;
}
```

`sizeof(arr[0])` is the size of a single slot. `sizeof(arr)` is the whole block. On a typical machine this prints 4 and 400.

---

## Task 3: Number of steps for an array with 100 elements

Assuming a plain C++ array (fixed size, we have to shift things ourselves):

| Operation | Steps |
|---|---:|
| Reading | 1 |
| Searching for a value that is not in the array | 100 |
| Insertion at the beginning | 101 |
| Insertion at the end | 1 |
| Deletion at the beginning | 99 |
| Deletion at the end | 1 |

**Reading**  
Arrays give random access. If I want `arr[10]`, I jump straight there. That’s one step.

**Searching for a value that is not in the array**  
Worst case I have to look at every element before I can say “not found.” With 100 items that’s 100 comparisons.

**Insertion at the beginning**  
There is no empty slot at index 0. I have to slide all 100 existing values one spot to the right, then write the new value at index 0. That’s 100 shifts + 1 write = 101.

**Insertion at the end**  
If we pretend there is room after the last used index (or we just overwrite / write at index 99 / 100 depending how you count a “logical” end), putting a value at the end is just one write. No shifting.

**Deletion at the beginning**  
I throw away `arr[0]` and slide the other 99 items left so there is no hole. 99 moves.

**Deletion at the end**  
Just ignore / drop the last item. One step. No shifting.

---

## Task 4: Finding every `"apple"`

If the array has N elements and I need *all* occurrences of `"apple"` (not just the first one), I have to look at every element. Stopping early would miss later apples.

So it takes **N steps**.

---

## Task 5: Memory address of an array

In C++ the array name is basically a pointer to the first element. `&` works on any individual element.

```cpp
#include <iostream>
using namespace std;

int main() {
    int arr[100];

    cout << "Array name (base address): " << arr << endl;
    cout << "Address of arr[0]: " << &arr[0] << endl;
    cout << "Address of arr[1]: " << &arr[1] << endl;

    return 0;
}
```

`arr` and `&arr[0]` print the same address. `&arr[1]` is a little further along — the gap is exactly `sizeof(int)` bytes because the elements are stored back-to-back.

---

That’s all five tasks. Arrays are fast to read by index and cheap to add/remove at the end, but inserting or deleting at the front means shifting a lot of data.

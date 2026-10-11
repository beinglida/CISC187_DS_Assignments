# Lab W7 — Implementing a Singly Linked List in C++

**Files in this repository**

| File | Contents |
|---|---|
| `linked_list.cpp` | Complete singly linked-list implementation and test driver |
| `README.md` | Diagrams, traces, test evidence, complexity analysis, and written answers |

**Compile and run**

```bash
g++ -std=c++17 -Wall -Wextra linked_list.cpp -o linked_list
./linked_list
```

The program was also compiled with `-fsanitize=address,undefined` (AddressSanitizer). It ran with no use-after-free errors and no memory leaks reported, which confirms that `remove()` and the destructor free every node correctly.

---

## Part 1 — Understanding Nodes and Pointers

```cpp
Node* first  = new Node{7, nullptr};
Node* second = new Node{13, nullptr};
Node* third  = new Node{21, nullptr};
Node* head = first;
first->next = second;
second->next = third;
```

### Diagram

```
 head
  |
  v
+------+------+     +------+------+     +------+---------+
| data | next |     | data | next |     | data |  next   |
|  7   |  *---+---->|  13  |  *---+---->|  21  | nullptr |
+------+------+     +------+------+     +------+---------+
   ^                   ^                   ^
 first               second              third
```

- `head` and `first` both hold the address of the node containing 7.
- Each `next` field holds the address of the following node.
- The last node's `next` is `nullptr`, which marks the end of the list.

### Analysis

1. **Does `head` contain the value 7?**
   No. `head` is a `Node*`, so it does not hold an integer. The value 7 is stored in the `data` field of the node that `head` points to. You get 7 by dereferencing: `head->data`.

2. **What does `head` actually store?**
   It stores the memory address of the first node (the same address stored in `first`).

3. **What does `first->next` contain?**
   The address of the second node (the node holding 13), i.e. the same value as `second`.

4. **What does `third->next` contain?**
   `nullptr`. It was created with `nullptr` and never changed, so it marks the end of the list.

5. **What happens if `head` is lost?**
   Nodes are only reachable through the chain that starts at `head`. They are not stored at known or contiguous addresses, so if `head` is overwritten and no other pointer refers to the first node, the entire list becomes unreachable. The data cannot be accessed anymore, and the nodes can never be `delete`d, so all of them become a memory leak.

---

## Part 2 — Trace a Linked List

```
head → 10 → 20 → 30 → 40 → nullptr
```

```cpp
Node* temp = head;
while (temp != nullptr) {
    cout << temp->data << " ";
    temp = temp->next;
}
```

| Iteration | `temp->data` | Where does `temp->next` lead? |
|---|---|---|
| 1 | 10 | Node containing 20 |
| 2 | 20 | Node containing 30 |
| 3 | 30 | Node containing 40 |
| 4 | 40 | `nullptr` |

Output: `10 20 30 40`

**Why the traversal stops:** At the end of iteration 4, `temp = temp->next` assigns `nullptr` to `temp`, because the last node's `next` is `nullptr`. The loop condition `temp != nullptr` is then false, so the loop exits. This also prevents dereferencing a null pointer.

### Complexity

Traversing the whole list is **O(N)**. The loop body runs exactly once per node, and each iteration does constant work (print one value, follow one pointer). There is no way to skip ahead because each node's address is only known from the previous node's `next`. N nodes means N iterations.

---

## Part 3 — Build a LinkedList Class

The complete implementation is below and in `linked_list.cpp`. It uses only raw pointers, `new`, and `delete`. It does not use `std::list` or `std::forward_list`.

```cpp
// Singly Linked List Implementation in C++
// W7 Lab - Linked Lists

#include <iostream>
#include <string>
using namespace std;

struct Node {
    int data;
    Node* next;
};

class LinkedList {
private:
    Node* head;

public:
    LinkedList();
    bool empty() const;
    void display() const;
    bool search(int value) const;
    void insertFront(int value);
    void insertBack(int value);
    bool insertAfter(int target, int value);
    bool remove(int value);
    ~LinkedList();
};

// Part 4: An empty list is represented by head == nullptr
LinkedList::LinkedList() : head(nullptr) {}

bool LinkedList::empty() const {
    return head == nullptr;
}

// Prints the list in the form: 10 -> 20 -> 30 -> nullptr
void LinkedList::display() const {
    Node* temp = head;
    cout << "head -> ";
    while (temp != nullptr) {
        cout << temp->data << " -> ";
        temp = temp->next;
    }
    cout << "nullptr" << endl;
}

// Part 7: Linear search from head
bool LinkedList::search(int value) const {
    Node* temp = head;
    while (temp != nullptr) {
        if (temp->data == value) {
            return true;
        }
        temp = temp->next;
    }
    return false;
}

// Part 5: Insert at the beginning - O(1)
void LinkedList::insertFront(int value) {
    Node* newNode = new Node{value, nullptr};
    newNode->next = head;   // 1. new node points to current first node
    head = newNode;         // 2. head now points to the new node
}

// Part 6: Insert at the end (no tail pointer) - O(N)
void LinkedList::insertBack(int value) {
    Node* newNode = new Node{value, nullptr};

    // Case 1: empty list
    if (head == nullptr) {
        head = newNode;
        return;
    }

    // Case 2 & 3: one or more nodes - walk to the last node
    Node* temp = head;
    while (temp->next != nullptr) {
        temp = temp->next;
    }
    temp->next = newNode;
}

// Part 8: Insert a new node after the first node containing target
bool LinkedList::insertAfter(int target, int value) {
    Node* temp = head;
    while (temp != nullptr && temp->data != target) {
        temp = temp->next;
    }

    if (temp == nullptr) {
        return false;   // target not found
    }

    Node* newNode = new Node{value, nullptr};
    newNode->next = temp->next;   // 1. new node points to the rest of the list
    temp->next = newNode;         // 2. target node points to the new node
    return true;
}

// Part 9: Remove the first node containing value
bool LinkedList::remove(int value) {
    // Case 1: empty list
    if (head == nullptr) {
        return false;
    }

    // Case 2: removing the head node
    if (head->data == value) {
        Node* oldHead = head;
        head = head->next;
        delete oldHead;
        return true;
    }

    // Case 3 & 4: removing a middle or last node
    Node* prev = head;
    while (prev->next != nullptr && prev->next->data != value) {
        prev = prev->next;
    }

    // Case 5: value not found
    if (prev->next == nullptr) {
        return false;
    }

    Node* toDelete = prev->next;
    prev->next = toDelete->next;   // reconnect around the node first
    delete toDelete;               // then free it
    return true;
}

// Part 11: Destructor - free every remaining node
LinkedList::~LinkedList() {
    Node* current = head;
    while (current != nullptr) {
        Node* nextNode = current->next;   // save next before deleting
        delete current;
        current = nextNode;
    }
    head = nullptr;
}
```

(The `main()` test driver is shown in Part 10.)

---

## Part 4 — Initialize the List

```cpp
LinkedList::LinkedList() : head(nullptr) {}

bool LinkedList::empty() const {
    return head == nullptr;
}
```

Test evidence (from the program output):

```
=== 1. Create an empty list ===
head -> nullptr

=== 2. Verify that it is empty ===
empty(): true
```

### Analysis — Why `nullptr` represents an empty list

`head` means "the address of the first node." An empty list has no first node, so there is no valid address to store. `nullptr` is the value that means "points to nothing," so `head == nullptr` says exactly "there is no first node."

It also makes the rest of the code consistent. Every traversal loop in my class uses `while (temp != nullptr)`. On an empty list, `temp` starts as `nullptr`, so the loop runs zero times and `search()`, `display()`, `insertAfter()`, and the destructor all work on an empty list without special handling. The "edge cases on an empty list" section of the output confirms this: `search(5)`, `remove(5)`, and `insertAfter(5, 6)` all safely return `false`.

If `head` were left uninitialized it would contain a garbage address, and `empty()` would give a wrong answer and traversal would dereference random memory.

---

## Part 5 — Insert at the Beginning

```cpp
void LinkedList::insertFront(int value) {
    Node* newNode = new Node{value, nullptr};
    newNode->next = head;   // step 1
    head = newNode;         // step 2
}
```

Diagram for inserting 5:

```
Step 0 (before):   head → 10 → 20 → 30 → nullptr

Step 1:  newNode->next = head
         [5] ─┐
              v
   head → 10 → 20 → 30 → nullptr

Step 2:  head = newNode
   head → 5 → 10 → 20 → 30 → nullptr
```

Test evidence:

```
insertFront(30): head -> 30 -> nullptr
insertFront(20): head -> 20 -> 30 -> nullptr
insertFront(10): head -> 10 -> 20 -> 30 -> nullptr
```

The first call works on an empty list too: `newNode->next = head` sets `next` to `nullptr`, which correctly makes the new node both the first and last node.

### Pointer Analysis

The order matters because `head` is the only pointer to the existing list. Step 1 copies that address into `newNode->next` while it is still available. Only after it is saved is it safe to overwrite `head`.

If `head` were changed first:

```cpp
head = newNode;          // the old list's address is overwritten
newNode->next = head;    // now newNode points to itself
```

The address of the old first node (10) is lost, so 10 → 20 → 30 is unreachable and leaks. Also, `newNode->next = head` would make the new node point to itself, creating a one-node cycle, so `display()` would loop forever.

### Complexity

`insertFront()` is **O(1)**. It performs one allocation and two pointer assignments regardless of how many nodes are in the list. It never looks at any node other than the one `head` already points to, so no traversal is required. The front of the list is always directly available through `head`.

---

## Part 6 — Insert at the End

```cpp
void LinkedList::insertBack(int value) {
    Node* newNode = new Node{value, nullptr};
    if (head == nullptr) {          // empty list
        head = newNode;
        return;
    }
    Node* temp = head;
    while (temp->next != nullptr) { // stop at the last node
        temp = temp->next;
    }
    temp->next = newNode;
}
```

How each required case is handled:

| Case | What happens |
|---|---|
| Empty list | `head` is `nullptr`, so the new node becomes `head`. The loop is never reached (accessing `head->next` would crash). |
| One node | `temp = head`, and `head->next` is already `nullptr`, so the loop runs 0 times and `head->next = newNode`. |
| Multiple nodes | The loop advances until `temp->next == nullptr`, i.e. `temp` is the last node, then links the new node after it. |

Note the loop condition is `temp->next != nullptr`, not `temp != nullptr`. I need to stop *on* the last node so I can change its `next`. If the loop ran until `temp == nullptr`, I would have walked past the end and have no node to attach to.

Test evidence:

```
insertBack(7) on empty list: head -> 7 -> nullptr
insertBack(8) on one-node list: head -> 7 -> 8 -> nullptr
insertBack(40): head -> 10 -> 20 -> 30 -> 40 -> nullptr
insertBack(50): head -> 10 -> 20 -> 30 -> 40 -> 50 -> nullptr
```

### Analysis

**Why insertion at the end is O(N) with only `head`:** The class only knows where the first node is. To attach a node at the end, it needs the address of the last node, and the only way to find it is to start at `head` and follow `next` pointers one at a time. For N nodes the `while` loop runs N − 1 times. The actual link (`temp->next = newNode`) is O(1), but finding the place to link it costs O(N).

**With a tail pointer:** If the class also stored `Node* tail` (the address of the last node), insertion at the end becomes:

```cpp
tail->next = newNode;
tail = newNode;
```

That is **O(1)**, because no searching is needed. The cost is that every operation that can change the last node (`insertBack`, `insertAfter` on the last node, `remove` of the last node, inserting into an empty list) must also keep `tail` correct.

---

## Part 7 — Search the List

```cpp
bool LinkedList::search(int value) const {
    Node* temp = head;
    while (temp != nullptr) {
        if (temp->data == value) return true;
        temp = temp->next;
    }
    return false;
}
```

Test evidence (list is `10 -> 20 -> 30 -> 40 -> 50`):

```
search(10) [beginning]: true
search(30) [middle]:    true
search(50) [end]:       true
search(99) [missing]:   false
```

### Complexity Analysis

| Case | Big-O | List arrangement that causes it |
|---|---|---|
| Best case | **O(1)** | The value is in the first node. The function compares `head->data`, matches, and returns immediately. Example: `search(10)`. |
| Worst case | **O(N)** | The value is in the last node, or not in the list at all. Every node must be compared. Example: `search(50)` checks all 5 nodes; `search(99)` checks all 5 and then reaches `nullptr`. |

On average, if the value is present at a random position, about N/2 nodes are checked, which is still O(N). Even if the list were sorted, it would not help much, because a linked list cannot jump to the middle element the way binary search requires.

---

## Part 8 — Insert After a Specific Value

### Pointer Reasoning (before writing the function)

To insert `25` after `20` in `10 → 20 → 30 → 40`, with `temp` pointing to the node 20:

1. **`newNode->next = temp->next;`** The new node points to 30, so it grabs the rest of the list.
2. **`temp->next = newNode;`** Node 20 now points to the new node.

```
Before:        10 → [20] → 30 → 40 → nullptr

Step 1:        10 → [20] → 30 → 40 → nullptr
                            ^
                     [25] ──┘

Step 2:        10 → [20] → [25] → 30 → 40 → nullptr
```

These must happen in this order. If `temp->next = newNode` ran first, the address of 30 would be overwritten before it was saved, and 30 → 40 would be disconnected and leaked.

### Implementation

```cpp
bool LinkedList::insertAfter(int target, int value) {
    Node* temp = head;
    while (temp != nullptr && temp->data != target) {
        temp = temp->next;
    }
    if (temp == nullptr) return false;

    Node* newNode = new Node{value, nullptr};
    newNode->next = temp->next;
    temp->next = newNode;
    return true;
}
```

The node is allocated only after the target is found, so a failed call does not allocate memory that would then leak. If the target is the last node, `temp->next` is `nullptr`, so the new node correctly becomes the new last node.

Test evidence:

```
=== 7. Insert after a value in the middle ===
insertAfter(20, 25): true
head -> 10 -> 20 -> 25 -> 30 -> 40 -> 50 -> nullptr

=== 8. Attempt insertion after a nonexistent value ===
insertAfter(99, 100): false
head -> 10 -> 20 -> 25 -> 30 -> 40 -> 50 -> nullptr
```

### Complexity

The insertion itself is **O(1)**: two pointer assignments, no matter how long the list is, and no nodes have to be shifted (unlike an array, where everything after the insertion point must move).

The complete operation is still **O(N)** because before the two pointer changes can happen, the function has to find the node containing `target`. That search starts at `head` and may visit every node (worst case: target is last or missing). So the total cost is O(N) search + O(1) link = O(N).

---

## Part 9 — Delete a Node

```cpp
bool LinkedList::remove(int value) {
    if (head == nullptr) return false;              // empty list

    if (head->data == value) {                      // remove first node
        Node* oldHead = head;
        head = head->next;
        delete oldHead;
        return true;
    }

    Node* prev = head;                              // find node before target
    while (prev->next != nullptr && prev->next->data != value) {
        prev = prev->next;
    }
    if (prev->next == nullptr) return false;        // not found

    Node* toDelete = prev->next;
    prev->next = toDelete->next;                    // reconnect first
    delete toDelete;                                // then free
    return true;
}
```

Diagram for `remove(30)` in `10 → 20 → 30 → 40`:

```
Before:     10 → [20] → (30) → 40 → nullptr
                 prev  toDelete

prev->next = toDelete->next:
            10 → [20] ─────────→ 40 → nullptr
                        (30) ──┘   (30 still points to 40, but nothing points to 30)

delete toDelete:
            10 → 20 → 40 → nullptr
```

How each required case is handled:

| Case | Handled by | Evidence |
|---|---|---|
| Empty list | First `if` returns `false` | `remove(5) on empty list: false` |
| First node | Second `if`: move `head` forward, delete old head | `remove(10): true` → `20 -> 25 -> 30 -> 40 -> 50` |
| Middle node | `prev` stops before it; link around it | `remove(30): true` → `20 -> 25 -> 40 -> 50` |
| Last node | Same as middle; `toDelete->next` is `nullptr`, so `prev` becomes the new last node | `remove(50): true` → `20 -> 25 -> 40` |
| Not present | Loop reaches the end, returns `false`, list unchanged | `remove(99): false` → `20 -> 25 -> 40` |
| Only node | Head case; `head` becomes `nullptr` | `remove(8): true` → `head -> nullptr`, `empty(): true` |

### Analysis

1. **Why does deleting the head require special handling?**
   For every other node, removing it means changing the `next` pointer of the node before it. The first node has no previous node. The "pointer that points to it" is `head` itself, a member of the class, so the code must update `head` instead of some `prev->next`. My loop also starts with `prev = head` and checks `prev->next`, so it never examines the head node; that case must be checked before the loop.

2. **Why must access to the node be preserved before it is deleted?**
   In a singly linked list, the node to be deleted holds the only address of the rest of the list in its `next` field. If I deleted it first, I could no longer read `toDelete->next` safely, so I would not know where 40 is and the rest of the list would be lost. That is why the code saves the node in `toDelete`, reconnects `prev->next = toDelete->next`, and only then calls `delete`. Similarly, if I just did `prev->next = prev->next->next` without saving the node, I would lose its address and could never free it.

3. **What is a memory leak?**
   Memory allocated with `new` that is never released with `delete`, after the program has lost every pointer to it. The memory stays reserved for the rest of the program but can never be used or freed.

4. **Why is accessing a node after `delete` unsafe?**
   After `delete`, the memory is returned to the system and the pointer becomes a dangling pointer. It still holds the old address, but that memory may have been reused for something else or marked invalid. Reading it may return garbage, and writing to it may corrupt other data. This is undefined behavior: the program might seem to work, crash, or fail in unpredictable ways later. (AddressSanitizer reports this as "heap-use-after-free"; my program had no such errors.)

---

## Part 10 — Test the Complete Linked List

### Test driver

```cpp
string yesNo(bool b) {
    return b ? "true" : "false";
}

int main() {
    cout << "=== 1. Create an empty list ===" << endl;
    LinkedList list;
    list.display();

    cout << "\n=== 2. Verify that it is empty ===" << endl;
    cout << "empty(): " << yesNo(list.empty()) << endl;

    cout << "\n=== Edge cases on an empty list ===" << endl;
    cout << "search(5) on empty list: " << yesNo(list.search(5)) << endl;
    cout << "remove(5) on empty list: " << yesNo(list.remove(5)) << endl;
    cout << "insertAfter(5, 6) on empty list: " << yesNo(list.insertAfter(5, 6)) << endl;

    cout << "\n=== 3. Insert several values at the front ===" << endl;
    list.insertFront(30);
    cout << "insertFront(30): "; list.display();
    list.insertFront(20);
    cout << "insertFront(20): "; list.display();
    list.insertFront(10);
    cout << "insertFront(10): "; list.display();
    cout << "empty(): " << yesNo(list.empty()) << endl;

    cout << "\n=== 4. Insert several values at the back ===" << endl;
    list.insertBack(40);
    cout << "insertBack(40): "; list.display();
    list.insertBack(50);
    cout << "insertBack(50): "; list.display();

    cout << "\n=== 5. Display the list ===" << endl;
    list.display();

    cout << "\n=== 6. Search for existing and missing values ===" << endl;
    cout << "search(10) [beginning]: " << yesNo(list.search(10)) << endl;
    cout << "search(30) [middle]:    " << yesNo(list.search(30)) << endl;
    cout << "search(50) [end]:       " << yesNo(list.search(50)) << endl;
    cout << "search(99) [missing]:   " << yesNo(list.search(99)) << endl;

    cout << "\n=== 7. Insert after a value in the middle ===" << endl;
    cout << "insertAfter(20, 25): " << yesNo(list.insertAfter(20, 25)) << endl;
    list.display();

    cout << "\n=== 8. Attempt insertion after a nonexistent value ===" << endl;
    cout << "insertAfter(99, 100): " << yesNo(list.insertAfter(99, 100)) << endl;
    list.display();

    cout << "\n=== 9. Remove the first node ===" << endl;
    cout << "remove(10): " << yesNo(list.remove(10)) << endl;
    list.display();

    cout << "\n=== 10. Remove a middle node ===" << endl;
    cout << "remove(30): " << yesNo(list.remove(30)) << endl;
    list.display();

    cout << "\n=== 11. Remove the last node ===" << endl;
    cout << "remove(50): " << yesNo(list.remove(50)) << endl;
    list.display();

    cout << "\n=== 12. Attempt to remove a nonexistent value ===" << endl;
    cout << "remove(99): " << yesNo(list.remove(99)) << endl;
    list.display();

    cout << "\n=== 13. Display the final list ===" << endl;
    list.display();

    cout << "\n=== Single-node edge case ===" << endl;
    LinkedList single;
    single.insertBack(7);
    cout << "insertBack(7) on empty list: "; single.display();
    single.insertBack(8);
    cout << "insertBack(8) on one-node list: "; single.display();
    cout << "remove(7): " << yesNo(single.remove(7)) << endl;
    single.display();
    cout << "remove(8): " << yesNo(single.remove(8)) << endl;
    single.display();
    cout << "empty(): " << yesNo(single.empty()) << endl;

    return 0;   // destructors run here and free all remaining nodes
}
```

### Program output

```
=== 1. Create an empty list ===
head -> nullptr

=== 2. Verify that it is empty ===
empty(): true

=== Edge cases on an empty list ===
search(5) on empty list: false
remove(5) on empty list: false
insertAfter(5, 6) on empty list: false

=== 3. Insert several values at the front ===
insertFront(30): head -> 30 -> nullptr
insertFront(20): head -> 20 -> 30 -> nullptr
insertFront(10): head -> 10 -> 20 -> 30 -> nullptr
empty(): false

=== 4. Insert several values at the back ===
insertBack(40): head -> 10 -> 20 -> 30 -> 40 -> nullptr
insertBack(50): head -> 10 -> 20 -> 30 -> 40 -> 50 -> nullptr

=== 5. Display the list ===
head -> 10 -> 20 -> 30 -> 40 -> 50 -> nullptr

=== 6. Search for existing and missing values ===
search(10) [beginning]: true
search(30) [middle]:    true
search(50) [end]:       true
search(99) [missing]:   false

=== 7. Insert after a value in the middle ===
insertAfter(20, 25): true
head -> 10 -> 20 -> 25 -> 30 -> 40 -> 50 -> nullptr

=== 8. Attempt insertion after a nonexistent value ===
insertAfter(99, 100): false
head -> 10 -> 20 -> 25 -> 30 -> 40 -> 50 -> nullptr

=== 9. Remove the first node ===
remove(10): true
head -> 20 -> 25 -> 30 -> 40 -> 50 -> nullptr

=== 10. Remove a middle node ===
remove(30): true
head -> 20 -> 25 -> 40 -> 50 -> nullptr

=== 11. Remove the last node ===
remove(50): true
head -> 20 -> 25 -> 40 -> nullptr

=== 12. Attempt to remove a nonexistent value ===
remove(99): false
head -> 20 -> 25 -> 40 -> nullptr

=== 13. Display the final list ===
head -> 20 -> 25 -> 40 -> nullptr

=== Single-node edge case ===
insertBack(7) on empty list: head -> 7 -> nullptr
insertBack(8) on one-node list: head -> 7 -> 8 -> nullptr
remove(7): true
head -> 8 -> nullptr
remove(8): true
head -> nullptr
empty(): true
```

### How the output verifies the list stays connected

After every structural change (`insertFront`, `insertBack`, `insertAfter`, `remove`), the program calls `display()`, which walks from `head` all the way to `nullptr`. If any `next` pointer were broken, the printout would be missing nodes; if a cycle were created, it would never reach `nullptr`. Every printout above shows all expected values in the correct order ending in `nullptr`, so the list remained correctly linked after every step. The extra single-node test shows `insertBack` and `remove` also work when the list has exactly one node, and that removing the last remaining node correctly returns the list to `head -> nullptr` with `empty()` true.

---

## Part 11 — Memory Management

```cpp
LinkedList::~LinkedList() {
    Node* current = head;
    while (current != nullptr) {
        Node* nextNode = current->next;   // save next before deleting
        delete current;
        current = nextNode;
    }
    head = nullptr;
}
```

The destructor runs automatically when `list` and `single` go out of scope at the end of `main()`. In my final test, `list` still contains `20 → 25 → 40`, so the destructor frees those three nodes. It saves `current->next` in `nextNode` before deleting `current`, because after `delete current` it would be unsafe to read `current->next`.

Running the program with AddressSanitizer (`-fsanitize=address`) reported **no leaks**, which confirms every node created with `new` was released with `delete`, either by `remove()` or by the destructor.

### Analysis

**What happens to the allocated memory if nodes are created but never deleted?**
Each `new Node` reserves memory on the heap. Heap memory is not freed automatically when a pointer variable goes out of scope; only the pointer itself (on the stack) disappears. The node stays allocated for the rest of the program's run. If this happens repeatedly (for example inside a loop or a long-running server), the program's memory use keeps growing, which can slow the system and eventually cause allocation to fail.

**Why is it called a memory leak?**
The memory is still reserved, but the program no longer has any pointer to it, so it can neither use it nor `delete` it. The memory has "leaked" out of the program's control, just like water leaking out of a container.

**Why is memory management especially important for linked structures with raw pointers?**
- Every node is a separate `new` allocation, so a list of N nodes needs N separate `delete` calls. Destroying `head` alone frees nothing else.
- Nodes are only reachable through the chain. Losing one pointer (for example, overwriting `head` in the wrong order, or deleting a node before reconnecting its neighbors) can make every node after it unreachable at once, leaking a whole section of the list.
- Raw pointers do not track ownership. The compiler does not stop me from deleting a node twice or using one after it is deleted. The class has to guarantee correctness itself, which is why `remove()` reconnects before deleting and the destructor walks the whole list.

---

## Part 12 — Complexity Analysis

Assume the list has N nodes and maintains a `head` pointer but no `tail` pointer.

| Operation | Big-O | Explanation |
|---|---|---|
| Access element at position i | **O(N)** (O(i) exactly) | There is no index arithmetic. To reach position i, the code must start at `head` and follow `next` i times. In the worst case (i = N − 1), that is N − 1 steps. |
| Search | **O(N)** | `search()` compares nodes one by one from `head`. Best case O(1) if the value is first, but worst case (value last or missing) checks all N nodes. |
| Insert at beginning | **O(1)** | `insertFront()` allocates one node and changes two pointers (`newNode->next = head; head = newNode;`). It never visits other nodes. |
| Insert at end | **O(N)** | `insertBack()` must walk from `head` to the last node (N − 1 steps) because there is no `tail`. The final link is O(1). |
| Insert after a specified value | **O(N)** | Finding `target` takes up to N steps. Once found, the two pointer changes are O(1). Total: O(N). |
| Delete first node | **O(1)** | `remove()` checks `head->data`, moves `head` to `head->next`, and deletes the old head. No traversal. |
| Delete specified value | **O(N)** | The code must find the node *before* the target (`prev`) by traversal, up to N steps. The reconnect and `delete` are O(1). |
| Traverse entire list | **O(N)** | `display()` and the destructor visit each node exactly once, constant work per node. |

The pattern: any operation at the **front** is O(1) because `head` gives direct access. Any operation that needs a **specific position or value** is O(N) because nodes can only be reached by following the chain from `head`. The pointer manipulation itself is always O(1); the cost comes from finding where to manipulate.

---

## Part 13 — Linked List vs. Array

Both contain N elements.

| Aspect | Array | Singly Linked List |
|---|---|---|
| Access element at index N/2 | **O(1)**. The address is computed directly: `base + (N/2) × sizeof(int)`. | **O(N)**. Must follow `next` N/2 times from `head`. |
| Insert at the beginning | **O(N)**. Every existing element must shift one position right to make room at index 0 (and the array may need to be resized). | **O(1)**. Create a node, point it at `head`, update `head`. |
| Remove the first element | **O(N)**. Every remaining element shifts one position left to fill index 0. | **O(1)**. Move `head` to `head->next`, delete the old node. |
| Search for a value | **O(N)** if unsorted (linear scan). **O(log N)** if sorted, using binary search. | **O(N)** whether sorted or not. Binary search is not practical without O(1) access to the middle. |
| Memory layout | Contiguous block. Elements are next to each other in memory, which is cache-friendly and fast to scan. | Nodes are scattered anywhere on the heap, connected only by pointers. Poor cache locality; each step may be a cache miss. |
| Ability to grow dynamically | A static array has a fixed size. A dynamic array/`vector` can grow, but growing may require allocating a larger block and copying all N elements (amortized O(1) for append). | Grows one node at a time with `new`. No copying of existing elements, and no need for a large contiguous block. |
| Additional memory per element | None (just the element itself; a `vector` may have some unused reserved capacity). | One extra pointer per node (`next`): usually 8 bytes on a 64-bit system, plus allocator overhead per `new`. For `int` data (4 bytes), the overhead is larger than the data itself. |

### Analysis

Linked lists can insert and delete efficiently because elements are not stored by position. To insert or delete, I only change a couple of `next` pointers; none of the other nodes move. In an array, the position of an element *is* its index, so inserting or deleting near the front forces every later element to shift.

The same property is why linked lists cannot offer O(1) random access. An array element's address can be calculated from its index because elements are contiguous and equal-sized. In a linked list, the address of node i is stored only inside node i − 1, which is known only from node i − 2, and so on back to `head`. There is no formula to jump to node i; the chain must be followed. So the flexibility that makes insertion cheap is the same thing that makes indexing expensive.

---

## Part 14 — Reasoning About a Tail Pointer

Suppose the class maintains both `Node* head;` and `Node* tail;`.

1. **Which operation becomes significantly faster?**
   `insertBack()`. In my implementation it walks the whole list to find the last node. With `tail`, it already knows where the last node is.

2. **What is its new Big-O complexity?**
   **O(1)**:
   ```cpp
   Node* newNode = new Node{value, nullptr};
   if (head == nullptr) { head = tail = newNode; }
   else { tail->next = newNode; tail = newNode; }
   ```

3. **Does maintaining `tail` make searching O(1)?**
   No. `tail` only gives direct access to the last node. To find an arbitrary value, the code still has to check nodes one by one from `head`. Search remains **O(N)**. (Checking only whether the *last* value matches becomes O(1), but that is not general search.)

4. **Does maintaining `tail` make deleting the final node O(1) in a singly linked list?**
   **No.** It is still **O(N)**.

5. **Explanation:**
   Deleting the last node requires two things: freeing the last node, and making the second-to-last node the new end by setting its `next` to `nullptr` (and updating `tail` to point to it). `tail` gives me the last node, but in a singly linked list nodes only point forward. The last node has no pointer back to its predecessor. The only way to find the second-to-last node is to start at `head` and walk until `temp->next == tail`, which takes O(N). This is exactly the `prev` search that my `remove()` function already does. A doubly linked list fixes this because `tail->prev` gives the predecessor directly.

---

## Part 15 — Singly vs. Doubly Linked Lists

```cpp
struct Node {
    int data;
    Node* prev;
    Node* next;
};
```

**What additional information does `prev` provide?**
The address of the node immediately before the current node. In a singly linked list, a node only knows its successor; with `prev`, it also knows its predecessor.

**Why can a doubly linked list traverse in both directions?**
Forward traversal follows `next` from `head` to `nullptr`, as in my class. Backward traversal starts at `tail` and follows `prev` until reaching `nullptr` (the `prev` of the first node). Because every node has links in both directions, you can move either way from any node.

**How does the additional pointer affect memory usage?**
Each node stores one more pointer, usually 8 more bytes on a 64-bit system. For a node holding an `int`, a singly linked node is about 16 bytes (4 data + padding + 8 next) and a doubly linked node is about 24 bytes (4 + padding + 8 + 8). So the per-node overhead grows by roughly 50% in this case, and every insert or delete must update twice as many pointers.

**If you already have a pointer directly to a node, why is deletion easier in a doubly linked list?**
To delete a node, its predecessor's `next` must be changed to skip it. In my singly linked `remove()`, I had to traverse from `head` to find `prev` (O(N)), because a node does not know its predecessor. In a doubly linked list, the predecessor is `node->prev`, so deletion is O(1):

```cpp
if (node->prev) node->prev->next = node->next; else head = node->next;
if (node->next) node->next->prev = node->prev; else tail = node->prev;
delete node;
```

No search for the predecessor is needed.

---

## Part 16 — Circular Linked Lists

```
head
 ↓
10 → 20 → 30 → 40
 ↑              |
 |______________|
```

In a circular singly linked list, `last->next == head` instead of `nullptr`.

**Why would a normal `while (temp != nullptr)` traversal fail?**
No node's `next` is ever `nullptr`. After 40, `temp` moves back to 10 and continues forever, so the condition `temp != nullptr` is never false and the loop is infinite (printing 10 20 30 40 10 20 30 40 ...). My `display()` and destructor would never terminate on such a list, and the destructor would also try to delete already-deleted nodes.

**What condition determines when one complete traversal has finished?**
Stop when `temp` returns to `head`. Because the first node must be processed before checking, a `do-while` loop fits:

```cpp
if (head != nullptr) {
    Node* temp = head;
    do {
        cout << temp->data << " ";
        temp = temp->next;
    } while (temp != head);
}
```

The empty-list check is still needed because `head` can be `nullptr`.

**Why are circular linked lists suitable for repeated cyclic processing?**
After the last element, the next element is automatically the first one, so the list never "runs out." A program can keep calling `current = current->next` to move to the next item forever, without checking for the end or resetting to `head`. Adding or removing participants is still just pointer changes.

**One application where circular behavior is useful:**
Round-robin CPU scheduling. The operating system keeps ready processes in a circular list, gives the current process a time slice, then moves to `current->next`. After the last process, it automatically comes back to the first one. Other examples: taking turns in a multiplayer game, or a music playlist on repeat.

---

## Analysis and Reflection

**Why linked lists do not require contiguous memory.**
Each node is allocated separately with `new` and can be anywhere on the heap. The order of the list is not determined by where nodes are in memory but by the `next` pointers. In my program, `insertFront(10)` created a node that comes first in the list even though it was allocated after 20 and 30. The structure only needs each node to store its successor's address.

**The role of the head pointer.**
`head` is the single entry point to the list. Every operation in my class begins with `head`: `display()`, `search()`, `insertBack()`, `insertAfter()`, `remove()`, and the destructor all start with `temp = head` or `current = head`. It also represents the empty state (`head == nullptr`). Operations that change the first node (`insertFront`, removing the first node, inserting into an empty list) must update `head`; losing it would lose the entire list.

**Why traversal is O(N).**
To visit every node, `display()` follows one `next` pointer per node until it reaches `nullptr`. There is no way to skip a node because each node's address is only stored in the previous node. N nodes means N iterations of constant work.

**Why insertion at the beginning is O(1).**
`insertFront()` only touches `head` and the new node: `newNode->next = head; head = newNode;`. The number of steps is the same for a list of 3 nodes or 3 million nodes. In my output, each `insertFront` call immediately placed the value at the front without visiting any other node.

**Why insertion at the end is O(N) without a tail pointer.**
`insertBack()` needs the last node, but the class only stores the first. Its `while (temp->next != nullptr)` loop walks the whole chain. Adding 50 to `10 → 20 → 30 → 40` took 3 pointer hops before the link could be made; a longer list would take proportionally more.

**Why deletion requires careful pointer manipulation.**
The node being removed is the only holder of the address of the rest of the list. My `remove()` saves the node in `toDelete`, reconnects `prev->next = toDelete->next`, and only then calls `delete`. Doing these out of order either disconnects the remaining nodes (leaking them) or reads freed memory. The head case also needs separate code because `head` itself must change. The test output after `remove(10)`, `remove(30)`, and `remove(50)` shows the list stayed fully connected each time.

**Why dynamically allocated nodes must eventually be deallocated.**
Every node came from `new`, and heap memory is not reclaimed automatically. Without `delete` in `remove()` and the destructor, every node ever created would remain allocated after it became unreachable. Running my program under AddressSanitizer reported no leaks, which shows that each `new` had a matching `delete`.

**Why a linked list cannot perform binary search efficiently like an indexed array.**
Binary search repeatedly jumps to the middle of the remaining range. In an array that jump is O(1). In a linked list, reaching the middle requires walking N/2 nodes, then N/4, and so on, which adds up to about N steps of traversal. The total becomes O(N), no better than the simple linear `search()` I implemented, so the advantage of binary search disappears.

**When a linked list may be preferable to an array or vector.**
- Frequent insertions or deletions at the front, or at a position you already have a pointer to (O(1) pointer changes, no shifting).
- When the size changes a lot and is unknown in advance, and you want to avoid resizing and copying a large block.
- When elements must not move in memory (pointers to nodes stay valid after other inserts/deletes, while a `vector` may relocate all elements when it grows).
- For implementing structures like stacks, queues, or round-robin schedulers.

**When an array or vector may be preferable to a linked list.**
- When you need random access by index (O(1) vs O(N)).
- When you need binary search on sorted data.
- When most operations are scanning or appending at the end; contiguous memory is much more cache-friendly, so in practice a `vector` is often faster even for operations with the same Big-O.
- When memory is tight; a vector stores no extra pointer per element, while my linked list uses an extra 8-byte `next` pointer for every 4-byte `int`.

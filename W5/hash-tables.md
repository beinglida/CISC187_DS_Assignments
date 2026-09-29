# Hash Tables and Linear Probing

## Part 1 — Understanding Hash Functions

For this part only, the table has **10 slots**, indexed 0 through 9. The index is the sum of the decimal digits modulo 10.

| Key | Digit calculation | Digit sum | Index |
|---|---|---:|---:|
| 555223 | 5 + 5 + 5 + 2 + 2 + 3 | 22 | 2 |
| 555980 | 5 + 5 + 5 + 9 + 8 + 0 | 32 | 2 |
| 555000 | 5 + 5 + 5 + 0 + 0 + 0 | 15 | 5 |
| 555890 | 5 + 5 + 5 + 8 + 9 + 0 | 32 | 2 |

Three distinct keys map to index 2. This is a **collision**. A nonnegative digit sum modulo 10 is always between 0 and 9, so it is a valid index. Increasing the table size may reduce collisions but does not guarantee their elimination. In particular, 555980 and 555890 have identical digit sums and collide for every table size with this hash function. More generally, mapping a larger key space into a smaller finite set of slots necessarily permits collisions.

## Part 2 — Implement a Hash Function

The complete implementation below provides `int hashFunction(int key, int tableSize)`. It repeatedly extracts the last digit using `% 10`, adds its magnitude, and removes it using integer division by 10. It then returns `sum % tableSize`.

The function rejects nonpositive table sizes. It handles zero and negative keys, including `INT_MIN`, without negating the whole key. Negative keys use the sum of their magnitude's digits, so positive and negative versions may collide but remain distinct stored keys. Assertions in `main` verify all four manual results using table size 10.

## Part 3 — Build a Hash Table

The operational hash table has **11 slots**, as required; it does not reuse the 10-slot size from Part 1. Each slot contains a `Record` with an integer key and string value, plus a state: `EMPTY`, `OCCUPIED`, or `DELETED`. A fixed `std::array` stores the slots. No `std::map` or `std::unordered_map` is used.

### Complete C++ implementation and experiment driver

```cpp
#include <array>
#include <cassert>
#include <climits>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

int hashFunction(int key, int tableSize) {
    if (tableSize <= 0) throw std::invalid_argument("positive table size required");
    // Individual signed remainders avoid negating INT_MIN.
    int sum = 0;
    do { int digit = key % 10; sum += digit < 0 ? -digit : digit; key /= 10; }
    while (key != 0);
    return sum % tableSize;
}
struct Record { int key = 0; std::string value; };
enum class State { EMPTY, OCCUPIED, DELETED };
struct Slot { Record record; State state = State::EMPTY; };
struct InsertResult { bool success; bool updated; int probes; int collisions; };
struct SearchResult { bool found; int index; int probes; std::string value; };
class HashTable {
    static constexpr int CAPACITY = 11;
    std::array<Slot, CAPACITY> slots{};
    int occupied = 0;
    void place(int index, int key, const std::string& value) {
        slots[index].record = {key, value};
        slots[index].state = State::OCCUPIED;
        ++occupied;
    }
public:
    int size() const { return occupied; }
    double loadFactor() const { return static_cast<double>(occupied) / CAPACITY; }
    InsertResult insert(int key, const std::string& value) {
        int home = hashFunction(key, CAPACITY), firstDeleted = -1;
        int collisions = 0;
        for (int i = 0; i < CAPACITY; ++i) {
            int index = (home + i) % CAPACITY;
            Slot& slot = slots[index];
            if (slot.state == State::OCCUPIED) {
                if (slot.record.key == key) {
                    slot.record.value = value;
                    return {true, true, i + 1, collisions};
                }
                ++collisions; // Encounter with an occupied different key.
            } else if (slot.state == State::DELETED) {
                if (firstDeleted == -1) firstDeleted = index;
            } else {
                place(firstDeleted == -1 ? index : firstDeleted, key, value);
                return {true, false, i + 1, collisions};
            }
        }
        if (firstDeleted != -1) {
            place(firstDeleted, key, value);
            return {true, false, CAPACITY, collisions};
        }
        return {false, false, CAPACITY, collisions};
    }
    SearchResult search(int key) const {
        int home = hashFunction(key, CAPACITY);
        for (int i = 0; i < CAPACITY; ++i) {
            int index = (home + i) % CAPACITY;
            const Slot& slot = slots[index];
            if (slot.state == State::EMPTY) return {false, -1, i + 1, ""};
            if (slot.state == State::OCCUPIED && slot.record.key == key)
                return {true, index, i + 1, slot.record.value};
        }
        return {false, -1, CAPACITY, ""};
    }
    bool remove(int key) {
        SearchResult result = search(key);
        if (!result.found) return false;
        slots[result.index].state = State::DELETED;
        slots[result.index].record.value.clear();
        --occupied;
        return true;
    }
    void display() const {
        std::cout << "Actual | State | Key | Value | Home\n";
        for (int i = 0; i < CAPACITY; ++i) {
            const Slot& slot = slots[i];
            std::cout << i << " | ";
            if (slot.state == State::EMPTY) std::cout << "EMPTY | - | - | -\n";
            else if (slot.state == State::DELETED) std::cout << "DELETED | - | - | -\n";
            else std::cout << "OCCUPIED | " << slot.record.key << " | "
                << slot.record.value << " | " << hashFunction(slot.record.key, CAPACITY) << '\n';
        }
    }
};
void reportSearch(const HashTable& table, int key) {
    auto r = table.search(key);
    std::cout << "SEARCH " << key << " found=" << r.found << " index=" << r.index
              << " probes=" << r.probes << '\n';
}
void experiment(const char* name, const std::vector<int>& keys) {
    HashTable table;
    int total = 0, collisions = 0, maximum = 0;
    for (int key : keys) {
        auto r = table.insert(key, "Test record");
        assert(r.success && !r.updated);
        total += r.probes; collisions += r.collisions;
        if (r.probes > maximum) maximum = r.probes;
        std::cout << name << " n=" << table.size() << " load=" << table.loadFactor()
                  << " collisions=" << collisions << " avg="
                  << static_cast<double>(total) / table.size() << " max=" << maximum << '\n';
    }
}
void edgeTests() {
    HashTable t;
    assert(!t.search(1).found && !t.remove(1));
    for (int key = 0; key < 11; ++key) assert(t.insert(key, "value").success);
    assert(t.size() == 11 && t.loadFactor() == 1.0);
    auto failed = t.insert(999, "full");
    assert(!failed.success && failed.probes == 11);
    assert(!t.search(999).found && t.search(999).probes == 11);
    assert(t.insert(10, "updated full").updated && t.size() == 11);
    assert(t.remove(0));
    auto reused = t.insert(999, "reused");
    assert(reused.success && reused.probes == 11 && t.size() == 11);
    for (int key = 1; key < 11; ++key) assert(t.remove(key));
    assert(t.remove(999) && t.size() == 0);
    assert(t.search(123).probes == 11); // All slots are tombstones.
    assert(t.insert(123, "after tombstones").success);
    HashTable wrap;
    assert(wrap.insert(19, "A").success); // home 10
    assert(wrap.insert(28, "B").success); // actual 0
    assert(wrap.insert(37, "C").success); // actual 1
    assert(wrap.search(19).index == 10 && wrap.search(28).index == 0);
    assert(wrap.search(37).index == 1 && wrap.search(37).probes == 3);
    assert(wrap.remove(19) && wrap.search(37).found);
    HashTable extremes;
    for (int key : {0, -123, INT_MIN, INT_MAX}) {
        assert(extremes.insert(key, "integer").success);
        assert(extremes.search(key).found);
    }
    bool threw = false;
    try { hashFunction(123, 0); } catch (const std::invalid_argument&) { threw = true; }
    assert(threw);
    std::cout << "EDGE TESTS PASS\n";
}
int main() {
    std::cout << std::fixed << std::setprecision(4);
    const int keys[] = {555223, 555980, 555000, 555890};
    const int expected[] = {2, 2, 5, 2};
    for (int i = 0; i < 4; ++i) {
        assert(hashFunction(keys[i], 10) == expected[i]);
        std::cout << "HASH " << keys[i] << " index10=" << hashFunction(keys[i], 10) << '\n';
    }
    HashTable chain;
    assert(chain.insert(12, "Alice").success);
    assert(chain.insert(21, "Bob").success);
    assert(chain.insert(30, "Carol").success);
    std::cout << "BEFORE DELETE\n"; chain.display();
    reportSearch(chain, 12); reportSearch(chain, 30); reportSearch(chain, 102);
    assert(chain.remove(12));
    std::cout << "AFTER DELETE\n"; chain.display(); reportSearch(chain, 30);
    auto update = chain.insert(30, "Carol Updated");
    assert(update.updated && chain.size() == 2 && chain.search(30).index == 5);
    assert(chain.search(30).value == "Carol Updated");
    auto reuse = chain.insert(102, "David");
    assert(!reuse.updated && chain.search(102).index == 3 && chain.size() == 3);
    std::cout << "UPDATE probes=" << update.probes << "; REUSE probes=" << reuse.probes << '\n';
    std::cout << "LOAD EMPTY n=0 load=" << HashTable{}.loadFactor() << '\n';
    experiment("LOAD", {12,21,30,102,111,120,201,210,300,1002,1011});
    experiment("A", {1,2,3,4,5,6,7});
    experiment("B", {12,21,30,102,111,120,201});
    edgeTests();
}
```

Save the code as `hash_tables.cpp`, compile, and run:

```sh
c++ -std=c++17 -Wall -Wextra -Wpedantic hash_tables.cpp -o hash_tables
./hash_tables
```

The program uses predetermined inputs to make the demonstrations and experiments reproducible. Run this lab with assertions enabled (do not add `-DNDEBUG`), because its demonstration driver performs and checks operations inside assertions. The hash-table methods themselves do not depend on assertions.

## Part 4 — Linear Probing

Each operation examines `(home + i) % 11`, with `i` running from 0 through 10. This checks each position at most once and wraps around the table. For example, home 10 produces the sequence `10, 0, 1, ..., 9`.

Insertion updates an existing key instead of creating a duplicate. When it encounters a tombstone, it remembers that position but continues checking for the existing key. At a truly empty slot, the key cannot be farther along the valid probe chain, so insertion can use the first remembered tombstone or that empty slot. If all 11 positions have been checked, it uses a remembered tombstone or reports failure when all slots are occupied.

The `i < CAPACITY` bound prevents infinite loops, including when the table is full or every slot is a tombstone. Updating a value does not change the occupied count.

### Measurement definitions

- A **probe** is one slot examined, including a slot that is empty or deleted.
- An insertion **collision** is an encounter with an occupied slot holding a different key. An update's matching key is not a collision, and a tombstone is not counted as a collision.
- Experiment collision totals and average probes are cumulative over successful insertions from the beginning of that experiment. They are not merely counts of keys whose home positions collided.

## Part 5 — Home Position and Actual Position

Inserting keys 12, 21, and 30 creates this chain because all three digit sums are 3:

| Key | Value | Home position | Actual position |
|---|---|---:|---:|
| 12 | Alice | 3 | 3 |
| 21 | Bob | 3 | 4 |
| 30 | Carol | 3 | 5 |

The home position comes from the hash function. The actual position is determined by the first suitable position in the probe sequence after resolving collisions. The program's `display` method prints both positions, the key, value, and state.

A displaced key may require more slot accesses because a search must follow the probe sequence from its home. Relevant distance is the cyclic distance `(actual - home + capacity) % capacity`, not ordinary absolute index difference. Long clusters increase these distances and can slow searches and insertions.

## Part 6 — Searching with Linear Probing

Search stops when it finds the key, reaches a truly empty slot, or examines all 11 positions. It continues past tombstones.

Before deletion, the measured results are:

| Search type | Key | Positions examined | Found? |
|---|---:|---:|---|
| Home-position key | 12 | 1 | Yes, at 3 |
| Displaced key | 30 | 3 | Yes, at 5 |
| Missing key | 102 | 4 | No; stops at empty position 6 |

All four keys hash to home 3. Searching for 102 examines indices 3, 4, 5, and 6. Average O(1) does not mean exactly one access: a bounded expected number of accesses can still be constant. Poor distribution or a high load can invalidate that expectation.

## Part 7 — Deletion and Tombstones

The following full table displays and search results are actual program output:

```text
BEFORE DELETE
Actual | State | Key | Value | Home
0 | EMPTY | - | - | -
1 | EMPTY | - | - | -
2 | EMPTY | - | - | -
3 | OCCUPIED | 12 | Alice | 3
4 | OCCUPIED | 21 | Bob | 3
5 | OCCUPIED | 30 | Carol | 3
6 | EMPTY | - | - | -
7 | EMPTY | - | - | -
8 | EMPTY | - | - | -
9 | EMPTY | - | - | -
10 | EMPTY | - | - | -
SEARCH 12 found=1 index=3 probes=1
SEARCH 30 found=1 index=5 probes=3
SEARCH 102 found=0 index=-1 probes=4
AFTER DELETE
Actual | State | Key | Value | Home
0 | EMPTY | - | - | -
1 | EMPTY | - | - | -
2 | EMPTY | - | - | -
3 | DELETED | - | - | -
4 | OCCUPIED | 21 | Bob | 3
5 | OCCUPIED | 30 | Carol | 3
6 | EMPTY | - | - | -
7 | EMPTY | - | - | -
8 | EMPTY | - | - | -
9 | EMPTY | - | - | -
10 | EMPTY | - | - | -
SEARCH 30 found=1 index=5 probes=3
UPDATE probes=3; REUSE probes=4
```

Deleting 12 marks index 3 `DELETED`, leaving the chain searchable. The later search for 30 still examines positions 3, 4, and 5 and succeeds. Marking index 3 `EMPTY` would make the same search stop immediately and incorrectly report absence.

The demonstration then updates key 30 to `Carol Updated`. It continues past the tombstone, finds the existing entry at 5, and updates it with three probes; the occupied count remains two. Inserting new key 102 checks positions 3 through 6, confirms there is no existing copy, and reuses tombstone 3. This takes four probes, and the occupied count becomes three.

Tombstones preserve search continuity and allow future reuse, but searches cannot stop at them. Many tombstones can cause long probes even when few live records remain. Rebuilding a table can clear tombstones; resizing and rebuilding are not implemented in this fixed-capacity lab.

## Part 8 — Load Factor

`loadFactor()` computes `static_cast<double>(occupied) / 11`, avoiding integer truncation. Only `OCCUPIED` slots count toward this live load factor; tombstones do not.

Starting from an empty table, the experiment inserts these distinct keys in order:

```text
12, 21, 30, 102, 111, 120, 201, 210, 300, 1002, 1011
```

Every key has digit sum 3, deliberately creating a strong clustering stress test. Results:

| Occupied slots | Load factor | Cumulative collisions | Average insertion probes |
|---:|---:|---:|---:|
| 0 | 0.0000 | 0 | N/A (no insertions) |
| 1 | 0.0909 | 0 | 1.0000 |
| 2 | 0.1818 | 1 | 1.5000 |
| 3 | 0.2727 | 3 | 2.0000 |
| 4 | 0.3636 | 6 | 2.5000 |
| 5 | 0.4545 | 10 | 3.0000 |
| 6 | 0.5455 | 15 | 3.5000 |
| 7 | 0.6364 | 21 | 4.0000 |
| 8 | 0.7273 | 28 | 4.5000 |
| 9 | 0.8182 | 36 | 5.0000 |
| 10 | 0.9091 | 45 | 5.5000 |
| 11 | 1.0000 | 55 | 6.0000 |

The kth insertion examines k positions and encounters k - 1 occupied different keys. After n insertions, collisions total `n(n - 1)/2`, and average probes equal `(n + 1)/2`. At full capacity this gives 55 collisions and 6 average probes; the last insertion alone uses 11 probes.

An open-addressing table stores at most one live entry per slot, so live load cannot exceed 1.0. As free positions become scarce, collisions and long clusters generally become more costly, particularly for unsuccessful searches and insertions. This experiment also has deliberately poor hash distribution, so it does not isolate load factor as the only cause. Well-distributed inputs can behave much better at the same load. Tombstones additionally affect performance through the proportion of slots that are not truly empty.

## Part 9 — Hash Function Quality

Both experiments start with a fresh 11-slot table and insert seven distinct keys.

- **Dataset A:** `1, 2, 3, 4, 5, 6, 7`, whose home positions are all distinct.
- **Dataset B:** `12, 21, 30, 102, 111, 120, 201`, whose home positions are all 3.

| Measurement | Dataset A | Dataset B |
|---|---:|---:|
| Number of keys | 7 | 7 |
| Load factor | 0.6364 | 0.6364 |
| Cumulative collisions | 0 | 21 |
| Maximum insertion probes | 1 | 7 |
| Average insertion probes | 1.0000 | 4.0000 |

Dataset A performs better because each key can be inserted directly at its home. Dataset B forms one growing cluster. Its probe counts are 1 through 7, totaling 28, versus only 7 total probes for A. Equal table size and equal live load therefore do not imply equal performance.

Digit-sum hashing discards digit order and maps many structured keys to the same value. A better-distributed hash can reduce these systematic collisions, though it cannot eliminate collisions in general. Repeatedly mapping keys into the same region causes primary clustering with linear probing; even other keys hashing into that cluster can suffer long searches.

## Part 10 — Complexity Analysis

1. **Linear search on 1,000 unordered elements:** Up to 1,000 elements in the worst case, when the key is last or absent. Each failed comparison rules out only one element, giving O(N).
2. **Binary search on 1,000 sorted elements:** Approximately 10 midpoint comparisons. `log2(1000)` is about 9.97, and standard binary search uses at most `floor(log2(1000)) + 1 = 10` element probes. Each probe discards about half of the remaining candidates. This counts conceptual element comparisons, not separate equality and ordering expressions in a particular implementation.
3. **Why average constant time is possible:** A suitable hash gives direct access to a home position. With a suitable distribution, controlled load factor, and limited tombstone buildup, the expected probe count remains bounded independently of the number of records.
4. **When search degrades to O(N):** Many colliding keys, long clusters, a nearly full table, or substantial tombstone accumulation can force a scan through much of the table. A missing key in a full table can require every slot to be examined.
5. **Why “always O(1)” is incorrect:** Constant expected cost depends on assumptions, not a per-operation guarantee. The collision-chain search required three probes, while our full-table missing-key test required 11. As capacity grows, a scan can grow proportionally to capacity.

For precise notation, let M be capacity and N be the number of live entries. These methods take O(M) worst-case probes. When capacity is proportional to the number of records, this is commonly written O(N). With many tombstones and few live entries, O(M) is the more accurate bound. For the fixed lab capacity, operations are bounded by 11 probes; asymptotic claims refer to the generalized design.

Digit-sum hashing takes O(d) for a d-digit key, treated as O(1) for fixed-width integers here. Copying string values additionally depends on their lengths; the table-operation discussion concerns hashing and slot probes. General-purpose resizing would introduce occasional linear rehashing work, so expected and amortized guarantees should be distinguished.

## Part 11 — Hashing vs. Encryption

**Hashing:** A hash maps an input to a hash value, often a smaller fixed-size representation. Many possible inputs can share an output, so the output does not uniquely identify the original input. Secure cryptographic hashes are additionally designed to make finding a preimage computationally infeasible. This digit-sum function is not cryptographically one-way: matching inputs are easy to construct.

**Encryption:** Encryption transforms plaintext into ciphertext so an authorized party can recover the plaintext using the appropriate decryption key. Its intended reversibility is fundamentally different from hashing.

**Hashing is not encryption.** Hash-table indexing is an appropriate non-cryptographic hashing example. Encrypting a confidential document for later authorized decryption is an appropriate encryption example. A hash value alone does not provide a way to reconstruct the original document, and our hash table stores actual keys and values rather than encrypting them.

## Part 12 — Cryptographic and Non-Cryptographic Hashing

| Property | Explanation |
|---|---|
| Deterministic behavior | The same input, algorithm, and parameters produce the same output. |
| Pre-image resistance | Given a target hash value, finding an input that produces it should be computationally infeasible for a secure cryptographic hash. |
| Avalanche effect | A small input change should cause widespread, unpredictable-looking output changes; ideally about half the output bits change on average. |
| Collision resistance | Finding any two distinct inputs with the same hash should be computationally infeasible. This does not mean collisions do not exist. |

An ordinary in-memory hash function emphasizes fast evaluation and useful distribution for expected keys. A cryptographic hash also aims to resist deliberate attacks such as preimage and collision finding. The digit-sum function fails these security properties: rearranged digits produce identical hashes, and small digit changes have predictable effects. It is included for learning about collisions, not security. No cryptographic hash implementation is needed or supplied for this lab.

## Part 13 — Applications and Limitations

| Scenario | Generally a good hash-table choice? | Reason |
|---|---|---|
| A: Retrieve a record by student ID | Yes | An exact key lookup matches hashing's main strength: expected constant probe cost under suitable conditions. |
| B: IDs between 500000 and 600000 | Usually no | Hashing does not preserve order. A plain table scans all slots; an ordered tree or sorted array can locate a range efficiently and then enumerate its matches. |
| C: Display all keys in ascending order | Usually no | A hash table's storage order is not key order. Extracting and sorting records adds work; an ordered structure supports sorted traversal directly. |
| D: Quickly find the minimum key | Usually no | Without additional maintained metadata, a plain hash table must scan the slots. A suitable ordered tree or min-heap better matches repeated minimum queries. |
| E: Retrieve a profile by username | Yes | Username lookup is an exact-match operation. A string-key hash table is appropriate, although this lab implementation specifically uses integer keys. Hashing and equality also depend on username length. |

## Analysis and Reflection

A well-distributed hash can avoid the N-element scan of linear search and the repeated halving of binary search for exact-match queries. Dataset A needed only one insertion probe per record, demonstrating the potential benefit. However, finite indices inevitably allow collisions, and this particular hash creates avoidable ones by losing digit-order information.

Linear probing resolves collisions by moving forward cyclically until a suitable slot or matching key is found. Dataset B's seven insertions required 28 probes rather than Dataset A's seven, despite equal loads. This connects hash quality and clustering directly to observed cost. The stress test grew to 11 probes for its last insertion as the table filled.

Deletion must preserve the probe-chain invariant. Marking the first chain slot DELETED allowed the later key to remain searchable. Tombstones can be reused, but an insertion must first rule out an existing copy farther along the chain. The update-and-reuse demonstration confirmed both behaviors. Accumulated tombstones can prolong searches even when live load is low, making cleanup useful in a larger implementation.

Expected O(1) operations rely on suitable distribution and occupancy; worst-case probing can inspect every slot. Hash tables are therefore a strong match for exact lookup but not generally for range queries, sorted traversal, or minimum-key retrieval without additional structure.

## Verification

The embedded C++ source was compiled with C++17, warnings enabled, AddressSanitizer, and UndefinedBehaviorSanitizer. The program exited with status 0, printed `EDGE TESTS PASS`, and produced the recorded experiment results with no sanitizer diagnostics.

Assertions checked manual hashes, searching through tombstones, updating without duplicates, tombstone reuse, full-table insertion failure, updating an existing key while full, full-table missing searches, an all-tombstone table, wraparound from index 10 to indices 0 and 1, missing-key deletion, zero and negative keys, integer extrema, and invalid table-size rejection.

The tables report actual probe counts rather than runtime benchmarks. They illustrate the specified datasets and do not establish that digit-sum hashing is well distributed on general inputs.

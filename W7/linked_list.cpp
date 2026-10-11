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

// Helper for printing bool results
string yesNo(bool b) {
    return b ? "true" : "false";
}

// Part 10: Test the complete linked list
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

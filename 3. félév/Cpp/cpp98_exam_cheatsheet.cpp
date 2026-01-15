// ============================================================================
/* C++98 EXAM CHEAT SHEET (single .cpp file you can print/bring)
   Goal: help you solve "views/adapters + STL" style tasks to at least ~grade 3.
   Standard: C++98 (NO auto, NO lambdas, NO range-for, NO nullptr).
*/
// ============================================================================
//
// QUICK NAVIGATION (search these tags):
//   [00] C++98 rules / compilation
//   [01] References vs Pointers (MOST IMPORTANT)
//   [02] const correctness (what/where/why)
//   [03] Templates + typename + dependent names
//   [04] Functors (function objects) + comparators/predicates
//   [05] Iterators (what they are, invalidation basics)
//   [06] std::vector / std::deque / std::list (operations you used)
//   [07] std::set / std::map (insert/find/erase, pair<it,bool>)
//   [08] std::stack / std::queue (adapters)
//   [09] list::splice / erase return value
//   [10] "View" patterns: hold pointer/ref to external container
//   [11] operator overloading patterns (+=, [], conversion)
//   [12] Circular buffer (ring buffer) patterns
//   [13] Bit-packing bool into int[] (specialization idea)
//   [14] Binary tree view over vector (index formulas)
//   [15] Association layer with custom "equality" functor (not ordering)
//   [16] Debug checklist (why it doesn’t compile / typical STL errors)
//
// ============================================================================
// [00] C++98 rules / compilation
// ============================================================================
// Compile with C++98:
//   g++ -std=c++98 -Wall -Wextra -pedantic yourfile.cpp
//
// Things NOT in C++98:
//   - auto type deduction
//   - lambdas: [](int x){...}
//   - range-based for
//   - nullptr, enum class, override, etc.
//
// ============================================================================
// [01] References vs Pointers (MOST IMPORTANT)
// ============================================================================
//
// Reference: alias to an existing object. Must be initialized, cannot be null.
// Pointer: holds an address. Can be null, can be reseated.
//
// PASS BY VALUE:
//   void f(T x)         // copies x
//
// PASS BY REFERENCE:
//   void f(T& x)        // modifies caller’s object
//   void f(const T& x)  // no copy, cannot modify x (best for big objects)
//
// PASS BY POINTER:
//   void f(T* p)        // p can be 0; modify *p if not null
//
// When to use what:
//   - Want to guarantee object exists? Use reference (T& / const T&).
//   - Need "optional" / can be absent? Use pointer (T*).
//   - View/adaptor storing external container: store pointer (ContT*).
//
// Example: storing the original container (VIEW PATTERN)
//   struct view {
//     std::vector<int>* pv;
//     explicit view(std::vector<int>& v): pv(&v) {}
//   };
//
// NOTE: constructor param is a reference, but we store its ADDRESS (pointer).
// That’s normal: reference is only in the parameter list; member wants persistence.
//
// ============================================================================
// [02] const correctness
// ============================================================================
// const object: cannot modify its state.
// const method: promises not to modify *this.
// const reference: avoids copy and prevents modification.
//
// Typical pattern:
//   T& at(int i);             // writable
//   const T& at(int i) const; // readable on const objects
//
// Returning T& vs T:
//   - Returning T copies.
//   - Returning T& returns the real element so assignment modifies container.
//
// ============================================================================
// [03] Templates + typename + dependent names
// ============================================================================
// In templates, names depending on template parameters need 'typename' in C++98.
// Example:
//   template<class Cont>
//   void f(Cont& c) {
//     typename Cont::iterator it = c.begin();
//   }
//
// ============================================================================
// [04] Functors + comparators/predicates
// ============================================================================
// Functor = object with operator() so you can call it like a function.
// Used by STL for sorting, comparison, predicates.
//
// Comparator (ordering):
//   struct greater_int { bool operator()(int a,int b) const { return a>b; } };
// Predicate:
//   struct is_even { bool operator()(int x) const { return x%2==0; } };
//
// KEY IDEA:
//   - std::map/std::set need ORDERING comparator (strict weak ordering).
//   - Some tasks use "equality-like" functor (same_length). That is NOT ordering.
//     -> don’t use std::map with that; use linear search table.
//
// ============================================================================
// [05] Iterators + invalidation (rules of thumb)
// ============================================================================
// iterator acts like a pointer: *it, ++it
//
// invalidation quick:
//   vector: insert/erase can invalidate many iterators (reallocation)
//   deque: some invalidation on insert/erase
//   list: stable except erased element
//   set/map: stable except erased element
//
// ============================================================================
// [06] vector / deque / list essentials
// ============================================================================
// vector: contiguous, random access O(1), resize(idx+1,T()) to ensure index
// deque: random access O(1), not contiguous
// list: linked list, no random access, insert/erase/splice O(1) with iterators
//
// ============================================================================
// [07] set / map essentials
// ============================================================================
// set.insert(x) -> pair<iterator,bool>
//   bool inserted = s.insert(x).second;
//
// map.find(k) returns iterator (or end())
//   it->second is the mapped value
// map[k] inserts default V() if missing and returns V&
//
// ============================================================================
// [08] stack / queue adapters
// ============================================================================
// stack.pop() returns void (don’t return it)
// stack.top() returns T&
// queue.front()/back()
//
// ============================================================================
// [09] list::erase + splice
// ============================================================================
// it = lst.erase(it);           // returns next iterator
// lst.splice(pos, lst, it);     // move node at it to before pos (O(1), no copy)
//
// ============================================================================
// [10] View patterns (RAII restore)
// ============================================================================
// Store pointer to external container, backup initial state, restore in destructor.
// Prevent copying (private copy ctor/assign).
//
// ============================================================================
// [11] Operators
// ============================================================================
// += returns *this
// [] returns reference (except set specialization where cannot modify element)
//
// ============================================================================
// [12] Circular buffer (ring) pattern
// ============================================================================
// head, tail, count, cap
// insert: if(count==cap) reject; else buf[tail]=x; tail=(tail+1)%cap; ++count
// read:   if(count==0) reject; else out=buf[head]; head=(head+1)%cap; --count
//
// ============================================================================
// [13] Bit-pack bool into int[]
// ============================================================================
// word = bit/BITS; bit = bit%BITS; mask = 1u<<bit
// set: OR mask; clear: AND ~mask; get: (word & mask)!=0
//
// ============================================================================
// [14] Binary tree view over vector
// ============================================================================
// root index 0
// left  = 2*i+1
// right = 2*i+2
// insert child => ensure vector big enough (resize) and assign
// height often expected as floor(log2(size)) in edges
// inorder sort trick: sort values then seen-by-inorder write-back
//
// ============================================================================
// [15] Association layer with custom equality functor
// ============================================================================
// For KeyEq (same_length) you must NOT use std::map ordering.
// Store tags in vector< (key, iterator) > and linear search using eq(a,b).
//
// ============================================================================
// [16] Debug checklist
// ============================================================================
// - Using auto/lambda? not C++98.
// - map/set key not orderable (std::complex)? need comparator OR avoid map.
// - view: did you copy container instead of storing pointer/ref?
// - stack.pop() returns void.
// - map.find returns iterator; check end() before deref.
// - templates: missing typename.
//
// ============================================================================
// Tiny snippets you can copy during exam
// ============================================================================

#include <iostream>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <stack>
#include <algorithm>
#include <functional>
#include <stdexcept>

static void demo_set_insert() {
    std::set<int> s;
    bool inserted = s.insert(5).second;
    std::cout << inserted << "\n";
}

static void demo_list_splice() {
    std::list<int> l;
    l.push_back(1); l.push_back(2); l.push_back(3);
    std::list<int>::iterator it = l.begin(); ++it; // 2
    l.splice(l.begin(), l, it); // move 2 to front
}

template<class T>
static void ensure_index(std::vector<T>& v, std::size_t idx) {
    if (v.size() <= idx) v.resize(idx + 1, T());
}

int main() {
    // Uncomment to sanity-check compilation:
    // demo_set_insert();
    // demo_list_splice();
    return 0;
}






// Reference vs Pointer (side-by-side)
void inc_ref(int& x) {
    x++;
}

void inc_ptr(int* x) {
    if (x) (*x)++;
}

void demo_ref_ptr() {
    int a = 5;
    inc_ref(a);      // a == 6

    inc_ptr(&a);     // a == 7
}




// Why views store POINTERS, not references
struct view {
    std::vector<int>* pv;

    explicit view(std::vector<int>& v)
        : pv(&v) {}
};

void demo_view() {
    std::vector<int> v(3);
    view vw(v);
    (*vw.pv)[0] = 42;   // modifies original vector
}





// Iterator basics (manual loop, C++98)
void demo_iterators() {
    std::list<int> l;
    l.push_back(1);
    l.push_back(2);

    std::list<int>::iterator it = l.begin();
    for (; it != l.end(); ++it) {
        *it *= 2;
    }
}




// list::erase returns the NEXT iterator
void demo_erase() {
    std::list<int> l;
    l.push_back(1);
    l.push_back(2);
    l.push_back(3);

    std::list<int>::iterator it = l.begin();
    ++it;               // points to 2
    it = l.erase(it);   // removes 2, it now points to 3
}



// list::splice (move element without copying)
void demo_splice() {
    std::list<int> l;
    l.push_back(1);
    l.push_back(2);
    l.push_back(3);

    std::list<int>::iterator it = l.begin();
    ++it; // 2

    l.splice(l.begin(), l, it); // move 2 to front
    // list is now: 2 1 3
}



// std::set::insert → how to know if it succeeded
void demo_set_insert() {
    std::set<int> s;

    bool ok1 = s.insert(5).second; // true
    bool ok2 = s.insert(5).second; // false
}








// std::map::find vs operator[]
void demo_map() {
    std::map<int, int> m;

    m[3] = 7;            // inserts key 3
    int x = m[3];        // OK

    std::map<int,int>::iterator it = m.find(4);
    if (it == m.end()) {
        // key not present
    }
}









// Functor (custom comparison / equality)
struct same_parity {
    bool operator()(int a, int b) const {
        return (a % 2) == (b % 2);
    }
};

// used like
same_parity eq;
bool r = eq(3, 7); // true







// Linear “association” table (no map!)
template<class Key, class Iter>
struct tag {
    Key key;
    Iter it;
};

template<class Key, class Iter, class Eq>
Iter find_tag(std::vector< tag<Key,Iter> >& tags,
              const Key& k,
              Eq eq)
{
    for (std::size_t i = 0; i < tags.size(); ++i)
        if (eq(tags[i].key, k))
            return tags[i].it;
    return Iter();
}









// Circular buffer core logic (insert + read)
template<class T>
struct ring {
    T* buf;
    int cap, head, tail, cnt;

    ring(T* b, int c)
        : buf(b), cap(c), head(0), tail(0), cnt(0) {}

    bool insert(const T& x) {
        if (cnt == cap) return false;
        buf[tail] = x;
        tail = (tail + 1) % cap;
        ++cnt;
        return true;
    }

    bool read(T& out) {
        if (cnt == 0) return false;
        out = buf[head];
        head = (head + 1) % cap;
        --cnt;
        return true;
    }
};






// Binary tree view index math (memorize)
int left(int i)  { return 2*i + 1; }
int right(int i) { return 2*i + 2; }
// root = 0
// root.left()        -> index 1
// root.right()       -> index 2
// root.right().left  -> index 5






// Ensure vector size before writing
template<class T>
void ensure(std::vector<T>& v, std::size_t idx) {
    if (v.size() <= idx)
        v.resize(idx + 1, T());
}







// stack adapter reminder (common trap)
std::stack<int> s;
s.push(3);
int x = s.top();
s.pop();     // returns VOID






// RAII restore pattern (exam gold)
struct restore_vector {
    std::vector<int>* pv;
    std::vector<int> backup;

    explicit restore_vector(std::vector<int>& v)
        : pv(&v), backup(v) {
        v.clear();
    }

    ~restore_vector() {
        *pv = backup;
    }
};
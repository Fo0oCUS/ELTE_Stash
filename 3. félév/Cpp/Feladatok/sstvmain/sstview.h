#ifndef SET_STACK_VIEW
#define SET_STACK_VIEW

#include <set>
#include <stack>
#include <functional> // std::less
#include <vector>     // a konverzióhoz segédtároló

template <typename T, class Compare = std::less<T> >
class set_stack_view {
public:
    typedef std::set<T, Compare> set_type;
    typedef std::stack<T>        stack_type;

private:
    set_type*  pset;     // az eredeti set (nem másolat!)
    stack_type st;       // a stack teteje (utolsó sikeres push)
    set_type   backup;   // eredeti állapot mentése

public:
    explicit set_stack_view(set_type& s)
        : pset(&s), st(), backup(s)
    {
        pset->clear(); // a feladat szerint üressé válik
    }

    void push(const T& item) {
        if (pset->insert(item).second) {
            st.push(item);
        }
    }

    // 5-ös: intervallum push (tetszőleges iterátor-tartomány)
    template <class It>
    void push(It first, It last) {
        for (; first != last; ++first) {
            push(*first);
        }
    }

    T& top() { return st.top(); }
    const T& top() const { return st.top(); }

    void pop() {
        T x = st.top();
        st.pop();
        pset->erase(x);
    }

    set_stack_view& operator+=(const set_type& other) {
        typename set_type::const_iterator it = other.begin();
        typename set_type::const_iterator e  = other.end();
        for (; it != e; ++it) {
            push(*it);
        }
        return *this;
    }

    // 4-es: más comparatorú setet is tudjunk “rápusholni”
    template <class OtherCompare>
    set_stack_view& operator+=(const std::set<T, OtherCompare>& other) {
        typename std::set<T, OtherCompare>::const_iterator it = other.begin();
        typename std::set<T, OtherCompare>::const_iterator e  = other.end();
        for (; it != e; ++it) {
            push(*it);
        }
        return *this;
    }

    // 5-ös: std::stack konverzió tetszőleges mögöttes konténerrel
    template <class Container>
    operator std::stack<T, Container>() const {
        std::stack<T, Container> result;

        // Nem tudjuk iterálni a stacket, ezért másolunk és pop-olunk.
        stack_type tmp = st;
        std::vector<T> elems;
        while (!tmp.empty()) {
            elems.push_back(tmp.top()); // top-to-bottom sorrendben gyűjtjük
            tmp.pop();
        }

        // visszaépítjük úgy, hogy a result teteje ugyanaz legyen, mint az eredeti st teteje
        for (int i = (int)elems.size() - 1; i >= 0; --i) {
            result.push(elems[i]);
        }

        return result;
    }

    ~set_stack_view() {
        // visszaállítás az eredeti állapotra
        pset->clear();
        typename set_type::const_iterator it = backup.begin();
        typename set_type::const_iterator e  = backup.end();
        for (; it != e; ++it) {
            pset->insert(*it);
        }
    }

private:
    set_stack_view(const set_stack_view&);
    set_stack_view& operator=(const set_stack_view&);
};

#endif

#ifndef VECTOR_VIEW_H
#define VECTOR_VIEW_H

#include "arrayview.h"
#include <algorithm> // std::sort

template <typename T, typename ConT = std::vector< typename std::list<T>::iterator>>
class vector_view : public array_view<T, ConT>{

    template <class Comp>
    struct iter_comp {
        Comp comp_;
        iter_comp(Comp c) : comp_(c) {}

        bool operator()(typename std::list<T>::iterator a,
                        typename std::list<T>::iterator b) const {
            return comp_(*a, *b);
        }
    };

public:
    vector_view(std::list<T>& _list) : array_view<T, ConT>(_list) {}

    void push_back(const T& value) {
        // eredeti listához is hozzáadjuk
        this->list->push_back(value);

        // iterátor az új elemre (end()-- C++98)
        typename std::list<T>::iterator it = this->list->end();
        --it;

        // nézet indexének frissítése
        this->container.push_back(it);
    }

    // opcionális: default növekvő sorrend
    void sort() {
        sort(std::less<T>());
    }

    // EZ kell a példához: avv.sort(std::greater<int>())
    template <class Comp>
    void sort(Comp comp) {
        std::sort(this->container.begin(),
                  this->container.end(),
                  iter_comp<Comp>(comp));
    }
};


#endif
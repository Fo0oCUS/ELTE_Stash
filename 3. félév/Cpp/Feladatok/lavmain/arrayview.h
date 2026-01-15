#ifndef ARRAY_VIEW_H
#define ARRAY_VIEW_H

#include <list>
#include <vector>

template <typename T, typename ConT = std::vector< typename std::list<T>::iterator>>
class array_view{
    
protected:
    std::list<T>* list;
    ConT container;

public:

    array_view(std::list<T>& _list) : list(&_list), container(){
        typename std::list<T>::iterator x = _list.begin();
        typename std::list<T>::iterator y = _list.end();
        while(x != y){
            container.push_back(x);
            x++;
        }
    }

    T& at(int i)  { return *container[i]; }
    const T& at(int i) const { return *container[i]; }

    T& operator[](int i) { return *container[i]; }
    const T& operator[](int i) const { return *container[i]; }
};

#endif
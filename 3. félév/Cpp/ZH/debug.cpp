#include "selforgl.h"
#include <list>
#include <iostream>



template<typename T>
void print_l(std::list<T>* l){
    typedef typename std::list<T>::iterator iterator;
    iterator begin = l->begin();
    iterator end = l->end();

    while(begin != end){
        std::cout << *begin << ", ";
        begin++;
    }
    std::cout << std::endl;
};

void assert(bool& success, bool arg){
    success = success && arg;
};

struct isOne{
    bool operator()(const int& i){ return i == 1; }
};


int main(){
    std::list<int> pl;
    pl.push_back(1);
    pl.push_back(2);
    pl.push_back(3);
    print_l(&pl);
    self_organizing_list<int> p(pl);
    bool success = true;

    assert(success, p.contains(1));
    assert(success, !p.contains(100));

    move_to_back<int> k(pl);

    k.contains(1);
    assert(success, pl.back() == 1);

    k.contains(1);
    assert(success, pl.back() == 1);

    assert(success, k.contains_if(isOne()));

    if(success){std::cout << "Minden teszt sikeres!" << std::endl;}
    else{std::cout << "Hibas teszt!" << std::endl;}


};

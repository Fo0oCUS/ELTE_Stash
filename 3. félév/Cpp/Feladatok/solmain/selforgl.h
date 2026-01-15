#ifndef SELF_ORGANIZING_LIST_H
#define SELF_ORGANIZING_LIST_H

#include <iostream>
#include <list>
#include <map>

template<typename T>
class self_organizing_list{
    typedef std::list<T> list_type;
    typedef typename std::list<T>::iterator iterator;

protected:
    list_type* list;

public:
    self_organizing_list(list_type& _list) : list(&_list){

    }

    virtual bool contains(const T& item){
        iterator begin = list->begin();
        iterator end = list->end();
        while(begin != end){
            if(item == *begin) { return true; }
            begin++;
        }
        return false;
    }

    virtual ~self_organizing_list() {} // kell, mert delete base* történik
};


template<typename T>
class move_to_front : public self_organizing_list<T>{
    typedef std::list<T> list_type;
    typedef typename std::list<T>::iterator iterator;

public:
    move_to_front(list_type& _list) : self_organizing_list<T>(_list){}

    virtual bool contains(const T& item){
        iterator begin = this->list->begin();
        iterator end = this->list->end();

        while(begin != end){
            if(*begin == item){
                if (begin != this->list->begin()) {
                    this->list->splice(this->list->begin(), *(this->list), begin);
                }
                return true;      
            }
            begin++;
        }
        return false;
    }

    template<class Pred>
    bool contains_if(Pred pred){
        iterator begin = this->list->begin();
        iterator end = this->list->end();

        while(begin != end){
            if(pred(*begin)){
                if (begin != this->list->begin()) {
                    this->list->splice(this->list->begin(), *(this->list), begin);
                }
                return true;      
            }
            begin++;
        }
        return false;
    }

};


template<typename T>
class frequency_list : public self_organizing_list<T>{
    typedef std::list<T> list_type;
    typedef typename std::list<T>::iterator iterator;

    std::map<T, int> map;
public:
    frequency_list(list_type& _list) : self_organizing_list<T>(_list), map(){}

    virtual bool contains(const T& item){
        iterator begin = this->list->begin();
        iterator end   = this->list->end();

        while(begin != end){
            if(*begin == item){
                int value = ++map[item];   // frekvencia növelése

                // Keressük meg azt a pozíciót (it), ami elé be kellSÚRJUK (splice)
                iterator it = begin;
                while(it != this->list->begin()){
                    iterator prev = it;
                    --prev;

                    // prev frekvenciája (ha nincs benne a map-ben, akkor 0)
                    int prev_value = 0;
                    typename std::map<T,int>::iterator mp = map.find(*prev);
                    if(mp != map.end()) prev_value = mp->second;

                    // Ha az előző elem frekije >= a miénk, akkor itt a helyünk
                    if(prev_value >= value){
                        break;
                    }

                    it = prev; // még előrébb kell mennünk
                }

                // Ha nem ott van már, akkor egyetlen splice-szal átrakjuk it elé
                if(it != begin){
                    this->list->splice(it, *(this->list), begin);
                }

                return true;
            }
            ++begin;
        }
        return false;
    }

    template <class Pred>
    bool contains_if(Pred pred) {
        iterator begin = this->list->begin();
        iterator end   = this->list->end();

        while (begin != end) {
            if (pred(*begin)) {

                int value = ++map[*begin];

                iterator it = begin;
                while (it != this->list->begin()) {
                    iterator prev = it;
                    --prev;

                    int prev_value = 0;
                    typename std::map<T,int>::iterator mp = map.find(*prev);
                    if (mp != map.end()) prev_value = mp->second;

                    if (prev_value >= value) {
                        break;
                    }

                    it = prev;
                }

                if (it != begin) {
                    this->list->splice(it, *(this->list), begin);
                }

                return true;
            }
            ++begin;
        }
        return false;
    }

};

#endif
#ifndef SELF_ORGANIZING_LIST
#define SELF_ORGANIZING_LIST

#include <list>

template<typename T>
class self_organizing_list{
    typedef std::list<T> list_type;
    typedef typename std::list<T>::iterator iterator;

protected:
    list_type* list;

public:
    self_organizing_list(list_type& _list) : list(&_list){}

    virtual bool contains(const T& item){
        iterator begin = list->begin();
        iterator end = list->end();
        while(begin != end){
            if(item == *begin) { return true; }
            begin++;
        }
        return false;
    }

    virtual ~self_organizing_list() {} 
};


template<typename T>
class move_to_back : public self_organizing_list<T>{
    typedef typename std::list<T>::iterator iterator;

public:
    move_to_back(std::list<T>& _list) : self_organizing_list<T>(_list){}

    virtual bool contains(const T& item){
        iterator begin = this->list->begin();
        iterator end = this->list->end();

        while(begin != end){
            if(*begin == item){
                if (begin != this->list->end()) {
                    this->list->splice(this->list->end(), *(this->list), begin);
                }
                return true;      
            }
            begin++;
        }
        return false;
    }

    template <class Pred>
    bool contains_if(Pred pred){
        bool found;

        iterator begin = this->list->begin();
        iterator end = this->list->end();

        while(begin != end){
            if(pred(*begin)){
                if (begin != this->list->end()) {
                    this->list->splice(this->list->end(), *(this->list), begin);
                }
                found = true;;      
            }
            begin++;
        }

        return found;
    }

};


template<typename T>
class transpose : public self_organizing_list<T>{
    typedef typename std::list<T>::iterator iterator;

public:
    transpose(std::list<T>& _list) : self_organizing_list<T>(_list){}

    virtual bool contains(const T& item){
        iterator begin = this->list->begin();
        iterator end = this->list->end();

        while(begin != end){
            if(*begin == item){
                if (begin != this->list->begin()) {
                    iterator prev = begin;
                    prev--;
                    this->list->splice(prev, *(this->list), begin);
                }         
                return true;      
            }
            begin++;
        }
        return false;
    }

    template <class Pred>
    bool contains_if(Pred pred){
        bool found;

        iterator begin = this->list->begin();
        iterator end = this->list->end();

        while(begin != end){
            if(pred(*begin)){
                if (begin != this->list->begin()) {
                    iterator prev = begin;
                    prev--;
                    this->list->splice(prev, *(this->list), begin);
                }
                found = true;      
            }
            begin++;
        }
        return found;
    }
};




#endif
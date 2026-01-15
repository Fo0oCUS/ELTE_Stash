#ifndef ASSOC_H
#define ASSOC_H

#include <vector>
#include <set>
#include <functional> // std::equal_to
#include <stdexcept>  // std::out_of_range

// ------------------------------------------------------------
// association_common: közös alap (teszt ezt a nevet várja)
// ------------------------------------------------------------
template <typename KeyT,
          typename ContT,
          typename T = typename ContT::value_type,
          typename KeyEq = std::equal_to<KeyT> >
class association_common {
public:
    typedef ContT container_type;
    typedef T     value_type;
    typedef KeyT  key_type;
    typedef KeyEq key_eq_type;

    typedef typename ContT::iterator iterator;

protected:
    struct entry {
        KeyT key;
        iterator it;
        entry(const KeyT& k, iterator i) : key(k), it(i) {}
    };

    ContT* cont;
    std::vector<entry> tags;
    KeyEq eq;

    // megkeresi az első "egyező" kulcsot a predikátummal
    int index_of(const KeyT& key) const {
        for (int i = 0; i < (int)tags.size(); ++i) {
            if (eq(tags[i].key, key)) return i;
        }
        return -1;
    }

public:
    explicit association_common(ContT& c, const KeyEq& keq = KeyEq())
        : cont(&c), tags(), eq(keq) {}

    virtual ~association_common() {} // 5-ösnél “common” pointer is van a tesztben

    bool has(const KeyT& key) const {
        return index_of(key) != -1;
    }
};

// ------------------------------------------------------------
// Általános association: szekvenciális konténerekhez
// (list/vector/deque) - operator[] módosítható T&
// ------------------------------------------------------------
template <typename KeyT,
          typename ContT,
          typename T = typename ContT::value_type,
          typename KeyEq = std::equal_to<KeyT> >
class association : public association_common<KeyT, ContT, T, KeyEq> {
    typedef association_common<KeyT, ContT, T, KeyEq> base;

public:
    typedef typename base::iterator iterator;

    explicit association(ContT& c, const KeyEq& keq = KeyEq())
        : base(c, keq) {}

    void associate(const KeyT& key, iterator it) {
        int idx = this->index_of(key);
        if (idx == -1) {
            this->tags.push_back(typename base::entry(key, it));
        } else {
            this->tags[idx].it = it;
        }
    }

    T& find(const KeyT& key) {
        int idx = this->index_of(key);
        if (idx == -1) throw std::out_of_range("association::find");
        return *(this->tags[idx].it);
    }

    const T& find(const KeyT& key) const {
        int idx = this->index_of(key);
        if (idx == -1) throw std::out_of_range("association::find const");
        return *(this->tags[idx].it);
    }

    // map-szerű: ha még nincs tag, új elemet vesz fel a konténerbe (a végére)
    // és visszaadja T&-ként, hogy átírható legyen.
    T& operator[](const KeyT& key) {
        int idx = this->index_of(key);
        if (idx != -1) {
            return *(this->tags[idx].it);
        }

        // új elem beszúrása a konténer végére
        iterator it = this->cont->insert(this->cont->end(), T());
        this->tags.push_back(typename base::entry(key, it));
        return *it;
    }
};

// ------------------------------------------------------------
// 5-ös: std::set specializáció (tetszőleges rendezéssel)
// operator[] nem adhat módosítható referenciát -> const T&
// ------------------------------------------------------------
template <typename KeyT,
          typename V,
          typename SetComp,
          typename Alloc,
          typename KeyEq>
class association<KeyT, std::set<V, SetComp, Alloc>, V, KeyEq>
    : public association_common<KeyT, std::set<V, SetComp, Alloc>, V, KeyEq>
{
    typedef std::set<V, SetComp, Alloc> set_type;
    typedef association_common<KeyT, set_type, V, KeyEq> base;

public:
    typedef typename set_type::iterator iterator;

    explicit association(set_type& s, const KeyEq& keq = KeyEq())
        : base(s, keq) {}

    void associate(const KeyT& key, iterator it) {
        int idx = this->index_of(key);
        if (idx == -1) {
            this->tags.push_back(typename base::entry(key, it));
        } else {
            this->tags[idx].it = it;
        }
    }

    const V& find(const KeyT& key) const {
        int idx = this->index_of(key);
        if (idx == -1) throw std::out_of_range("association<set>::find const");
        return *(this->tags[idx].it);
    }

    // NINCS nem-const find(), set elemét nem lehet módosítani

    // operator[]: const refet ad vissza, így nem lehet utólag átírni
    const V& operator[](const KeyT& key) {
        int idx = this->index_of(key);
        if (idx != -1) {
            return *(this->tags[idx].it);
        }

        // új elem felvétele a set-be (default V())
        std::pair<iterator, bool> ins = this->cont->insert(V());
        iterator it = ins.first;

        this->tags.push_back(typename base::entry(key, it));
        return *it;
    }
};

#endif

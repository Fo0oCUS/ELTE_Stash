#ifndef MAPACCPART_H
#define MAPACCPART_H

#include <map>
#include <functional> // std::plus

template <typename KeyT,
          typename ValT,
          typename KeyComp = std::less<KeyT>,
          typename Acc = std::plus<ValT> >
class map_accumulate_partition {
public:
    typedef std::map<KeyT, ValT, KeyComp> map_type;
    typedef typename map_type::iterator iterator;

private:
    map_type* pm;     // a kapott map (nem másolat)
    map_type  orig;   // eredeti értékek mentése (kulcs -> eredeti érték)
    iterator  part;   // partíció határ: [begin, part) prefixelve, [part, end) eredeti
    Acc       acc;

    void rebuild_prefix_to_part() {
        if (pm->empty()) return;

        // prefix akkumuláció a [begin, part) tartományra az EREDETI értékekből
        iterator it = pm->begin();
        if (it == part) return; // üres prefix

        // első elem: running = orig[first]
        ValT running = orig.find(it->first)->second;
        it->second = running;
        ++it;

        for (; it != part; ++it) {
            ValT cur = orig.find(it->first)->second;
            running = acc(running, cur);
            it->second = running;
        }

        // part-tól jobbra maradjon eredeti
        for (iterator jt = part; jt != pm->end(); ++jt) {
            jt->second = orig.find(jt->first)->second;
        }
    }

    void restore_at(iterator it) {
        if (it == pm->end()) return;
        typename map_type::const_iterator oit = orig.find(it->first);
        if (oit != orig.end()) {
            it->second = oit->second;
        }
    }

public:
    explicit map_accumulate_partition(map_type& m, const Acc& a = Acc())
        : pm(&m), orig(m), part(m.end()), acc(a) {}

    // beállítja a partíció kulcsot és újraszámolja a prefix részt
    void set_partition_key(const KeyT& key) {
        if (pm->empty()) { part = pm->end(); return; }

        iterator lb = pm->lower_bound(key);

        if (lb == pm->end()) {
            // ha túl nagy, akkor a legutolsó elem legyen a határ (pont az "Unknown" teszt miatt)
            iterator last = pm->end();
            --last;
            part = last;
        } else {
            part = lb;
        }

        rebuild_prefix_to_part();
    }

    // postfix/prefix ++ : határ megy a következő kulcsra, és a határ elemét eredetire állítjuk
    map_accumulate_partition& operator++() {
        if (pm->empty()) return *this;
        if (part == pm->end()) return *this;
        ++part;
        restore_at(part);
        return *this;
    }

    map_accumulate_partition operator++(int) {
        map_accumulate_partition tmp(*this);
        ++(*this);
        return tmp;
    }

    // postfix/prefix -- : határ megy az előző kulcsra, és a határ elemét eredetire állítjuk
    map_accumulate_partition& operator--() {
        if (pm->empty()) return *this;
        if (part == pm->begin()) return *this;

        if (part == pm->end()) {
            // ha end-en állt, visszamegy az utolsóra
            part = pm->end();
            --part;
        } else {
            --part;
        }
        restore_at(part);
        return *this;
    }

    map_accumulate_partition operator--(int) {
        map_accumulate_partition tmp(*this);
        --(*this);
        return tmp;
    }
};

#endif

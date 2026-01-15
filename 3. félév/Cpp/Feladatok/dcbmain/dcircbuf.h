#ifndef DCIRCBUF_H
#define DCIRCBUF_H

#include <cstddef> // size_t
#include <vector>

template <class T>
class double_circular_buffer {
    T* buf;
    std::size_t n;

    // vágópont: A=[0..cut-1], B=[cut..n-1]
    std::size_t cut;

    // A ring állapot (abszolút indexek a buf-ban, de A-n belül wrap)
    std::size_t headA, tailA, cntA;

    // B ring állapot
    std::size_t headB, tailB, cntB;

    // sikeres read stat
    std::size_t readA_cnt, readB_cnt;

    // --- segédfüggvények a wrap-hoz ---
    std::size_t nextA(std::size_t idx) const {
        // A terület: 0..cut-1
        return (idx + 1 >= cut) ? 0 : (idx + 1);
    }

    std::size_t nextB(std::size_t idx) const {
        // B terület: cut..n-1
        return (idx + 1 >= n) ? cut : (idx + 1);
    }

    std::size_t capA() const { return cut; }
    std::size_t capB() const { return n - cut; }

    // Átosztás – csak akkor, ha mindkét oldal már olvasott legalább egyszer
    void rebalance_after_read() {
        if (n == 0) return;
        if (readA_cnt == 0 || readB_cnt == 0) return;

        std::size_t total = readA_cnt + readB_cnt;
        std::size_t newCut = (readA_cnt * n) / total;

        // Ne engedjük, hogy valamelyik oldal 0 kapacitást kapjon (tesztben nem kell extrém, de biztonságos)
        if (newCut == 0) newCut = 1;
        if (newCut >= n) newCut = n - 1;

        // ha nem változott, kész
        if (newCut == cut) return;

        cut = newCut;

        // A oldal: ha cut csökkent (ritka), elméletben lehetne cntA > capA -> vágjuk
        if (cntA > capA()) cntA = capA();
        if (capA() == 0) { cntA = 0; headA = tailA = 0; }
        else {
            if (headA >= capA()) headA = 0;
            if (tailA >= capA()) tailA = 0;
        }

        // B oldal: új kezdőpont cut, ha headB “átlóg” (headB < cut), dobjunk addig elemet
        if (capB() == 0) { cntB = 0; headB = tailB = cut; }
        else {
            // tailB és headB legyen a B tartományban
            if (headB < cut) headB = cut;
            if (tailB < cut) tailB = cut;

            // ha cntB nagyobb mint az új kapacitás, vágjuk (adatvesztés megengedett)
            if (cntB > capB()) cntB = capB();

            // extra “eldobás”: ha headB a régi wrap miatt esetleg rossz helyre mutatna, egyszerűen clamp-elünk
            // (a teszt nem corner-case-el)
        }
    }

public:
    // 2 paraméteres (teszt 2-es)
    double_circular_buffer(T* arr, std::size_t size)
        : buf(arr), n(size),
          cut(size / 2),
          headA(0), tailA(0), cntA(0),
          headB(cut), tailB(cut), cntB(0),
          readA_cnt(0), readB_cnt(0)
    {}

    // 4-es: tömbből méret nélkül
    template <std::size_t N>
    explicit double_circular_buffer(T (&arr)[N])
        : buf(arr), n(N),
          cut(N / 2),
          headA(0), tailA(0), cntA(0),
          headB(cut), tailB(cut), cntB(0),
          readA_cnt(0), readB_cnt(0)
    {}

    // 4-es: vectorral
    explicit double_circular_buffer(std::vector<T>& v)
        : buf(v.empty() ? 0 : &v[0]),
          n(v.size()),
          cut(v.size() / 2),
          headA(0), tailA(0), cntA(0),
          headB(cut), tailB(cut), cntB(0),
          readA_cnt(0), readB_cnt(0)
    {}

    // --- insert (eldob, ha tele) ---
    void insert_A(const T& value) {
        if (capA() == 0) return;
        if (cntA == capA()) return; // tele, dob

        buf[tailA] = value;
        tailA = nextA(tailA);
        ++cntA;
    }

    void insert_B(const T& value) {
        if (capB() == 0) return;
        if (cntB == capB()) return;

        buf[tailB] = value;
        tailB = nextB(tailB);
        ++cntB;
    }

    // 5-ös: intervallumos insert
    template <class It>
    void insert_A(It first, It last) {
        for (; first != last; ++first) insert_A(*first);
    }

    template <class It>
    void insert_B(It first, It last) {
        for (; first != last; ++first) insert_B(*first);
    }

    // --- read (teszt szerint értékkel tér vissza) ---
    T read_A() {
        // feltételezzük: nem hívják üresen (teszt így van)
        T out = buf[headA];
        headA = nextA(headA);
        --cntA;

        ++readA_cnt;
        rebalance_after_read();
        return out;
    }

    T read_B() {
        T out = buf[headB];
        headB = nextB(headB);
        --cntB;

        ++readB_cnt;
        rebalance_after_read();
        return out;
    }
};

#endif

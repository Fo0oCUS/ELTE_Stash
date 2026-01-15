#ifndef BTVIEW_H
#define BTVIEW_H

#include <vector>
#include <algorithm>
#include <functional> // std::less
#include <cstddef>    // size_t

template <typename T>
class binary_tree_view {
    std::vector<T>* pv;

    static std::size_t left_index(std::size_t i)  { return 2 * i + 1; }
    static std::size_t right_index(std::size_t i) { return 2 * i + 2; }

    void ensure_index(std::size_t idx) {
        if (!pv) return;
        if (pv->size() <= idx) {
            pv->resize(idx + 1, T()); // új helyek default T()
        }
    }

    static void inorder_indices(std::size_t n, std::size_t idx, std::vector<std::size_t>& out) {
        if (idx >= n) return;
        inorder_indices(n, left_index(idx), out);
        out.push_back(idx);
        inorder_indices(n, right_index(idx), out);
    }

public:
    class node {
        binary_tree_view* owner;
        std::size_t idx;

    public:
        node(binary_tree_view* o, std::size_t i) : owner(o), idx(i) {}

        // írható "érték" elérés
        T& value() {
            owner->ensure_index(idx);
            return (*(owner->pv))[idx];
        }

        const T& value() const {
            // const esetben nem resize-olunk
            return (*(owner->pv))[idx];
        }

        // hogy lehessen: node = T;
        node& operator=(const T& x) {
            value() = x;
            return *this;
        }

        // hogy lehessen: T& r = node; (implicit)
        operator T&() { return value(); }
        operator const T&() const { return value(); }

        node left()  { return node(owner, left_index(idx)); }
        node right() { return node(owner, right_index(idx)); }

        // 4-es: beszúrás (valójában resize + értékadás)
        void insert_left(const T& x) {
            std::size_t li = left_index(idx);
            owner->ensure_index(li);
            (*(owner->pv))[li] = x;
        }

        void insert_right(const T& x) {
            std::size_t ri = right_index(idx);
            owner->ensure_index(ri);
            (*(owner->pv))[ri] = x;
        }
    };

    explicit binary_tree_view(std::vector<T>& v) : pv(&v) {}

    node root() { return node(this, 0); }

    // height = floor(log2(size)) élekben mérve (1 elem -> 0, 1000 -> 9)
    int height() const {
        if (!pv || pv->empty()) return -1;
        std::size_t n = pv->size();
        int h = -1;
        std::size_t p = 1;
        while (p <= n) {
            ++h;
            if (p > (std::size_t)(-1) / 2) break;
            p <<= 1;
        }
        // p az első 2^(h+1), ami > n, tehát h jó
        return h;
    }

    // 5-ös: inorder szerint visszaírva rendez
    template <class Comp>
    void sort(Comp comp) {
        if (!pv) return;
        std::size_t n = pv->size();
        if (n <= 1) return;

        std::vector<T> vals(*pv);
        std::sort(vals.begin(), vals.end(), comp);

        std::vector<std::size_t> idxs;
        idxs.reserve(n);
        inorder_indices(n, 0, idxs); // inorder sorrendű indexlista

        // idxs mérete n lesz
        for (std::size_t i = 0; i < n; ++i) {
            (*pv)[idxs[i]] = vals[i];
        }
    }

    void sort() {
        sort(std::less<T>());
    }
};

#endif

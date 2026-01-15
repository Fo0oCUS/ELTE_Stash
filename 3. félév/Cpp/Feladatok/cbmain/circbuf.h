#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <cstddef> // size_t
#include <vector>  // ha a teszt ad vector-t is

// --------------------
// Általános template
// --------------------
template <class T>
class circular_buffer {
    T* buffer;
    std::size_t cap;
    std::size_t head;
    std::size_t tail;
    std::size_t cnt;

public:
    // pointer + kapacitás (a képed szerinti 2 paraméteres híváshoz)
    explicit circular_buffer(T* arr, std::size_t capacity)
        : buffer(arr), cap(capacity), head(0), tail(0), cnt(0) {}

    // C-tömb (1 paraméteres)
    template <std::size_t N>
    explicit circular_buffer(T (&arr)[N])
        : buffer(arr), cap(N), head(0), tail(0), cnt(0) {}

    // std::vector (1 paraméteres) – csak ha a teszt használja
    explicit circular_buffer(std::vector<T>& v)
        : buffer(v.empty() ? 0 : &v[0]),
          cap(v.size()),
          head(0), tail(0), cnt(0) {}

    std::size_t capacity() const { return cap; }
    std::size_t size() const { return cnt; }

    void insert(const T& value) {
        if (cap == 0) return;
        if (cnt == cap) return; // tele → nem írunk felül

        buffer[tail] = value;
        tail = (tail + 1) % cap;
        ++cnt;
    }

    T& read() {
        // Feltételezzük, hogy a teszt nem hívja üresen.
        T& out = buffer[head];
        head = (head + 1) % cap;
        --cnt;
        return out;
    }
};

// -------------------------------------
// Bool specializáció (5-ös) bitenként
// -------------------------------------
template <>
class circular_buffer<bool> {
    int* buffer;            // int tömbben bitenként tárolunk
    std::size_t int_count;  // hány int van
    std::size_t cap_bits;   // kapacitás bitekben
    std::size_t head;       // bit index olvasáshoz
    std::size_t tail;       // bit index íráshoz
    std::size_t cnt;        // bent lévő bitek száma

    enum { BITS = (int)(sizeof(int) * 8) };

    // bit műveletek
    bool get_bit(std::size_t bit_index) const {
        std::size_t wi = bit_index / BITS;      // word index
        std::size_t bi = bit_index % BITS;      // bit index a wordön belül
        unsigned int mask = (1u << bi);
        return ( (unsigned int)buffer[wi] & mask ) != 0;
    }

    void set_bit(std::size_t bit_index, bool value) {
        std::size_t wi = bit_index / BITS;
        std::size_t bi = bit_index % BITS;
        unsigned int mask = (1u << bi);

        if (value) {
            buffer[wi] = (int)((unsigned int)buffer[wi] | mask);
        } else {
            buffer[wi] = (int)((unsigned int)buffer[wi] & ~mask);
        }
    }

public:
    // int tömb (1 paraméteres) – ezt használja a teszt: circular_buffer<bool> b(iax);
    template <std::size_t N>
    explicit circular_buffer(int (&arr)[N])
        : buffer(arr),
          int_count(N),
          cap_bits(N * (std::size_t)BITS),
          head(0), tail(0), cnt(0) {}

    // (opcionális) pointer+méret, ha valahol így hívnák
    explicit circular_buffer(int* arr, std::size_t n_ints)
        : buffer(arr),
          int_count(n_ints),
          cap_bits(n_ints * (std::size_t)BITS),
          head(0), tail(0), cnt(0) {}

    std::size_t capacity() const { return cap_bits; }
    std::size_t size() const { return cnt; }

    void insert(bool value) {
        if (cap_bits == 0) return;
        if (cnt == cap_bits) return; // tele → nem írunk felül

        set_bit(tail, value);
        tail = (tail + 1) % cap_bits;
        ++cnt;
    }

    // bool-nál nem adhatunk T&-et (bitre nincs referencia) → értéket adunk vissza
    bool read() {
        // Feltételezzük, hogy a teszt nem hívja üresen.
        bool out = get_bit(head);
        head = (head + 1) % cap_bits;
        --cnt;
        return out;
    }
};

#endif

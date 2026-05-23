#pragma once
#include "Dictionary.hpp"

// kazdy kubelek to lista par klucz-wartosc, kolizje trafiaja do tej samej listy
// funkcja mieszajaca: h(key) = key mod capacity
class HashTableChaining : public Dictionary {
private:
    // wezel listy w kubelku
    struct Node {
        int key;
        int value;
        Node* next;
        Node(int k, int v) : key(k), value(v), next(nullptr) {}
    };

    Node** buckets;     // tablica glow list
    int capacity;       // liczba kubelkow
    int size;           // liczba par

    // po przekroczeniu tego wypelnienia powiekszamy tablice
    static constexpr double MAX_LOAD_FACTOR = 0.75;

    // zamienia klucz na indeks kubelka
    int hash(int key, int mod) const;

    // powieksza tablice i przepisuje elementy
    void rehash(int newCapacity);

public:
    HashTableChaining();
    ~HashTableChaining();

    void insert(int key, int value) override;
    bool remove(int key) override;
};

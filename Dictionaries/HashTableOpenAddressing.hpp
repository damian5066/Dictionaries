#pragma once
#include "Dictionary.hpp"

// tablica mieszajaca z adresowaniem otwartym
// wszystkie pary w jednej tablicy, przy kolizji sprawdzamy kolejne komorki
// usuniete komorki oznaczamy jako DELETED (tombstone)
class HashTableOpenAddressing : public Dictionary {
private:
    // stan komorki tablicy
    enum SlotState { EMPTY, OCCUPIED, DELETED };

    struct Slot {
        int key;
        int value;
        SlotState state;
        Slot() : key(0), value(0), state(EMPTY) {}
    };

    Slot* table;        // tablica komorek
    int capacity;       // rozmiar tablicy
    int size;           // liczba zajetych komorek
    int deletedCount;   // liczba tombstone'ow

    // przy adresowaniu otwartym wypelnienie musi byc nizsze niz w lancuchowej
    static constexpr double MAX_LOAD_FACTOR = 0.5;

    int hash(int key, int mod) const;

    // powieksza tablice (tombstone'y znikaja przy okazji)
    void rehash(int newCapacity);

public:
    HashTableOpenAddressing();
    ~HashTableOpenAddressing();

    void insert(int key, int value) override;
    bool remove(int key) override;
};

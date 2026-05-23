#include "HashTableOpenAddressing.hpp"
#include <iostream>
using namespace std;

HashTableOpenAddressing::HashTableOpenAddressing() {
    capacity = 11;
    size = 0;
    deletedCount = 0;
    table = new Slot[capacity];
    // konstruktor Slot ustawia stan na EMPTY
}

HashTableOpenAddressing::~HashTableOpenAddressing() {
    delete[] table;
}

int HashTableOpenAddressing::hash(int key, int mod) const {
    int h = key % mod;
    if (h < 0) h += mod;
    return h;
}

void HashTableOpenAddressing::rehash(int newCapacity) {
    Slot* oldTable = table;
    int oldCapacity = capacity;

    // nowa tablica
    table = new Slot[newCapacity];
    capacity = newCapacity;
    size = 0;
    deletedCount = 0;

    // przepisujemy tylko zajete komorki (tombstone'y pomijamy)
    for (int i = 0; i < oldCapacity; i++) {
        if (oldTable[i].state == OCCUPIED) {
            insert(oldTable[i].key, oldTable[i].value);
        }
    }

    delete[] oldTable;
}

void HashTableOpenAddressing::insert(int key, int value) {
    // za duze wypelnienie - powiekszamy tablice
    if ((double)(size + deletedCount + 1) / capacity > MAX_LOAD_FACTOR) {
        rehash(capacity * 2 + 1);
    }

    int idx = hash(key, capacity);
    int firstDeleted = -1; // pierwszy napotkany tombstone

    // sprawdzamy kolejne komorki
    for (int i = 0; i < capacity; i++) {
        int current = (idx + i) % capacity;

        if (table[current].state == EMPTY) {
            // klucza nie ma - wstawiamy
			if (firstDeleted != -1) { // mamy tombstone, wstawiamy tam zamiast do pustej komorki
                table[firstDeleted].key = key;
                table[firstDeleted].value = value;
                table[firstDeleted].state = OCCUPIED;
                deletedCount--;
            }
            else {
                table[current].key = key;
                table[current].value = value;
                table[current].state = OCCUPIED;
            }
            size++;
            return;
        }
        else if (table[current].state == DELETED) {
            if (firstDeleted == -1) firstDeleted = current;
        }
        else { // OCCUPIED
            // klucz juz jest - nadpisujemy wartosc
            if (table[current].key == key) {
                table[current].value = value;
                return;
            }
        }
    }

    // tablica pelna tombstone'ow - wymuszamy rehash
    rehash(capacity * 2 + 1);
    insert(key, value);
}

bool HashTableOpenAddressing::remove(int key) {
    int idx = hash(key, capacity);

    for (int i = 0; i < capacity; i++) {
        int current = (idx + i) % capacity;

        if (table[current].state == EMPTY) {
            return false; // puste miejsce - klucza nie ma
        }
        if (table[current].state == OCCUPIED && table[current].key == key) {
            // oznaczamy jako tombstone, nie czyscimy fizycznie
            table[current].state = DELETED;
            size--;
            deletedCount++;
            return true;
        }
    }

    return false;
}

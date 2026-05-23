#include "HashTableChaining.hpp"
#include <iostream>
using namespace std;

HashTableChaining::HashTableChaining() {
    // zaczynamy od malej tablicy (liczba pierwsza)
    capacity = 11;
    size = 0;
    buckets = new Node * [capacity];
    for (int i = 0; i < capacity; i++) {
        buckets[i] = nullptr;
    }
}

HashTableChaining::~HashTableChaining() {
    // zwalniamy wszystkie listy
    for (int i = 0; i < capacity; i++) {
        Node* current = buckets[i];
        while (current != nullptr) {
            Node* toDelete = current;
            current = current->next;
            delete toDelete;
        }
    }
    delete[] buckets;
}

int HashTableChaining::hash(int key, int mod) const {
    // reszta z dzielenia, zabezpieczenie przed ujemnym kluczem
    int h = key % mod;
    if (h < 0) h += mod;
    return h;
}

void HashTableChaining::rehash(int newCapacity) {
    Node** oldBuckets = buckets;
    int oldCapacity = capacity;

    // nowa, wieksza tablica
    buckets = new Node * [newCapacity];
    for (int i = 0; i < newCapacity; i++) {
        buckets[i] = nullptr;
    }
    capacity = newCapacity;

    // przepinamy wszystkie wezly (przeliczamy hash)
    for (int i = 0; i < oldCapacity; i++) {
        Node* current = oldBuckets[i];
        while (current != nullptr) {
            Node* next = current->next;

            int idx = hash(current->key, capacity);
            current->next = buckets[idx];
            buckets[idx] = current;

            current = next;
        }
    }

    delete[] oldBuckets;
}

void HashTableChaining::insert(int key, int value) {
    int idx = hash(key, capacity);

    // jesli klucz juz jest - nadpisujemy wartosc
    Node* current = buckets[idx];
    while (current != nullptr) {
        if (current->key == key) {
            current->value = value;
            return;
        }
        current = current->next;
    }

    // za duze wypelnienie - powiekszamy tablice
    if ((double)(size + 1) / capacity > MAX_LOAD_FACTOR) {
        rehash(capacity * 2 + 1);
        idx = hash(key, capacity); // indeks sie zmienil
    }

    // dodajemy na poczatek listy (O(1))
    Node* newNode = new Node(key, value);
    newNode->next = buckets[idx];
    buckets[idx] = newNode;
    size++;
}

bool HashTableChaining::remove(int key) {
    int idx = hash(key, capacity);

    Node* current = buckets[idx];
    Node* prev = nullptr;

    // szukamy klucza w liscie
    while (current != nullptr) {
        if (current->key == key) {
            // wypinamy wezel
            if (prev == nullptr) {
                buckets[idx] = current->next;
            }
            else {
                prev->next = current->next;
            }
            delete current;
            size--;
            return true;
        }
        prev = current;
        current = current->next;
    }

    return false; // nie znaleziono
}


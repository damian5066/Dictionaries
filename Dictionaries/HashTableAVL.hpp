#pragma once
#include "Dictionary.hpp"

// tablica mieszajaca z kubelkami opartymi na drzewie AVL
// jak metoda lancuchowa, ale w kubelku zamiast listy mamy zbalansowane drzewo BST
// dzieki temu operacje w kubelku sa O(log k) zamiast O(k)
// AVL trzyma warunek: wysokosci poddrzew kazdego wezla roznia sie o najwyzej 1
class HashTableAVL : public Dictionary {
private:
    // wezel drzewa AVL
    struct AVLNode {
        int key;
        int value;
        int height;         // wysokosc poddrzewa
        AVLNode* left;
        AVLNode* right;
        AVLNode(int k, int v)
            : key(k), value(v), height(1), left(nullptr), right(nullptr) {
        }
    };

    AVLNode** buckets;  // tablica korzeni drzew (po jednym na kubelek)
    int capacity;       // liczba kubelkow
    int size;           // laczna liczba par

    static constexpr double MAX_LOAD_FACTOR = 0.75;

    int hash(int key, int mod) const;

    // pomocnicze operacje na AVL
    int nodeHeight(AVLNode* node) const;        // wysokosc (0 dla nullptr)
    int balanceFactor(AVLNode* node) const;     // roznica wysokosci poddrzew
    void updateHeight(AVLNode* node);           // przelicza wysokosc
    AVLNode* rotateRight(AVLNode* y);           // rotacja w prawo
    AVLNode* rotateLeft(AVLNode* x);            // rotacja w lewo
    AVLNode* balance(AVLNode* node);            // przywraca balans (4 przypadki)

    // wstawia/nadpisuje pare, added=true jesli dodano nowy wezel
    AVLNode* insertNode(AVLNode* node, int key, int value, bool& added);

    AVLNode* findMin(AVLNode* node) const;      // najmniejszy wezel

    // usuwa klucz, removed=true jesli faktycznie usunieto
    AVLNode* removeNode(AVLNode* node, int key, bool& removed);

    void destroyTree(AVLNode* node);            // zwalnianie drzewa

    // powieksza tablice i przepisuje pary
    void rehash(int newCapacity);

    void collectInOrder(AVLNode* node) const;

public:
    HashTableAVL();
    ~HashTableAVL();

    void insert(int key, int value) override;
    bool remove(int key) override;
    void display() const override;
};

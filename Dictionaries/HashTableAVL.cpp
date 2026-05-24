#include "HashTableAVL.hpp"
#include <iostream>

using namespace std;

HashTableAVL::HashTableAVL() {
    capacity = 11;
    size = 0;
    buckets = new AVLNode * [capacity];
    for (int i = 0; i < capacity; i++) {
        buckets[i] = nullptr;
    }
}

HashTableAVL::~HashTableAVL() {
    for (int i = 0; i < capacity; i++) {
        destroyTree(buckets[i]);
    }
    delete[] buckets;
}

int HashTableAVL::hash(int key, int mod) const {
    int h = key % mod;
    if (h < 0) h += mod;
    return h;
}

int HashTableAVL::nodeHeight(AVLNode* node) const {
    // puste poddrzewo ma wysokosc 0
    return node == nullptr ? 0 : node->height;
}

int HashTableAVL::balanceFactor(AVLNode* node) const {
    // dodatni - przewaga lewej strony, ujemny - prawej
    if (node == nullptr) return 0;
    return nodeHeight(node->left) - nodeHeight(node->right);
}

void HashTableAVL::updateHeight(AVLNode* node) {
    if (node == nullptr) return;
    int hl = nodeHeight(node->left);
    int hr = nodeHeight(node->right);
    node->height = (hl > hr ? hl : hr) + 1;
}

HashTableAVL::AVLNode* HashTableAVL::rotateRight(AVLNode* y) {
    // rotacja w prawo (przypadek lewy-lewy)
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;

    x->right = y;
    y->left = T2;

    // najpierw nizszy wezel
    updateHeight(y);
    updateHeight(x);

    return x; // nowy korzen poddrzewa
}

HashTableAVL::AVLNode* HashTableAVL::rotateLeft(AVLNode* x) {
    // rotacja w lewo (przypadek prawy-prawy)
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;

    y->left = x;
    x->right = T2;

    updateHeight(x);
    updateHeight(y);

    return y;
}

HashTableAVL::AVLNode* HashTableAVL::balance(AVLNode* node) {
    if (node == nullptr) return nullptr;

    updateHeight(node);
    int bf = balanceFactor(node);

    // lewy-lewy
    if (bf > 1 && balanceFactor(node->left) >= 0) {
        return rotateRight(node);
    }
    // lewy-prawy
    if (bf > 1 && balanceFactor(node->left) < 0) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    // prawy-prawy
    if (bf < -1 && balanceFactor(node->right) <= 0) {
        return rotateLeft(node);
    }
    // prawy-lewy
    if (bf < -1 && balanceFactor(node->right) > 0) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node; // juz zbalansowane
}

HashTableAVL::AVLNode* HashTableAVL::insertNode(AVLNode* node, int key, int value, bool& added) {
    // zwykle wstawianie do BST
    if (node == nullptr) {
        added = true;
        return new AVLNode(key, value);
    }

    if (key < node->key) {
        node->left = insertNode(node->left, key, value, added);
    }
    else if (key > node->key) {
        node->right = insertNode(node->right, key, value, added);
    }
    else {
        // klucz juz jest - nadpisujemy wartosc
        node->value = value;
        added = false;
        return node;
    }

    // przywracamy balans w drodze powrotnej
    return balance(node);
}

HashTableAVL::AVLNode* HashTableAVL::findMin(AVLNode* node) const {
    // najmniejszy klucz to skrajnie lewy wezel
    while (node != nullptr && node->left != nullptr) {
        node = node->left;
    }
    return node;
}

HashTableAVL::AVLNode* HashTableAVL::removeNode(AVLNode* node, int key, bool& removed) {
    if (node == nullptr) {
        removed = false;
        return nullptr;
    }

    if (key < node->key) {
        node->left = removeNode(node->left, key, removed);
    }
    else if (key > node->key) {
        node->right = removeNode(node->right, key, removed);
    }
    else {
        // znaleziony wezel
        removed = true;

        if (node->left == nullptr || node->right == nullptr) {
            // najwyzej jedno dziecko
            AVLNode* child = (node->left != nullptr) ? node->left : node->right;
            delete node;
            return balance(child); // moze byc nullptr
        }
        else {
            // dwoje dzieci - zastepujemy nastepnikiem in-order
            AVLNode* successor = findMin(node->right);
            node->key = successor->key;
            node->value = successor->value;
            bool dummy = false;
            node->right = removeNode(node->right, successor->key, dummy);
        }
    }

    return balance(node);
}

void HashTableAVL::destroyTree(AVLNode* node) {
    if (node == nullptr) return;
    destroyTree(node->left);
    destroyTree(node->right);
    delete node;
}

void HashTableAVL::rehash(int newCapacity) {
    AVLNode** oldBuckets = buckets;
    int oldCapacity = capacity;

    buckets = new AVLNode * [newCapacity];
    for (int i = 0; i < newCapacity; i++) {
        buckets[i] = nullptr;
    }
    capacity = newCapacity;
    size = 0;

    // przechodzimy kazde stare drzewo in-order i wstawiamy pary do nowej tablicy
    for (int i = 0; i < oldCapacity; i++) {
        // recznie robiony stos do przejscia in-order
        // wysokosc AVL <= ~1.44*log2(n), 128 starcza z zapasem
        AVLNode* stack[128];
        int top = 0;
        AVLNode* current = oldBuckets[i];

        while (current != nullptr || top > 0) {
            while (current != nullptr) {
                stack[top++] = current;
                current = current->left;
            }
            current = stack[--top];

            int idx = hash(current->key, capacity);
            bool added = false;
            buckets[idx] = insertNode(buckets[idx], current->key, current->value, added);
            if (added) size++;

            current = current->right;
        }

        // zwalniamy stare drzewo
        destroyTree(oldBuckets[i]);
    }

    delete[] oldBuckets;
}


void HashTableAVL::insert(int key, int value) {
    int idx = hash(key, capacity);

    // za duze wypelnienie - powiekszamy tablice
    if ((double)(size + 1) / capacity > MAX_LOAD_FACTOR) {
        rehash(capacity * 2 + 1);
        idx = hash(key, capacity);
    }

    bool added = false;
    buckets[idx] = insertNode(buckets[idx], key, value, added);
    if (added) size++;
}

bool HashTableAVL::remove(int key) {
    int idx = hash(key, capacity);
    bool removed = false;
    buckets[idx] = removeNode(buckets[idx], key, removed);
    if (removed) size--;
    return removed;
}

#pragma once

// element slownika - para (klucz, wartosc), oba to liczby calkowite
struct Element {
    int key;
    int value;

    Element() : key(0), value(0) {}
    Element(int k, int v) : key(k), value(v) {}
};

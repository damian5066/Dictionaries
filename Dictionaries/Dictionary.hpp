#pragma once

// klasa bazowa (interfejs) dla slownika opartego na tablicy mieszajacej
// kazdy wariant tablicy musi dziedziczyc po tej klasie
// klucze i wartosci to liczby calkowite
class Dictionary {
public:
    // destruktor wirtualny
    virtual ~Dictionary() {}

    // dodanie pary klucz-wartosc, jesli klucz istnieje to nadpisuje wartosc
    virtual void insert(int key, int value) = 0;

    // usuniecie pary o danym kluczu, zwraca czy klucz istnial
    virtual bool remove(int key) = 0;

};

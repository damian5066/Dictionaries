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

    // szukanie wartosci dla klucza, zwraca czy znaleziono
    virtual bool find(int key, int& outValue) const = 0;

    // sprawdzenie czy klucz jest w slowniku
    virtual bool contains(int key) const = 0;

    // liczba par w slowniku
    virtual int returnSize() const = 0;

    // wyswietlenie zawartosci
    virtual void display() const = 0;

    // sprawdzenie czy pusty
    virtual bool isEmpty() const = 0;
};

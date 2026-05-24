#include <iostream>
#include <random>
#include <ctime>
#include <fstream>
#include <string>
#include "HashTableChaining.hpp"
#include "HashTableOpenAddressing.hpp"
#include "HashTableAVL.hpp"
#include "Timer.hpp"
#include <set>


using namespace std;

mt19937 rng;

// Stała liczba powtórzeń pojedynczej operacji na danym rozmiarze
const int NUM_REPS = 100;

// usrednione wyniki dla jednej struktury i jednego rozmiaru
struct BenchmarkResult {
    string structureName;
    int size;
    long long insert_ns;   // sredni czas insert
    long long remove_ns;   // sredni czas remove
};

// losowy klucz - zakres kilkukrotnie większy niz rozmiar 
int randomKey(int structureSize) {
    uniform_int_distribution<int> dist(1, structureSize * 5);
    return dist(rng);
}

// losowa wartosc
int randomValue() {
    uniform_int_distribution<int> dist(1, 100000);
    return dist(rng);
}

//wypelnia slownik n losowymi parami, zapewnia unikalnosc kluczy
int fillDictionaryUnique(Dictionary* dict, int n, int* keysOut) {
    set<int> used;
    int inserted = 0;
    int maxAttempts = n * 20;
    int attempts = 0;

    while (inserted < n && attempts < maxAttempts) {
        int k = randomKey(n);
        if (used.find(k) == used.end()) {
            used.insert(k);
            dict->insert(k, randomValue());
            keysOut[inserted++] = k;
        }
        attempts++;
    }
    return inserted;
}

// tworzy pusty slownik wybranego wariantu
Dictionary* createDictionary(int variant) {
    switch (variant) {
    case 0: return new HashTableChaining();
    case 1: return new HashTableOpenAddressing();
    case 2: return new HashTableAVL();
    default: return nullptr;
    }
}

const char* variantName(int variant) {
    switch (variant) {
    case 0: return "Lancuchowa";
    case 1: return "Adresowanie_otwarte";
    case 2: return "Kubelki_AVL";
    default: return "Nieznana";
    }
}

// glowna funkcja badan
// dla kazdego rozmiaru mierzymy na 20 ziarnach, to samo ziarno daje te same dane
// dla wszystkich struktur. wyniki z 20 ziaren usredniamy i zapisujemy jeden plik csv
void runBenchmarks() {
    int sizes[] = { 5000, 8000, 10000, 16000, 20000, 40000, 60000, 100000 };
    int numSizes = 8;

    int seeds[] = { 1, 6, 19, 42, 57, 78, 98, 123, 232, 345,
                    512, 999, 1111, 1555, 2024, 3, 17, 71, 256, 777 };
    int numSeeds = 20;
    const int numVariants = 3;

    cout << "WYNIKI BADAN" << endl;
    cout << "Liczba ziaren: " << numSeeds << ", powtorzen na operacje: " << NUM_REPS << endl;

    BenchmarkResult wyniki[8 * numVariants];
    int liczbaWynikow = 0;

    for (int s = 0; s < numSizes; s++) {
        int n = sizes[s];
        cout << "\nrozmiar = " << n << endl;

        long long sumInsert[numVariants] = { 0, 0, 0 };
        long long sumRemove[numVariants] = { 0, 0, 0 };

        for (int seedIdx = 0; seedIdx < numSeeds; seedIdx++) {
            for (int variant = 0; variant < numVariants; variant++) {

                rng.seed(seeds[seedIdx]);

                Dictionary* dict = createDictionary(variant);
                int* keys = new int[n];

                // wypelniamy slownik do rozmiaru n
                fillDictionaryUnique(dict, n, keys);

                // POMIAR INSERT
                // wstawiamy 1 element, mierzymy, usuwamy, rozmiar wraca do n
                long long totalInsert = 0;
                for (int rep = 0; rep < NUM_REPS; rep++) {
                    int k = randomKey(n);
                    int v = randomValue();

                    Timer t;
                    t.start();
                    dict->insert(k, v);
                    long long elapsed = t.stop();
                    totalInsert += elapsed;

                    dict->remove(k);
                }
                sumInsert[variant] += totalInsert / NUM_REPS;

                // POMIAR REMOVE
                // usuwamy 1 istniejacy element, mierzymy, wstawiamy z powrotem
                long long totalRemove = 0;
                for (int rep = 0; rep < NUM_REPS; rep++) {
                    int keyToRemove = keys[rep];

                    Timer t;
                    t.start();
                    dict->remove(keyToRemove);
                    long long elapsed = t.stop();
                    totalRemove += elapsed;

                    dict->insert(keyToRemove, randomValue());
                }
                sumRemove[variant] += totalRemove / NUM_REPS;

                delete dict;
                delete[] keys;
            }
        }

        for (int variant = 0; variant < numVariants; variant++) {
            BenchmarkResult result;
            result.structureName = variantName(variant);
            result.size = n;
            result.insert_ns = sumInsert[variant] / numSeeds;
            result.remove_ns = sumRemove[variant] / numSeeds;
            wyniki[liczbaWynikow++] = result;

            cout << "  " << result.structureName
                << " | insert (sr.) = " << result.insert_ns << " ns"
                << " | remove (sr.) = " << result.remove_ns << " ns" << endl;
        }
    }

    cout << "\nBadania zakonczone." << endl;
    rng.seed((unsigned)time(nullptr));

    string nazwaPliku = "wyniki_badan.csv";
    ofstream csv(nazwaPliku.c_str());
    if (!csv.is_open()) {
        cout << "Blad: nie mozna otworzyc pliku " << nazwaPliku << endl;
        return;
    }
    csv << "struktura;rozmiar;insert_ns;remove_ns" << endl;
    for (int i = 0; i < liczbaWynikow; i++) {
        csv << wyniki[i].structureName << ";"
            << wyniki[i].size << ";"
            << wyniki[i].insert_ns << ";"
            << wyniki[i].remove_ns << endl;
    }
    csv.close();
    cout << "Wyniki zapisano do: " << nazwaPliku << endl;
}

//MENU
void displayMainMenu() {
    cout << "\nWybierz wariant tablicy mieszajacej:" << endl;
    cout << "1. Lancuchowa (lista w kubelku)" << endl;
    cout << "2. Adresowanie otwarte (linear probing)" << endl;
    cout << "3. Kubelki z drzewem AVL" << endl;
    cout << "4. Przeprowadz badania" << endl;
    cout << "0. Wyjdz" << endl;
    cout << "Wybor: ";
}

void displayStructureMenu(const string& structureName) {
    cout << "\n" << structureName << "" << endl;
    cout << "1. Wstaw pare (insert)" << endl;
    cout << "2. Usun klucz (remove)" << endl;
    cout << "0. Wroc do menu glownego" << endl;
    cout << "Wybor: ";
}

// obsluga menu dla wybranego wariantu
void handleStructureMenu(Dictionary* dict, const string& name) {
    int choice;
    do {
        displayStructureMenu(name);
        cin >> choice;

        switch (choice) {
        case 1: {
            int key, value;
            cout << "Podaj klucz: ";
            cin >> key;
            cout << "Podaj wartosc: ";
            cin >> value;
            dict->insert(key, value);
            cout << "Wstawiono pare [" << key << " => " << value << "]" << endl;
            break;
        }
        case 2: {
            int key;
            cout << "Podaj klucz do usuniecia: ";
            cin >> key;
            if (dict->remove(key)) {
                cout << "Usunieto klucz " << key << endl;
            }
            else {
                cout << "Nie znaleziono klucza " << key << endl;
            }
            break;
        }
        case 0:
            cout << "Powrot do menu glownego." << endl;
            break;
        default:
            cout << "Zly wybor, sprobuj ponownie." << endl;
        }
    } while (choice != 0);
}

//MAIN

int main() {
    rng.seed((unsigned)time(nullptr));

    int choice;
    do {
        displayMainMenu();
        cin >> choice;

        switch (choice) {
        case 1: {
            Dictionary* dict = new HashTableChaining();
            handleStructureMenu(dict, "Tablica mieszajaca - metoda lancuchowa");
            delete dict;
            break;
        }
        case 2: {
            Dictionary* dict = new HashTableOpenAddressing();
            handleStructureMenu(dict, "Tablica mieszajaca - adresowanie otwarte");
            delete dict;
            break;
        }
        case 3: {
            Dictionary* dict = new HashTableAVL();
            handleStructureMenu(dict, "Tablica mieszajaca - kubelki z drzewem AVL");
            delete dict;
            break;
        }
        case 4:
            runBenchmarks();
            break;
        case 0:
            cout << "Do widzenia!" << endl;
            break;
        default:
            cout << "Zly wybor!" << endl;
        }
    } while (choice != 0);

    return 0;
}

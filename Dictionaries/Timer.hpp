#pragma once
#include <chrono>

// klasa do mierzenia czasu
// mierzymy w nanosekundach
class Timer {
private:
    std::chrono::high_resolution_clock::time_point startTime;

public:
    void start() {
        startTime = std::chrono::high_resolution_clock::now();
    }

    // zwraca czas w nanosekundach
    long long stop() {
        auto endTime = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime).count();
    }
};

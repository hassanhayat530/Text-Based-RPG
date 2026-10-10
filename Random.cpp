#include "Random.h"

#include <random>

int randomInRange(int min, int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}

bool chance(int percent) {
    return randomInRange(0, 99) < percent;
}
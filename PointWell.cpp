#include "PointWell.h"

bool PointWell::setMax(welltype newMax) {
    if (newMax < 1)
        return false;
    max = newMax;
    if (current > max)
        current = max;
    return true;
}

void PointWell::setCurrent(welltype newCurrent) {
    if (newCurrent < 1)
        current = 0;
    current = newCurrent;
    if (current > max)
        current = max;
}

welltype PointWell::getMax() const {
    return max;
}

welltype PointWell::getCurrent() const {
    return current;
}

bool PointWell::isFull() const {
    return current == max;
}

void PointWell::reduceCurrent(welltype damage) {
    if (damage > current) {
        current = 0;
        return;
    }
    current -= damage;
}

void PointWell::increaseCurrent(welltype amount) {
    if (amount + current > max) {
        current = max;
        return;
    }
    current += amount;
}

PointWell::PointWell() : current(1), max(1) {}

PointWell::PointWell(welltype c, welltype m) : current(c), max(m) {
    if (current > max)
        current = max;
}
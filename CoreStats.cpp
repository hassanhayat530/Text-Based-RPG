#include "CoreStats.h"

CoreStats::CoreStats()
    : strength(0), intellect(0), agility(0), armor(0), elementRes(0) {
}

CoreStats::CoreStats(stattype all) : strength(all), intellect(all), agility(all), armor(all), elementRes(all) {}

CoreStats::CoreStats(stattype s, stattype i, stattype ag, stattype arm, stattype elres)
    : strength(s), intellect(i), agility(ag), armor(arm), elementRes(elres) {
}

CoreStats& CoreStats::operator+=(const CoreStats& rhs) {
    strength += rhs.strength;
    intellect += rhs.intellect;
    agility += rhs.agility;
    armor += rhs.armor;
    elementRes += rhs.elementRes;
    return *this;
}

CoreStats& CoreStats::operator-=(const CoreStats& rhs) {
    CoreStats tmp = *this;

    strength -= rhs.strength;
    intellect -= rhs.intellect;
    agility -= rhs.agility;
    armor -= rhs.armor;
    elementRes -= rhs.elementRes;

    if (strength > tmp.strength)   strength = 0u;
    if (intellect > tmp.intellect)  intellect = 0u;
    if (agility > tmp.agility)    agility = 0u;
    if (armor > tmp.armor)      armor = 0u;
    if (elementRes > tmp.elementRes) elementRes = 0u;

    return *this;
}

CoreStats& CoreStats::operator=(const stattype& rhs) {
    strength = rhs;
    intellect = rhs;
    agility = rhs;
    armor = rhs;
    elementRes = rhs;
    return *this;
}
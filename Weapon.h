#ifndef WEAPON_H
#define WEAPON_H

#include "Item.h"

enum class WeaponSlot {
    MELEE, RANGED, NUM_SLOTS
};

class Weapon final : public EquipmentDelegate {
public:
    WeaponSlot slot;
    damagetype minDamage;
    damagetype maxDamage;
    bool is2H;

private:
    Weapon(std::string nam, CoreStats cstats, WeaponSlot ws,
        damagetype min, damagetype max, bool twoHanded = false);
    Weapon() = delete;
    Weapon(const Weapon&) = delete;
    Weapon(const Weapon&&) = delete;
    friend class ItemManager;
};

#endif
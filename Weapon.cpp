#include "Weapon.h"

Weapon::Weapon(std::string nam, CoreStats cstats, WeaponSlot ws,
    damagetype min, damagetype max, bool twoHanded)
    : EquipmentDelegate(nam, cstats), slot(ws),
    minDamage(min), maxDamage(max), is2H(twoHanded) {
}
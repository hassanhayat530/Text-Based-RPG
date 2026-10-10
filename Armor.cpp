#include "Armor.h"

Armor::Armor(std::string nam, CoreStats cstats, ArmorSlot as)
    : EquipmentDelegate(nam, cstats), slot(as) {
}
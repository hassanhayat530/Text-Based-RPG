#include "Item.h"

static std::uint32_t EQUIPMENT_UNIQUE_IDENTIFIER = 0u;

// ----- ItemDelegate -----------------------------------------------------------
ItemDelegate::~ItemDelegate() = default;
ItemDelegate::ItemDelegate(std::string nam) : name(nam) {}

// ----- EquipmentDelegate -------------------------------------------------------
EquipmentDelegate::~EquipmentDelegate() = default;

EquipmentDelegate::EquipmentDelegate(std::string nam, CoreStats cstats)
    : ItemDelegate(nam), stats(cstats), uniqueId(++EQUIPMENT_UNIQUE_IDENTIFIER) {
}

// ----- Item --------------------------------------------------------------------
Item::Item(ItemDelegate* itm) : item(itm), markedForDeletion(false) {}

Item::~Item() {
    delete item;
    item = nullptr;
}

const ItemDelegate* Item::getItem() const { return item; }

const bool Item::getmarkedForDeletion() const { return markedForDeletion; }

// ----- operator<< (uncomment if needed) ----------------------------------------
/*
std::ostream& operator<<(std::ostream& os, const Item& t) {
    Armor* tmpCast = dynamic_cast<Armor*>(t.item);
    if (tmpCast) {
        return os << tmpCast->name << "(Armor: " << tmpCast->stats.armor
                  << ", Resist: " << tmpCast->stats.elementRes << ')';
    }

    Weapon* tmpCast2 = dynamic_cast<Weapon*>(t.item);
    if (tmpCast2) {
        return os << tmpCast2->name << "(Damage: " << tmpCast2->minDamage
                  << "-" << tmpCast2->maxDamage << ')';
    }

    Potion* tmpCast3 = dynamic_cast<Potion*>(t.item);
    if (tmpCast3) {
        return os << tmpCast3->name << "( " << tmpCast3->quantity << ')';
    }

    return os;
}
*/
#include "ItemManager.h"
#include "PlayerCharacter.h"   // for full PlayerCharacter definition

Item* ItemManager::createArmor(std::string nam, CoreStats cstats, ArmorSlot as) {
    return new Item(new Armor(nam, cstats, as));
}

Item* ItemManager::createWeapon(std::string nam, CoreStats cstats, WeaponSlot ws,
    damagetype min, damagetype max, bool twoHanded) {
    return new Item(new Weapon(nam, cstats, ws, min, max, twoHanded));
}

bool ItemManager::isItemWeapon(const Item* in) {
    if (dynamic_cast<Weapon*>(in->item)) return true;
    return false;
}

bool ItemManager::isItemArmor(const Item* in) {
    if (dynamic_cast<Armor*>(in->item)) return true;
    return false;
}

bool ItemManager::isItemPotion(const Item* in) {
    if (dynamic_cast<Potion*>(in->item)) return true;
    return false;
}

void ItemManager::castItemToWeapon(const Item* in, Weapon*& out) {
    if (!in)
        return;
    out = dynamic_cast<Weapon*>(in->item);
}

void ItemManager::castItemToArmor(const Item* in, Armor*& out) {
    if (!in)
        return;
    out = dynamic_cast<Armor*>(in->item);
}

void ItemManager::castItemToPotion(const Item* in, Potion*& out) {
    if (!in)
        return;
    out = dynamic_cast<Potion*>(in->item);
}

Item* ItemManager::createPotion(std::string nam, welltype heal, itemcount quant, Buff* buff) {
    if (quant == 0) quant = 1;
    return new Item(new Potion(nam, heal, quant, buff));
}

bool ItemManager::equip(Item* itemToEquip, PlayerCharacter* pChar) {
    if (!itemToEquip || !itemToEquip->getItem() || !pChar)
        return false;

    Armor* armor = dynamic_cast<Armor*>(itemToEquip->item);
    if (armor) {
        unsigned long long slotNum = static_cast<unsigned long long>(armor->slot);
        if (pChar->equippedArmors[slotNum])
            moveToBackpack(pChar->equippedArmors[slotNum], pChar);

        pChar->equippedArmors[slotNum] = itemToEquip;
        removeFromBackpack(itemToEquip, pChar);
        return true;
    }

    Weapon* weapon = dynamic_cast<Weapon*>(itemToEquip->item);
    if (weapon) {
        unsigned long long slotNum = static_cast<unsigned long long>(weapon->slot);
        if (pChar->equippedWeapons[slotNum])
            moveToBackpack(pChar->equippedWeapons[slotNum], pChar);
        pChar->equippedWeapons[slotNum] = itemToEquip;
        removeFromBackpack(itemToEquip, pChar);
        return true;
    }

    // if item fails to equip, move it to the characters backpack
    moveToBackpack(itemToEquip, pChar);

    return false;
}

bool ItemManager::use(Item* itemToUse, PlayerCharacter* pChar) {
    if (!itemToUse || !itemToUse->getItem() || !pChar)
        return false;

    Potion* potion = dynamic_cast<Potion*>(itemToUse->item);
    if (potion) {
        // Only block if HP is full AND there is no buff to apply
        if (pChar->pcClass->HP->isFull() && !potion->buff)
            return false;

        // Apply buff first if present
        if (potion->buff)
            pChar->applyBuff(*potion->buff);

        // Apply heal
        pChar->pcClass->HP->increaseCurrent(potion->healAmount);

        // Decrement quantity and clean up if empty
        --potion->quantity;
        if (potion->quantity == 0) {
            itemToUse->markedForDeletion = true;
            pChar->cleanUpBackpack();
        }
        return true;
    }
    return false;
}

bool ItemManager::moveToBackpack(Item* itemToMove, PlayerCharacter* pChar) {
    if (!itemToMove || !itemToMove->getItem() || !pChar)
        return false;
    pChar->backpack.push_back(itemToMove);
    return true;
}

bool ItemManager::removeFromBackpack(Item* item, PlayerCharacter* pChar) {
    auto& bp = pChar->backpack;
    auto it = std::find(bp.begin(), bp.end(), item);
    if (it != bp.end()) {
        bp.erase(it);
        return true;
    }
    return false; // item wasn't in backpack — that's fine, not an error
}

void ItemManager::deleteItem(Item*& itemToDelete) {
    delete itemToDelete;
    itemToDelete = nullptr;
}
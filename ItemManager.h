#ifndef ITEMMANAGER_H
#define ITEMMANAGER_H

#include "Types.h"
#include "Armor.h"
#include "Potion.h"
#include "Weapon.h"

class PlayerCharacter;   // forward declaration

class ItemManager {
public:
    static Item* createArmor(std::string nam, CoreStats cstats, ArmorSlot as);
    static Item* createWeapon(std::string nam, CoreStats cstats, WeaponSlot ws,
        damagetype min, damagetype max, bool twoHanded = false);

    static bool isItemWeapon(const Item* in);
    static bool isItemArmor(const Item* in);
    static bool isItemPotion(const Item* in);

    static void castItemToWeapon(const Item* in, Weapon*& out);
    static void castItemToArmor(const Item* in, Armor*& out);
    static void castItemToPotion(const Item* in, Potion*& out);

    static Item* createPotion(std::string nam, welltype heal = 1u,
        itemcount quant = 1u, Buff* buff = nullptr);
    static bool equip(Item* itemToEquip, PlayerCharacter* pChar);
    static bool use(Item* itemToUse, PlayerCharacter* pChar);
    static bool moveToBackpack(Item* itemToMove, PlayerCharacter* pChar);
    static bool removeFromBackpack(Item* item, PlayerCharacter* pChar);

    static void deleteItem(Item*& itemToDelete);
};

#endif
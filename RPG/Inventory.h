#ifndef INVENTORY_H
#define INVENTORY_H

#include "Player.h"
#include "ItemManager.h"

class Inventory {
public:
    Inventory(Player* player);
    void run();

private:
    enum class Page {
        ITEM_LIST,
        ARMOR_DETAIL,
        WEAPON_DETAIL,
        POTION_DETAIL
    };

    Page currentPage = Page::ITEM_LIST;
    bool done = false;
    Player* player_;
    size_t selectedIndex = 0; // which item is currently selected

    void showItemList();

    void showArmorDetail(const Armor* armor, size_t index);
    void showWeaponDetail(const Weapon* weapon, size_t index);

    static void printArmorStats(const Armor* armor);
    static void printWeaponStats(const Weapon* weapon);

    void equipItemAt(size_t index, const std::string& name);
    void destroyItemAt(size_t index, const std::string& name);

    void showPotionDetail(const Potion* potion, size_t index);
};

#endif
#include "Inventory.h"
#include "Utils.h"
#include <iostream>

Inventory::Inventory(Player* player) : player_(player) {}

void Inventory::run() {
    while (!done) {
        clearScreen();
        switch (currentPage) {
        case Page::ITEM_LIST: showItemList(); break;
        case Page::ARMOR_DETAIL: {
            const auto& backpack = player_->us.getBackpackList();
            const Armor* armor = dynamic_cast<const Armor*>(backpack[selectedIndex]->getItem());
            showArmorDetail(armor, selectedIndex);
            break;
        }
        case Page::WEAPON_DETAIL: {
            const auto& backpack = player_->us.getBackpackList();
            const Weapon* weapon = dynamic_cast<const Weapon*>(backpack[selectedIndex]->getItem());
            showWeaponDetail(weapon, selectedIndex);
            break;
        }
        case Page::POTION_DETAIL: {
            const auto& backpack = player_->us.getBackpackList();
            const Potion* potion = dynamic_cast<const Potion*>(backpack[selectedIndex]->getItem());
            showPotionDetail(potion, selectedIndex);
            break;
        }
        }
    }
}

void Inventory::showItemList() {
    const auto& backpack = player_->us.getBackpackList();

    std::cout << "-----------------------------Inventory-----------------------\n"
        << "Gold: " << player_->gold << '\n';

    if (backpack.empty()) {
        std::cout << "Your inventory is empty.\n";
    }
    else {
        for (size_t i = 0; i < backpack.size(); ++i) {
            const ItemDelegate* delegate = backpack[i]->getItem();
            const Armor* armor = dynamic_cast<const Armor*>(delegate);
            const Weapon* weapon = dynamic_cast<const Weapon*>(delegate);
            const Potion* potion = dynamic_cast<const Potion*>(delegate);

            std::cout << "[" << i + 1 << "] ";
            if (armor)  std::cout << "(Armor)  " << armor->name << "\n";
            else if (weapon) std::cout << "(Weapon) " << weapon->name << "\n";
            else if (potion) std::cout << "(Potion) " << potion->name
                << " x" << potion->quantity << "\n";
        }
    }

    std::cout << "\n-------------------------------------------------------------\n"
        << "Enter item number to inspect, or 0 = back, c = character\n";

    // Peek first to handle single-char commands
    char peek = std::cin.peek();
    if (peek == 'b' || peek == 'c') {
        char c = getchar();
        flushInput();
        if (c == 'b') { done = true; return; }
        if (c == 'c') { showCharacter(player_); return; }
    }

    int input = readInt(); // reads full line including multi-digit numbers

    if (input <= 0) {
        if (input == 0) { done = true; return; }
        wrongInput();
        return;
    }

    size_t index = static_cast<size_t>(input - 1);
    if (index >= backpack.size()) {
        wrongInput();
        return;
    }

    selectedIndex = index;
    const ItemDelegate* delegate = backpack[index]->getItem();

    if (dynamic_cast<const Armor*> (delegate)) currentPage = Page::ARMOR_DETAIL;
    else if (dynamic_cast<const Weapon*>(delegate)) currentPage = Page::WEAPON_DETAIL;
    else if (dynamic_cast<const Potion*>(delegate)) currentPage = Page::POTION_DETAIL;
}

void Inventory::showArmorDetail(const Armor* armor, size_t index) {
    if (!armor) { currentPage = Page::ITEM_LIST; return; }

    std::cout << "-----------------------------Armor---------------------------\n";
    printArmorStats(armor);

    const Armor* equipped = player_->us.getEquippedArmorAt(
        static_cast<unsigned long long>(armor->slot));

    std::cout << "\n-----------Currently Equipped in This Slot-----------\n";
    if (equipped) printArmorStats(equipped);
    else          std::cout << "Nothing equipped in this slot.\n";

    std::cout << "\n-------------------------------------------------------------\n"
        << "e = equip     r = destroy     b = back\n";

    char c = getchar();
    flushInput();

    switch (c) {
    case 'e': equipItemAt(index, armor->name);   break;
    case 'r': destroyItemAt(index, armor->name); break;
    case 'b': currentPage = Page::ITEM_LIST;     break;
    default:  wrongInput();                      break;
    }
}

void Inventory::showWeaponDetail(const Weapon* weapon, size_t index) {
    if (!weapon) { currentPage = Page::ITEM_LIST; return; }

    std::cout << "-----------------------Weapon--------------------------\n";
    printWeaponStats(weapon);

    const Weapon* equipped = player_->us.getEquippedWeaponAt(
        static_cast<unsigned long long>(weapon->slot));

    std::cout << "\n-----------Currently Equipped in This Slot-----------\n";
    if (equipped) printWeaponStats(equipped);
    else          std::cout << "Nothing equipped in this slot.\n";

    std::cout << "\n-------------------------------------------------------------\n"
        << "e = equip     r = destroy     b = back\n";

    char c = getchar();
    flushInput();

    switch (c) {
    case 'e': equipItemAt(index, weapon->name);   break;
    case 'r': destroyItemAt(index, weapon->name); break;
    case 'b': currentPage = Page::ITEM_LIST;      break;
    default:  wrongInput();                       break;
    }
}

void Inventory::equipItemAt(size_t index, const std::string& name) {
    ItemManager::equip(player_->us.getBackpackList()[index], &player_->us);
    std::cout << name << " equipped.\n"
        << "Press any key to continue.\n";
    getchar();
    flushInput();
    currentPage = Page::ITEM_LIST;
}

void Inventory::printArmorStats(const Armor* armor) {
    std::cout << "Name:               | " << armor->name << "\n"
        << "Strength:           | " << armor->stats.strength << "\n"
        << "Intelligence:       | " << armor->stats.intellect << "\n"
        << "Agility:            | " << armor->stats.agility << "\n"
        << "Armor:              | " << armor->stats.armor << "\n"
        << "Element Resistance: | " << armor->stats.elementRes << "\n";
}

void Inventory::printWeaponStats(const Weapon* weapon) {
    std::cout << "Name:               | " << weapon->name << "\n"
        << "Damage:             | " << weapon->minDamage
        << " - " << weapon->maxDamage << "\n"
        << "Two-handed:         | " << (weapon->is2H ? "Yes" : "No") << "\n"
        << "Strength:           | " << weapon->stats.strength << "\n"
        << "Intelligence:       | " << weapon->stats.intellect << "\n"
        << "Agility:            | " << weapon->stats.agility << "\n";
}

void Inventory::destroyItemAt(size_t index, const std::string& name) {
    ItemManager::removeFromBackpack(player_->us.getBackpackList()[index], &player_->us);
    std::cout << name << " destroyed.\n"
        << "Press any key to continue.\n";
    getchar();
    flushInput();
    currentPage = Page::ITEM_LIST;
}

void Inventory::showPotionDetail(const Potion* potion, size_t index) {
    if (!potion) { currentPage = Page::ITEM_LIST; return; }

    std::cout << "-----------------------------Potion--------------------------\n"
        << "Name:               | " << potion->name << "\n"
        << "Heals:              | " << potion->healAmount << " HP\n"
        << "Quantity:           | " << potion->quantity << "\n"
        << "\n-------------------------------------------------------------\n"
        << "u = use     b = back\n";

    char c = getchar();
    flushInput();

    switch (c) {
    case 'u':
        if (potion->quantity <= 0) {
            std::cout << "No potions left.\n";
        }
        else {
            // Capture name and heal amount before use() potentially deletes the item
            std::string name = potion->name;
            welltype    healAmount = potion->healAmount;

            bool used = ItemManager::use(
                player_->us.getBackpackList()[index], &player_->us);

            if (!used) {
                std::cout << "HP is already full!\n";
            }
            else {
                std::cout << "Used " << name
                    << ". Healed " << healAmount << " HP.\n";
            }
        }
        std::cout << "Press any key to continue.\n";
        getchar();
        flushInput();
        currentPage = Page::ITEM_LIST;
        break;
    case 'b':
        currentPage = Page::ITEM_LIST;
        break;
    default:
        wrongInput();
        break;
    }
}
#include "Utils.h"

#include <iostream>

void clearScreen() {
#ifdef _WIN32
    system("CLS");
#else
    system("clear");
#endif
}

void flushInput() {
	int c;
	while ((c = getchar()) != '\n' && c != EOF);
}

void showTop(const Player* player) {
    std::cout << "----------------------------------Day: " << player->daysAlive << "----------------------------------\n";
    std::cout << "HP: " << player->us.getCurrentHP() << '/' << player->us.getMaxHP()
        << "                       "
        << "EXP: " << player->us.getCurrentEXP() << '/' << player->us.getEXPToNextLevel()
        << "                       "
        << "AP: " << player->us.getCurrentAP() << '/' << player->us.getMaxAP() << "\n\n";
    std::cout << "Gold" << player->gold << " gold" << "\n\n";
}

void showCharacter(const Player* player) {
    clearScreen();
    std::cout
        << "---------------------------Character-------------------------\n"
        << "Name:               | " << player->characterName << "\n"
        << "Class:              | " << player->us.getClassName() << "\n"
        << "Race:               | " << player->us.getRaceName() << "\n"
        << "Depth Reached:      | " << player->depthReached << "\n"
        << "-----------------------------Stats---------------------------\n"
        << "Level:              | " << player->us.getLevel() << "\n"
        << "EXP:                | " << player->us.getCurrentEXP()
        << " / "
        << player->us.getEXPToNextLevel() << "\n"
        << "HP:                 | " << player->us.getCurrentHP()
        << " / "
        << player->us.getMaxHP() << "\n"
        << "AP:                 | " << player->us.getCurrentAP()
        << " / "
        << player->us.getMaxAP() << "\n"
        << "---------------------------Attributes------------------------\n"
        << "Strength:           | " << player->us.getTotalStrength() << "\n"
        << "Intelligence:       | " << player->us.getTotalIntellect() << "\n"
        << "Agility:            | " << player->us.getTotalAgility() << "\n"
        << "Armor:              | " << player->us.getTotalArmor() << "\n"
        << "Element Resistance: | " << player->us.getTotalElementRes() << "\n"
        << "------------------------Equipped Armor-----------------------\n";

    for (size_t i = 0u; i < static_cast<size_t>(ArmorSlot::NUM_SLOTS); ++i) {
        const Armor* armor = player->us.getEquippedArmorAt(i);
        if (armor)
            std::cout << "  [" << i << "] " << armor->name << "\n";
    }

    std::cout << "------------------------Equipped Weapons---------------------\n";
    for (size_t i = 0u; i < static_cast<size_t>(WeaponSlot::NUM_SLOTS); ++i) {
        const Weapon* weapon = player->us.getEquippedWeaponAt(i);
        if (weapon)
            std::cout << "  [" << i << "] " << weapon->name << "\n";
    }

    std::cout << "---------------------------Abilities-------------------------\n\n";
    const auto abilityList = player->us.getAbilityList();
    for (size_t i = 0; i < abilityList.size(); ++i) {
        const Ability ability = abilityList[i];
        std::cout << "  [" << i + 1 << "] " << ability.name_ << "\n";
    }

    std::cout << "\n-------------------------------------------------------------\n"
        << "Press any key to go back.\n";
    char _ = getchar();
    flushInput();
}

void wrongInput() {
    clearScreen();
    std::cout << "You pressed an unknown key.\n"
        << "Press any key to go back.\n";
    char c = getchar();
    flushInput();
}

int readInt() {
    std::string line;
    std::getline(std::cin, line);
    if (line.empty()) return -1;
    for (char c : line)
        if (!std::isdigit(c)) return -1;
    try {
        return std::stoi(line);
    }
    catch (...) {
        return -1;
    }
}
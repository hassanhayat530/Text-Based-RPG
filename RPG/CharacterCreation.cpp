#include "CharacterCreation.h"
#include "Player.h"
#include "Utils.h"
#include <iostream>
#include <algorithm>
#include <cctype>

// ---------- Entry point -------------------------------------------------------

std::unique_ptr<Player> CharacterCreation::run() {
    while (!done) {
        clearScreen();
        switch (currentPage) {
        case Page::CLASS_SELECT:  showClassSelect();  break;
        case Page::CLASS_CONFIRM: showClassConfirm(); break;
        case Page::RACE_SELECT:   showRaceSelect();   break;
        case Page::RACE_CONFIRM:  showRaceConfirm();  break;
        case Page::NAME_ENTRY:    showNameEntry();    break;
        case Page::FINAL_CONFIRM: showFinalConfirm(); break;
        }
    }
    return std::move(creation);
}

// ---------- Page handlers -----------------------------------------------------

void CharacterCreation::showClassSelect() {
    std::cout << "Choose a class: \n"
        << "1 = Cleric    2 = Warrior\n"
        << "3 = Rogue     4 = Wizard\n";

    char c = getchar();
    flushInput();

    if (c < '1' || c > '4') {
        wrongInput();
        return;
    }

    lastClassChoice = c;
    playerClass = makeClass(c);
    currentPage = Page::CLASS_CONFIRM;
}

void CharacterCreation::showClassConfirm() {
    printClassInfo(lastClassChoice);

    char c = getchar();
    flushInput();

    switch (c) {
    case 'y': currentPage = Page::RACE_SELECT;  break;
    case 'n': currentPage = Page::CLASS_SELECT; break;
    default:  wrongInput(); break;
    }
}

void CharacterCreation::showRaceSelect() {
    std::cout << "Choose a race: \n"
        << "1 = Human    2 = Elf\n"
        << "3 = Dwarf    4 = Dark Elf\n";

    char c = getchar();
    flushInput();

    if (c < '1' || c > '4') {
        wrongInput();
        return;
    }

    lastRaceChoice = c;
    race = makeRace(c);
    currentPage = Page::RACE_CONFIRM;
}

void CharacterCreation::showRaceConfirm() {
    printRaceInfo(lastRaceChoice);

    char c = getchar();
    flushInput();

    switch (c) {
    case 'y': currentPage = Page::NAME_ENTRY;  break;
    case 'n': currentPage = Page::RACE_SELECT; break;
    default:  wrongInput(); break;
    }
}

void CharacterCreation::showNameEntry() {
    std::cout << "Type a name for your character:\n\n";

    std::string input;
    std::getline(std::cin, input);

    if (!isValidName(input)) {
        std::cout << "\nInvalid name. Only letters, spaces, hyphens and apostrophes allowed "
            << "(at least 2 letters, max 20 characters).\n"
            << "Press any key to try again.\n";
        char c = getchar();
        return; // stays on NAME_ENTRY
    }

    name = input;
    currentPage = Page::FINAL_CONFIRM;
}

void CharacterCreation::showFinalConfirm() {
    std::cout << "Are you sure you want to continue with the following character?\n\n"
        << "Class: " << playerClass->getClassName() << '\n'
        << "Race:  " << race->getRaceName() << '\n'
        << "Name:  " << name << "\n\n"
        << "Go back to:\n"
        << "1 = Class Selection\n"
        << "2 = Race Selection\n"
        << "3 = Name Entry\n\n"
        << "Or confirm (y):\n";

    char c = getchar();
    flushInput();

    switch (c) {
    case '1': currentPage = Page::CLASS_SELECT;  break;
    case '2': currentPage = Page::RACE_SELECT;   break;
    case '3': currentPage = Page::NAME_ENTRY;    break;
    case 'y':
        creation = std::make_unique<Player>(std::move(playerClass), std::move(race));
        creation->characterName = name;

        // Starting Equipment
        ItemManager::moveToBackpack(ItemManager::createWeapon("Crude Sword", CoreStats(), WeaponSlot::MELEE, 1, 3, false), &creation->us);
        ItemManager::moveToBackpack(ItemManager::createPotion("Minor Health Potion", 10, 2), &creation->us);

        done = true;
        break;
    default:
        wrongInput();
        break;
    }
}

// ---------- Helpers -----------------------------------------------------------

bool CharacterCreation::isValidName(const std::string& name) {
    if (name.empty() || name.size() > 20) return false;

    auto letterCount = std::count_if(name.begin(), name.end(), [](char c) {
        return std::isalpha(c);
        });

    if (letterCount < 2) return false;

    return std::all_of(name.begin(), name.end(), [](char c) {
        return std::isalpha(c) || std::isspace(c) || c == '-' || c == '\'';
        });
}

void CharacterCreation::wrongInput() {
    std::cout << "You pressed an unknown key.\n"
        << "Press any key to go back.\n";
    char c = getchar();
    flushInput();
}

void CharacterCreation::printClassInfo(char c) {
    switch (c) {
    case '1':
        std::cout << "-------------------Cleric-------------------\n\n"
            << "As a cleric, you can be a skillful divine warrior utilizing either\n"
            << "your strength to deal smiteful blows or call upon holy powers to heal yourself.\n\n"
            << "Base Attributes:\n"
            << "- HP: 14\n- AP: 10\n- Strength: 3\n- Intelligence: 5\n- Agility: 1\n"
            << "\nConfirm(y/n):";
        break;
    case '2':
        std::cout << "-------------------Warrior-------------------\n\n"
            << "Being a fearless warrior means fortitude and resilience in battle\n"
            << "and using your ruthlessness to deal heavy blows that will shake any foe.\n\n"
            << "Base Attributes:\n"
            << "- HP: 18\n- AP: 0\n- Strength: 6\n- Intelligence: 2\n- Agility: 2\n"
            << "\nConfirm(y/n):";
        break;
    case '3':
        std::cout << "-------------------Rogue-------------------\n\n"
            << "As a dextrous rogue, you can take advantage of your speed to dodge blows\n"
            << "or use your swiftness to land successful stabs more often.\n\n"
            << "Base Attributes:\n"
            << "- HP: 12\n- AP: 0\n- Strength: 3\n- Intelligence: 3\n- Agility: 6\n"
            << "\nConfirm(y/n):";
        break;
    case '4':
        std::cout << "-------------------Wizard-------------------\n\n"
            << "The stories about the arcane skills of wizards are not exaggerated. Being equipped\n"
            << "with a wide variety of spells means there is nobody more unexpected and tactical than a wizard.\n\n"
            << "Base Attributes:\n"
            << "- HP: 10\n- AP: 14\n- Strength: 1\n- Intelligence: 8\n- Agility: 2\n"
            << "\nConfirm(y/n):";
        break;
    }
}

void CharacterCreation::printRaceInfo(char c) {
    switch (c) {
    case '1':
        std::cout << "-------------------Human-------------------\n\n"
            << "Humans are ever diligent and courageous,\n"
            << "gifted more in the arts of physical weapons than magicka.\n\n"
            << "Attribute Bonuses:\n"
            << "- HP: +2\n- AP: +0\n- Strength: +2\n- Intelligence: +0\n- Agility: +1\n"
            << "\nConfirm(y/n):";
        break;
    case '2':
        std::cout << "-------------------Elf-------------------\n\n"
            << "Elves are known for their affinity with the magicks along\n"
            << "with their graceful abilities of movement.\n\n"
            << "Attribute Bonuses:\n"
            << "- HP: +0\n- AP: +0\n- Strength: +0\n- Intelligence: +3\n- Agility: +2\n"
            << "\nConfirm(y/n):";
        break;
    case '3':
        std::cout << "-------------------Dwarf-------------------\n\n"
            << "Through their hard lived upbringing in the mines where dwarves\n"
            << "grow up, they have acquired an extraordinary resilience.\n\n"
            << "Attribute Bonuses:\n"
            << "- HP: +3\n- AP: +0\n- Strength: +1\n- Intelligence: +0\n- Agility: +1\n"
            << "\nConfirm(y/n):";
        break;
    case '4':
        std::cout << "-------------------Dark Elf-------------------\n\n"
            << "Due to being cursed for turning towards evils forbidden by ordinary elves, dark elves\n"
            << "have lost some of their magicks but make up for it in fluidity and speed.\n\n"
            << "Attribute Bonuses:\n"
            << "- HP: +0\n- AP: +0\n- Strength: +0\n- Intelligence: +2\n- Agility: +3\n"
            << "\nConfirm(y/n):";
        break;
    }
}

std::unique_ptr<PlayerCharacterDelegate> CharacterCreation::makeClass(char c) {
    switch (c) {
    case '1': return std::make_unique<Cleric>();
    case '2': return std::make_unique<Warrior>();
    case '3': return std::make_unique<Rogue>();
    case '4': return std::make_unique<Wizard>();
    default:  return nullptr;
    }
}

std::unique_ptr<Race> CharacterCreation::makeRace(char c) {
    switch (c) {
    case '1': return std::make_unique<Human>();
    case '2': return std::make_unique<Elf>();
    case '3': return std::make_unique<Dwarf>();
    case '4': return std::make_unique<DarkElf>();
    default:  return nullptr;
    }
}
#include "Town.h"
#include "Utils.h"
#include "Inventory.h"
#include "SaveManager.h"
#include <iostream>

Town::Town(std::unique_ptr<Player>& mainCharacter) : mainCharacter_(mainCharacter) {}

int Town::run() {
    int depth = 1;

    while (!done) {
        clearScreen();
        showTop(mainCharacter_.get());
        switch (currentPage) {
        case Page::MAIN_OPTIONS:            showMainOptions(); break;
        case Page::TAVERN:                  showTavern(); break;
        case Page::SLUMS:                   showSlums(); break;
        case Page::LEAVE_CONFIRM:           showLeaveconfirm(); break;
        case Page::NEXT_DAY:                showNextDay(); break;
        case Page::DEPTH_SELECTION: 
            depth = showDepthSelection();
            if (depth != 0) done = true;
            break;
        }
    }

    return depth;
}

void Town::showMainOptions() {

    // Save on first entry into down after dungeon or character creation
    if (!enteredTown) {
        enteredTown = true;
        if (!SaveManager::save(*mainCharacter_))
            std::cout << "\nWarning: save failed...\n\n";
        else
            std::cout << "\nSaved Successfully...\n\n";
    }

    std::cout << "You arrive at the town of Kordovy on the outskirts of the capital. Conveniently\n"
        << "close to many places of oppurtunity for inquisitive souls such as yourself.\n"
        << "\nChoose an option: \n"
        << "1 = Go to Slums\n" << "2 = Go to the Tavern\n" << "3 = Leave for Dungeon\n"
        << "\n(c = character    i = inventory)\n";

    char c = getchar();
    flushInput();

    switch (c) {
    case '1': currentPage = Page::SLUMS;  break;
    case '2': currentPage = Page::TAVERN; break;
    case '3': currentPage = Page::LEAVE_CONFIRM; break;
    case 'c': showCharacter(mainCharacter_.get()); break;
    case 'i': {
        Inventory inv(mainCharacter_.get());
        inv.run();
        break;
    }
    default:  wrongInput(); break;
    }
}

void Town::showTavern() {
    int cost = 5 + (mainCharacter_->us.getLevel() * 2);

    std::cout << "Sleep in Tavern?\n" <<
        "Costs " << cost << " gold for a restful sleep that will heal you. \n";

    std::cout << "n = no";
    if (mainCharacter_->gold >= cost) std::cout << "        y = yes"; 
    std::cout << '\n';

    char c = getchar();
    flushInput();

    switch (c) {
    case 'y':
        if (mainCharacter_->gold >= cost) {
            currentPage = Page::NEXT_DAY;
            mainCharacter_->gold -= 5;
        }
        else wrongInput();
        break;
    case 'n': currentPage = Page::MAIN_OPTIONS; break;
    default: wrongInput(); break;
    }

    if (currentPage == Page::NEXT_DAY) {
        mainCharacter_->us.levelUp();
        ++mainCharacter_->daysAlive;

        const auto maxHP = mainCharacter_->us.getMaxHP();
        const auto maxAP = mainCharacter_->us.getMaxAP();

        mainCharacter_->us.heal(maxHP);
        mainCharacter_->us.regenerateAP(maxAP);
    }
}

void Town::showSlums() {
    std::cout << "Sleep in the slums?\n"
        << "Saves you money but sleeping with one eye open \n"
        << "in this dangerous area will not heal you.\n"
        << "y = yes       n = no\n";

    char c = getchar();
    flushInput();

    switch (c) {
    case 'y': currentPage = Page::NEXT_DAY;  break;
    case 'n': currentPage = Page::MAIN_OPTIONS; break;
    default: wrongInput(); break;
    }

    if (currentPage == Page::NEXT_DAY) {
        mainCharacter_->us.levelUp();
        ++mainCharacter_->daysAlive;
    }
}

void Town::showLeaveconfirm() {
    std::cout << "Are you sure you want to leave without resting?\n"
        << "You will not heal your health/ability points or level up if you have enough EXP\n"
        << "y = yes       n = no\n";

    char c = getchar();
    flushInput();

    switch (c) {
    case 'y': currentPage = Page::NEXT_DAY; break;
    case 'n': currentPage = Page::MAIN_OPTIONS; break;
    default: wrongInput(); break;
    }
}

void Town::showNextDay() {
    std::cout << "Another day in your adventurous life.\n"
        << "You embark on the hunt for more treasures in nearby dungeons.\n"
        << "Press any key to continue.\n";
    char _ = getchar();
    flushInput();

    currentPage = Page::DEPTH_SELECTION;
}

int Town::showDepthSelection() {
    std::cout << "Found a Dungeon! Select one of the unlocked depths to jump to.\n"
        << "You'll unlock shortcuts to new depths as you become more adept at exploring.\n"
        << "1 = Depth 1\n";

    if (mainCharacter_->depthReached >= 4) std::cout  << "2 = Depth 4\n";
    if (mainCharacter_->depthReached >= 7) std::cout  << "3 = Depth 7\n";
    if (mainCharacter_->depthReached >= 10) std::cout << "4 = Depth 10\n";

    char c = getchar();
    flushInput();

    switch (c) {
    case '1': return 1;
    case '2': if (mainCharacter_->depthReached >= 4)  return 4;  else { wrongInput(); return 0; }
    case '3': if (mainCharacter_->depthReached >= 7)  return 7;  else { wrongInput(); return 0; }
    case '4': if (mainCharacter_->depthReached >= 10) return 10; else { wrongInput(); return 0; }
    default: wrongInput(); return 0;
    }
}
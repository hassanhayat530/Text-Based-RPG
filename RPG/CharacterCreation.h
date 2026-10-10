#ifndef CHARACTERCREATION_H
#define CHARACTERCREATION_H

#include "Player.h"
#include <memory>
#include <string>

class CharacterCreation {
public:
    CharacterCreation() = default;

    // Entry point - called from printPage1 on new game
    std::unique_ptr<Player> run();

private:
    enum class Page {
        CLASS_SELECT,
        CLASS_CONFIRM,
        RACE_SELECT,
        RACE_CONFIRM,
        NAME_ENTRY,
        FINAL_CONFIRM
    };

    std::unique_ptr<Player> creation = nullptr;
    Page currentPage = Page::CLASS_SELECT;
    bool done = false;

    std::unique_ptr<PlayerCharacterDelegate> playerClass = nullptr;
    std::unique_ptr<Race> race = nullptr;
    std::string name;

    // Stores the last class/race choice char for re-display on back navigation
    char lastClassChoice = 0;
    char lastRaceChoice = 0;

    // Page handlers
    void showClassSelect();
    void showClassConfirm();
    void showRaceSelect();
    void showRaceConfirm();
    void showNameEntry();
    void showFinalConfirm();

    // Helpers
    static bool isValidName(const std::string& name);
    static void printClassInfo(char c);
    static void printRaceInfo(char c);
    static std::unique_ptr<PlayerCharacterDelegate> makeClass(char c);
    static std::unique_ptr<Race> makeRace(char c);
    static void wrongInput();
};

#endif
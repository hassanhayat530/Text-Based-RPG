#ifndef SAVEMANAGER_H
#define SAVEMANAGER_H

#include "Player.h"
#include <string>
#include <memory>
#include <fstream>

// Saves/loads or serialises/deserializes the full game state to a text file
class SaveManager {
public:
    // Returns false if save failed
    static bool save(const Player& player, const std::string& filename = "save.dat");

    // Returns nullptr if load failed or file doesn't exist
    static std::unique_ptr<Player> load(const std::string& filename = "save.dat");

    static bool saveExists(const std::string& filename = "save.dat");

private:
    // Section tags in the save file
    static constexpr const char* SECTION_PLAYER = "[PLAYER]";
    static constexpr const char* SECTION_INVENTORY = "[INVENTORY]";
    static constexpr const char* SECTION_ARMOR = "[EQUIPPED_ARMOR]";
    static constexpr const char* SECTION_WEAPONS = "[EQUIPPED_WEAPONS]";
    static constexpr const char* SECTION_ABILITIES = "[ABILITIES]";

    static void savePlayerData(std::ofstream& file, const Player& player);
    static void saveInventory(std::ofstream& file, const Player& player);
    static void saveEquippedArmor(std::ofstream& file, const Player& player);
    static void saveEquippedWeapons(std::ofstream& file, const Player& player);
    static void saveAbilities(std::ofstream& file, const Player& player);

    //static bool loadPlayerData(std::ifstream& file, Player& player);
    static bool loadInventory(std::ifstream& file, Player& player);
    static bool loadEquippedArmor(std::ifstream& file, Player& player);
    static bool loadEquippedWeapons(std::ifstream& file, Player& player);
    static bool loadAbilities(std::ifstream& file, Player& player);

    static std::unique_ptr<PlayerCharacterDelegate> makeClassFromID(ClassID id);
    static std::unique_ptr<Race> makeRaceFromID(RaceID id);
};

#endif
#include "SaveManager.h"
#include "Cleric.h"
#include "Warrior.h"
#include "Rogue.h"
#include "Wizard.h"
#include "ItemManager.h"
#include <iostream>
#include <sstream>
#include <filesystem>

// ---- Public -----------------------------------------------------------------

bool SaveManager::saveExists(const std::string& filename) {
    return std::filesystem::exists(filename);
}

bool SaveManager::save(const Player& player, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "SaveManager: failed to open " << filename << " for writing.\n";
        return false;
    }

    try {
        savePlayerData(file, player);
        saveInventory(file, player);
        saveEquippedArmor(file, player);
        saveEquippedWeapons(file, player);
        saveAbilities(file, player);  // saved last — abilities can grow over time via level ups
    }
    catch (const std::exception& e) {
        std::cerr << "SaveManager: error during save - " << e.what() << '\n';
        return false;
    }

    return true;
}

std::unique_ptr<Player> SaveManager::load(const std::string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "SaveManager: no save file found.\n";
        return nullptr;
    }

    // Enable exceptions so failed reads throw instead of silently corrupting data
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    // data collectors
    std::string line;
    std::string name;
    int classIDRaw = 0, raceIDRaw = 0;
    int daysAlive = 0;
    int gold = 0;
    int depthReached = 0;
    welltype currentHP, maxHP, currentMP = 0, maxMP = 0;
    leveltype level;
    exptype exp, expToNext;
    stattype str, intel, agi, arm, elres;

    try {
        std::getline(file, line);
        if (line != SECTION_PLAYER) {
            std::cerr << "SaveManager: corrupt save file.\n";
            return nullptr;
        }

        // Name on its own line to support spaces
        std::getline(file, name);

        // Get data in the same order it was saved
        file >> classIDRaw >> raceIDRaw
            >> daysAlive >> gold >> depthReached
            >> level >> exp >> expToNext
            >> currentHP >> maxHP
            >> currentMP >> maxMP
            >> str >> intel >> agi >> arm >> elres;

        // Consume trailing newline left by >>
        std::getline(file, line);
    }

    // Catch the file exceptions enabled earlier
    catch (const std::ios_base::failure& e) {
        std::cerr << "SaveManager: failed to read player data: " << e.what() << '\n';
        return nullptr;
    }

    auto playerClass = makeClassFromID(static_cast<ClassID>(classIDRaw));
    auto race = makeRaceFromID(static_cast<RaceID>(raceIDRaw));

    if (!playerClass || !race) {
        std::cerr << "SaveManager: unknown class or race ID in save file.\n";
        return nullptr;
    }

    // Restore stats BEFORE constructing Player so the constructor
    // doesn't apply race bonuses on top of already boosted saved stats
    playerClass->restoreStats(currentHP, maxHP, currentMP, maxMP,
        level, exp, expToNext,
        str, intel, agi, arm, elres);

    // Use LoadTag constructor to skip applyRaceBonuses in PlayerCharacter
    auto player = std::make_unique<Player>(std::move(playerClass), std::move(race), LoadTag{});
    player->characterName = name;
    player->daysAlive = daysAlive;
    player->gold = gold;
    player->depthReached = depthReached;

    if (!loadInventory(file, *player))       return nullptr;
    if (!loadEquippedArmor(file, *player))   return nullptr;
    if (!loadEquippedWeapons(file, *player)) return nullptr;

    // Load abilities last — replaces constructor-granted abilities so that
    // any abilities gained from level ups after character creation are restored
    if (!loadAbilities(file, *player))       return nullptr;

    return player;
}

// ---- Save helpers -----------------------------------------------------------

void SaveManager::savePlayerData(std::ofstream& file, const Player& player) {

    // Pass all necessary player data info under SECTION_PLAYER tag separated by newlines/spaces
    file << SECTION_PLAYER << '\n'
        << player.characterName << '\n'  // name on its own line for getline on load
        << static_cast<int>(player.us.getClassID()) << '\n'
        << static_cast<int>(player.us.getRaceID()) << '\n'
        << player.daysAlive << '\n'
        << player.gold << '\n'
        << player.depthReached << '\n'
        << player.us.getLevel() << '\n'
        << player.us.getCurrentEXP() << '\n'
        << player.us.getEXPToNextLevel() << '\n'
        << player.us.getCurrentHP() << ' ' << player.us.getMaxHP() << '\n'
        << player.us.getCurrentAP() << ' ' << player.us.getMaxAP() << '\n'
        << player.us.getBaseStrength() << ' '
        << player.us.getBaseIntellect() << ' '
        << player.us.getBaseAgility() << ' '
        << player.us.getBaseArmor() << ' '
        << player.us.getBaseElementRes() << '\n';
}

void SaveManager::saveInventory(std::ofstream& file, const Player& player) {
    const auto& backpack = player.us.getBackpackList();
    file << SECTION_INVENTORY << '\n'
        << backpack.size() << '\n';

    for (const Item* item : backpack) {
        const ItemDelegate* delegate = item->getItem();

        const Armor* armor = dynamic_cast<const Armor*>(delegate);
        if (armor) { // Will be null if item is not armor
            // Numeric fields first, name last so getline handles spaces in name when loading
            file << "ARMOR "
                << armor->stats.strength << ' '
                << armor->stats.intellect << ' '
                << armor->stats.agility << ' '
                << armor->stats.armor << ' '
                << armor->stats.elementRes << ' '
                << static_cast<int>(armor->slot) << ' '
                << armor->name << '\n';
            continue;
        }

        const Weapon* weapon = dynamic_cast<const Weapon*>(delegate);
        if (weapon) {
            file << "WEAPON "
                << weapon->stats.strength << ' '
                << weapon->stats.intellect << ' '
                << weapon->stats.agility << ' '
                << weapon->stats.armor << ' '
                << weapon->stats.elementRes << ' '
                << weapon->minDamage << ' '
                << weapon->maxDamage << ' '
                << weapon->is2H << ' '
                << static_cast<int>(weapon->slot) << ' '
                << weapon->name << '\n';
            continue;
        }

        const Potion* potion = dynamic_cast<const Potion*>(delegate);
        if (potion) {
            file << "POTION "
                << potion->healAmount << ' '
                << potion->quantity << ' '
                << potion->name << '\n';
        }
    }
}

void SaveManager::saveEquippedArmor(std::ofstream& file, const Player& player) {
    file << SECTION_ARMOR << '\n';
    for (size_t i = 0u; i < static_cast<size_t>(ArmorSlot::NUM_SLOTS); ++i) {
        const Armor* armor = player.us.getEquippedArmorAt(i);
        if (armor) {
            file << armor->stats.strength << ' '
                << armor->stats.intellect << ' '
                << armor->stats.agility << ' '
                << armor->stats.armor << ' '
                << armor->stats.elementRes << ' '
                << static_cast<int>(armor->slot) << ' '
                << armor->name << '\n';
        }
    }
    file << "END\n";
}

void SaveManager::saveEquippedWeapons(std::ofstream& file, const Player& player) {
    file << SECTION_WEAPONS << '\n';
    for (size_t i = 0u; i < static_cast<size_t>(WeaponSlot::NUM_SLOTS); ++i) {
        const Weapon* weapon = player.us.getEquippedWeaponAt(i);
        if (weapon) {
            file << weapon->stats.strength << ' '
                << weapon->stats.intellect << ' '
                << weapon->stats.agility << ' '
                << weapon->stats.armor << ' '
                << weapon->stats.elementRes << ' '
                << weapon->minDamage << ' '
                << weapon->maxDamage << ' '
                << weapon->is2H << ' '
                << static_cast<int>(weapon->slot) << ' '
                << weapon->name << '\n';
        }
    }
    file << "END\n";
}

void SaveManager::saveAbilities(std::ofstream& file, const Player& player) {
    const auto& abilities = player.us.getAbilityList();

    file << SECTION_ABILITIES << '\n'
        << abilities.size() << '\n';

    for (const Ability& ability : abilities) {
        // All numeric fields first so the name can always be read last with getline
        file << ability.HPEffect_ << ' '
            << ability.cost_ << ' '
            << ability.cooldown_ << ' '
            << static_cast<int>(ability.target_) << ' '
            << static_cast<int>(ability.scalar_) << ' ';

        // Write whether a buff exists, then buff data if so
        if (ability.givenBuff_) {
            const Buff& b = *ability.givenBuff_;

            // Format: hasBuff=1  duration  isDebuff  str int agi arm elres  buffName
            //         then ability name on the next line
            file << 1 << ' '
                << b.duration << ' '
                << (b.isDebuff ? 1 : 0) << ' '
                << b.buffedStats.strength << ' '
                << b.buffedStats.intellect << ' '
                << b.buffedStats.agility << ' '
                << b.buffedStats.armor << ' '
                << b.buffedStats.elementRes << ' '
                << b.name << '\n'  // buff name last on this line
                << ability.name_ << '\n'; // ability name on its own line
        }
        else {
            // Format: hasBuff=0  abilityName
            file << 0 << ' ' << ability.name_ << '\n';
        }
    }
}

// ---- Load helpers -----------------------------------------------------------

bool SaveManager::loadInventory(std::ifstream& file, Player& player) {
    std::string line;
    std::getline(file, line);
    if (line != SECTION_INVENTORY) return false;

    size_t count = 0;
    file >> count;
    std::getline(file, line); // consume trailing newline

    for (size_t i = 0; i < count; ++i) {
        std::getline(file, line);
        std::istringstream ss(line);

        std::string type;
        ss >> type;

        if (type == "ARMOR") {
            CoreStats stats;
            int armorSlotRaw = 0;
            std::string name;
            ss >> stats.strength >> stats.intellect
                >> stats.agility >> stats.armor
                >> stats.elementRes >> armorSlotRaw;
            ss.ignore(1, ' ');
            std::getline(ss, name);

            ItemManager::moveToBackpack(
                ItemManager::createArmor(name, stats, static_cast<ArmorSlot>(armorSlotRaw)),
                &player.us);
        }
        else if (type == "WEAPON") {
            CoreStats stats;
            damagetype minDmg, maxDmg;
            bool is2H;
            int slotRaw = 0;
            std::string name;
            ss >> stats.strength >> stats.intellect
                >> stats.agility >> stats.armor
                >> stats.elementRes >> minDmg >> maxDmg >> is2H >> slotRaw;
            ss.ignore(1, ' ');
            std::getline(ss, name);

            ItemManager::moveToBackpack(
                ItemManager::createWeapon(name, stats, static_cast<WeaponSlot>(slotRaw),
                    minDmg, maxDmg, is2H),
                &player.us);
        }
        else if (type == "POTION") {
            welltype healAmount;
            int quantity;
            std::string name;
            ss >> healAmount >> quantity;
            ss.ignore(1, ' ');
            std::getline(ss, name);

            ItemManager::moveToBackpack(
                ItemManager::createPotion(name, healAmount, quantity),
                &player.us);
        }
    }
    return true;
}

bool SaveManager::loadEquippedArmor(std::ifstream& file, Player& player) {
    std::string line;
    std::getline(file, line);
    if (line != SECTION_ARMOR) return false;

    while (std::getline(file, line) && line != "END") {
        std::istringstream ss(line);
        CoreStats stats;
        int armorSlotRaw = 0;
        std::string name;

        ss >> stats.strength >> stats.intellect
            >> stats.agility >> stats.armor
            >> stats.elementRes >> armorSlotRaw;
        ss.ignore(1, ' ');
        std::getline(ss, name);

        ItemManager::equip(
            ItemManager::createArmor(name, stats, static_cast<ArmorSlot>(armorSlotRaw)),
            &player.us);
    }
    return true;
}

bool SaveManager::loadEquippedWeapons(std::ifstream& file, Player& player) {
    std::string line;
    std::getline(file, line);
    if (line != SECTION_WEAPONS) return false;

    while (std::getline(file, line) && line != "END") {
        std::istringstream ss(line);
        CoreStats stats;
        damagetype minDmg, maxDmg;
        bool is2H;
        int slotRaw = 0;
        std::string name;

        ss >> stats.strength >> stats.intellect
            >> stats.agility >> stats.armor
            >> stats.elementRes >> minDmg >> maxDmg >> is2H >> slotRaw;
        ss.ignore(1, ' ');
        std::getline(ss, name);

        ItemManager::equip(
            ItemManager::createWeapon(name, stats, static_cast<WeaponSlot>(slotRaw),
                minDmg, maxDmg, is2H),
            &player.us);
    }
    return true;
}

bool SaveManager::loadAbilities(std::ifstream& file, Player& player) {
    std::string line;
    std::getline(file, line);
    if (line != SECTION_ABILITIES) return false;

    size_t count = 0;
    file >> count;
    std::getline(file, line); // consume trailing newline

    // Clear abilities reconstructed by the class constructor so that
    // abilities gained from level ups after character creation are fully restored
    // without duplicating the base abilities
    player.us.clearAbilities();

    for (size_t i = 0; i < count; ++i) {
        std::getline(file, line);
        std::istringstream ss(line);

        uint32_t HPEffect, cost, cooldown;
        int targetRaw, scalarRaw, hasBuff;

        ss >> HPEffect >> cost >> cooldown >> targetRaw >> scalarRaw >> hasBuff;
        ss.ignore(1, ' ');

        if (hasBuff) {
            // Buff data is on the same line as the numeric fields
            // Ability name is on the following line
            uint16_t duration;
            int isDebuff;
            CoreStats buffStats;
            std::string buffName;

            ss >> duration >> isDebuff
                >> buffStats.strength >> buffStats.intellect
                >> buffStats.agility >> buffStats.armor
                >> buffStats.elementRes;
            ss.ignore(1, ' ');
            std::getline(ss, buffName); // buff name is last on this line

            // Ability name is on its own line immediately after
            std::string abilityName;
            std::getline(file, abilityName);

            Buff* buff = new Buff(buffName.c_str(), buffStats, duration,
                static_cast<bool>(isDebuff));

            player.us.addAbility(Ability(abilityName, HPEffect, buff, cost, cooldown,
                static_cast<AbilityTarget>(targetRaw),
                static_cast<AbilityScalar>(scalarRaw)));
        }
        else {
            // No buff — ability name is at the end of the single line
            std::string abilityName;
            std::getline(ss, abilityName);

            player.us.addAbility(Ability(abilityName, HPEffect, nullptr, cost, cooldown,
                static_cast<AbilityTarget>(targetRaw),
                static_cast<AbilityScalar>(scalarRaw)));
        }
    }
    return true;
}

// ---- Creating from ID -------------------------------------------------------

std::unique_ptr<PlayerCharacterDelegate> SaveManager::makeClassFromID(ClassID id) {
    switch (id) {
    case ClassID::CLERIC:  return std::make_unique<Cleric>();
    case ClassID::WARRIOR: return std::make_unique<Warrior>();
    case ClassID::ROGUE:   return std::make_unique<Rogue>();
    case ClassID::WIZARD:  return std::make_unique<Wizard>();
    default:               return nullptr;
    }
}

std::unique_ptr<Race> SaveManager::makeRaceFromID(RaceID id) {
    switch (id) {
    case RaceID::HUMAN:   return std::make_unique<Human>();
    case RaceID::ELF:     return std::make_unique<Elf>();
    case RaceID::DWARF:   return std::make_unique<Dwarf>();
    case RaceID::DARKELF: return std::make_unique<DarkElf>();
    default:              return nullptr;
    }
}
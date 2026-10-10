#include "LootGenerator.h"
#include "Random.h" // used for uniform random methods
#include <cstdlib>

// ---- Entry point ------------------------------------------------------------

std::string LootGenerator::generateTreasure(PlayerCharacter* pc, int depth) {
    if (!pc) return ""; // guard against null

    LootType type = rollLootType(depth);
    Item* item = nullptr;

    switch (type) {
    case LootType::ARMOR:  item = generateArmor(depth);  break;
    case LootType::WEAPON: item = generateWeapon(depth); break;
    case LootType::POTION: item = generatePotion(depth); break;
    }

    if (!item) return "nothing";

    std::string name = item->getItem()->name;
    ItemManager::moveToBackpack(item, pc);
    return name;
}

// ---- Loot type roll ---------------------------------------------------------

LootGenerator::LootType LootGenerator::rollLootType(int depth) {
    // Decrease chance of potions as the player descends but increase chance of equipment
    int potionWeight = std::max(10, 40 - depth * 3);
    int weaponWeight = 30 + depth;
    int armorWeight = 100 - potionWeight - weaponWeight;
    if (armorWeight < 10) armorWeight = 10;

    int roll = randomInRange(0, 99);
    if (roll < potionWeight)                return LootType::POTION;
    if (roll < potionWeight + weaponWeight) return LootType::WEAPON;
    return LootType::ARMOR;
}

// ---- Generators -------------------------------------------------------------

Item* LootGenerator::generateArmor(int depth) {
    int slotCount = static_cast<int>(ArmorSlot::NUM_SLOTS);
    ArmorSlot slot = static_cast<ArmorSlot>(randomInRange(0, slotCount - 1));  // uniform 1/slotCount chance of getting each armor slot

    CoreStats stats = rollArmorStats(depth);
    std::string name = armorName(slot, stats);

    return ItemManager::createArmor(name, stats, slot);
}

Item* LootGenerator::generateWeapon(int depth) {
    int slotCount = static_cast<int>(WeaponSlot::NUM_SLOTS);
    WeaponSlot slot = static_cast<WeaponSlot>(randomInRange(0, slotCount - 1));
    bool is2H = chance(30);

    CoreStats stats = rollWeaponStats(depth);
    damagetype minDmg = static_cast<damagetype>(2 + depth);
    damagetype maxDmg = static_cast<damagetype>(5 + depth * 2);

    std::string name = weaponName(slot, stats, minDmg, maxDmg, is2H);
    return ItemManager::createWeapon(name, stats, slot, minDmg, maxDmg, is2H);
}

Item* LootGenerator::generatePotion(int depth) {
    welltype  healAmount = static_cast<welltype>(10 + depth * 3);
    itemcount quantity = 1;
    int roll = randomInRange(0, 99);
    if (roll < 15) quantity = 2;
    else if (roll < 5) quantity = 3;

    std::string name = potionName(healAmount, quantity);
    return ItemManager::createPotion(name, healAmount, quantity);
}

// ---- Stat rolling -----------------------------------------------------------

stattype LootGenerator::rollStatBonus(int chance_, int depth) {
    return chance(chance_) // roll 0-99 and see if it is less than chance_
        ? static_cast<stattype>(randomInRange(1, 1 + depth / 2))
        : 0;
}

CoreStats LootGenerator::rollArmorStats(int depth) {
    CoreStats stats;

    stats.armor = static_cast<stattype>(randomInRange(1, 1 + depth / 2));
    stats.strength = rollStatBonus(30, depth);
    stats.intellect = rollStatBonus(30, depth);
    stats.agility = rollStatBonus(30, depth);
    stats.elementRes = rollStatBonus(20, depth);

    return stats;
}


CoreStats LootGenerator::rollWeaponStats(int depth) {
    CoreStats stats;

    stats.strength = rollStatBonus(40, depth);
    stats.agility = rollStatBonus(40, depth);
    stats.intellect = rollStatBonus(20, depth);

    return stats;
}

// ---- Tier derivation --------------------------------------------------------

int LootGenerator::sumArmorStats(const CoreStats& stats) {
    return stats.strength + stats.intellect + stats.agility
        + stats.armor + stats.elementRes;
}

int LootGenerator::sumWeaponStats(const CoreStats& stats,
    damagetype minDmg, damagetype maxDmg) {
    return stats.strength + stats.intellect + stats.agility
        + static_cast<int>(minDmg + maxDmg) / 2;
}

LootGenerator::Tier LootGenerator::statTier(int totalStats) {
    if (totalStats <= 2)  return Tier::WORN;
    if (totalStats <= 5)  return Tier::COMMON;
    if (totalStats <= 9)  return Tier::STURDY;
    if (totalStats <= 14) return Tier::FINE;
    if (totalStats <= 20) return Tier::SUPERIOR;
    return Tier::LEGENDARY;
}

const char* LootGenerator::tierPrefix(Tier tier) {
    switch (tier) {
    case Tier::WORN:      return "Worn";
    case Tier::COMMON:    return "Common";
    case Tier::STURDY:    return "Sturdy";
    case Tier::FINE:      return "Fine";
    case Tier::SUPERIOR:  return "Superior";
    case Tier::LEGENDARY: return "Legendary";
    default:              return "Common";
    }
}

// ---- Name construction ------------------------------------------------------

std::string LootGenerator::armorName(ArmorSlot slot, const CoreStats& stats) {
    Tier tier = statTier(sumArmorStats(stats));
    std::string prefix = tierPrefix(tier);

    // Base name reflects the dominant stat
    std::string baseName;

    // Find which stat is highest to name the armor after it
    stattype highest = stats.armor;
    baseName = "Armor";

    if (stats.strength > highest) {
        highest = stats.strength;
        baseName = "Plate";  // strength-focused armor tends to be heavy plate
    }
    if (stats.agility > highest) {
        highest = stats.agility;
        baseName = "Leather";  // agility-focused is light leather
    }
    if (stats.intellect > highest) {
        highest = stats.intellect;
        baseName = "Robe";  // intellect-focused is a robe
    }
    if (stats.elementRes > highest) {
        baseName = "Vestment";  // resistance-focused
    }

    // Slot suffix gives context
    switch (slot) {
    case ArmorSlot::HEAD:  return prefix + " " + baseName + " Helm";
    case ArmorSlot::CHEST: return prefix + " " + baseName + " Chest";
    case ArmorSlot::LEGS:  return prefix + " " + baseName + " Legs";
    case ArmorSlot::FEET:  return prefix + " " + baseName + " Boots";
    case ArmorSlot::HANDS: return prefix + " " + baseName + " Gloves";
    default:               return prefix + " " + baseName;
    }
}

std::string LootGenerator::weaponName(WeaponSlot slot, const CoreStats& stats,
    damagetype minDmg, damagetype maxDmg, bool is2H) {
    Tier tier = statTier(sumWeaponStats(stats, minDmg, maxDmg));
    std::string prefix = tierPrefix(tier);

    std::string baseName;

    switch (slot) {
    case WeaponSlot::MELEE:
        // Name reflects dominant combat stat
        if (stats.intellect > stats.strength && stats.intellect > stats.agility) {
            baseName = is2H ? "Greatstaff" : "Wand";  // intellect melee
        }
        else if (stats.agility > stats.strength) {
            baseName = is2H ? "Halberd" : "Dagger";  // agility melee
        }
        else {
            baseName = is2H ? "Greatsword" : "Sword";  // strength melee
        }
        break;

    case WeaponSlot::RANGED:
        if (stats.intellect > stats.agility) {
            baseName = is2H ? "Runic Longbow" : "Enchanted Shortbow";
        }
        else {
            baseName = is2H ? "Longbow" : "Shortbow";
        }
        break;

    default:
        baseName = "Weapon";
        break;
    }

    return prefix + " " + baseName;
}

std::string LootGenerator::potionName(welltype healAmount, itemcount quantity) {
    // Potion name reflects healing power — quantity shown in inventory display
    std::string name;
    if (healAmount < 10) name = "Minor Health Potion";
    else if (healAmount < 25) name = "Health Potion";
    else if (healAmount < 40) name = "Greater Health Potion";
    else                      name = "Superior Health Potion";
    return name;
}
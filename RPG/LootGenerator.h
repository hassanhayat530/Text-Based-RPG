#ifndef LOOTGENERATOR_H
#define LOOTGENERATOR_H

#include "ItemManager.h"
#include "PlayerCharacter.h"
#include "Armor.h"
#include "Weapon.h"
#include "Potion.h"
#include <string>

class LootGenerator {
public:
    static std::string generateTreasure(PlayerCharacter* pc, int depth);

private:
    enum class LootType { ARMOR, WEAPON, POTION };

    // Tier is derived from total stat value — determines prefix and base name
    enum class Tier {
        WORN,
        COMMON,
        STURDY,
        FINE,
        SUPERIOR,
        LEGENDARY
    };

    // Determines loot type based on chance
    static LootType rollLootType(int depth);

    static Item* generateArmor(int depth);
    static Item* generateWeapon(int depth);
    static Item* generatePotion(int depth);

    static stattype rollStatBonus(int chance_, int depth); // chance-based secondary weapon/armor stats
    static CoreStats rollArmorStats(int depth);
    static CoreStats rollWeaponStats(int depth);

    // Derives tier from total stat points
    static Tier statTier(int totalStats);
    static int sumArmorStats(const CoreStats& stats);
    static int sumWeaponStats(const CoreStats& stats, damagetype minDmg, damagetype maxDmg);

    // Name construction - all use tier to pick prefix
    static std::string armorName(ArmorSlot slot, const CoreStats& stats);
    static std::string weaponName(WeaponSlot slot, const CoreStats& stats,
        damagetype minDmg, damagetype maxDmg, bool is2H);
    static std::string potionName(welltype healAmount, itemcount quantity);

    static const char* tierPrefix(Tier tier);
};

#endif
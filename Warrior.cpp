#include "Warrior.h"
#include "ItemManager.h"

Warrior::Warrior() : PlayerCharacterDelegate() {
    AP = std::make_unique<PointWell>(BASEAP, BASEAP);
    PCCONSTRUCT

        abilities.emplace_back("Pommel Strike", 2u, new Buff("Stun", CoreStats(2, 2, 2, 0, 2), 2, true), 2u, 2u, AbilityTarget::ENEMY, AbilityScalar::STR);
}

std::string Warrior::getClassName() { return "Warrior"; }

void Warrior::levelUp() {
    LEVELUP
        if (currentLevel == 2) {
            abilities.emplace_back("Power Attack", 4u, nullptr, 3u, 3u, AbilityTarget::ENEMY, AbilityScalar::STR);
        }

    if (currentLevel == 4) {
        abilities.emplace_back("Crush Armor", 4u, new Buff("Broken Armor", CoreStats(0, 0, 0, 6, 0), 3, true), 4u, 3u, AbilityTarget::ENEMY, AbilityScalar::STR);
    }

    if (currentLevel == 6) {
        abilities.emplace_back("Gather Courage", 4u, new Buff("Courage", CoreStats(4, 4, 4, 0, 4), 3, false), 4u, 3u, AbilityTarget::SELF, AbilityScalar::STR);
    }

    if (currentLevel == 8) {
        abilities.emplace_back("Taunting Attack", 7u, new Buff("Demoralized", CoreStats(4, 4, 4, 0, 4), 3, true), 6u, 3u, AbilityTarget::ENEMY, AbilityScalar::STR);
    }

    if (currentLevel == 10) {
        abilities.emplace_back("DeathBlow", 12u, nullptr, 8u, 3u, AbilityTarget::ENEMY, AbilityScalar::STR);
    }
}
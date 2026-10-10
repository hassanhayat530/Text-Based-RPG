#include "Cleric.h"

Cleric::Cleric() : PlayerCharacterDelegate() {
    AP = std::make_unique<PointWell>(BASEAP, BASEAP);
    PCCONSTRUCT
        abilities.emplace_back("Smite", 2u, nullptr, 2u, 1u, AbilityTarget::ENEMY, AbilityScalar::INT);
}

std::string Cleric::getClassName() { return "Cleric"; }

void Cleric::levelUp() {
    LEVELUP
        if (currentLevel == 2) {
            abilities.emplace_back("Heal", 2u, nullptr, 2u, 1u, AbilityTarget::SELF, AbilityScalar::INT);
        }

    if (currentLevel == 4) {
        abilities.emplace_back("Divine Onslaught", 5u, nullptr, 4u, 2u, AbilityTarget::ENEMY, AbilityScalar::INT);
    }

    if (currentLevel == 6) {
        abilities.emplace_back("Greater Heal", 6u, nullptr, 4u, 2u, AbilityTarget::SELF, AbilityScalar::INT);
    }

    if (currentLevel == 8) {
        abilities.emplace_back("Pestilence", 6u, new Buff("Poison", CoreStats(3, 3, 3, 0, 3), 3, true), 5u, 3u, AbilityTarget::ENEMY, AbilityScalar::INT);
    }

    if (currentLevel == 10) {
        abilities.emplace_back("Absolute Judgement", 10u, nullptr, 8u, 3u, AbilityTarget::ENEMY, AbilityScalar::INT);
    }
}
#include "Wizard.h"

Wizard::Wizard() : PlayerCharacterDelegate() {
    AP = std::make_unique<PointWell>(BASEAP, BASEAP);
    PCCONSTRUCT
        abilities.emplace_back("Firebolt", 4u, nullptr, 2u, 1u, AbilityTarget::ENEMY, AbilityScalar::INT);
}

std::string Wizard::getClassName() { return "Wizard"; }

void Wizard::levelUp() {
    LEVELUP
        if (currentLevel == 2) {
            abilities.emplace_back("IceBolt", 6u, nullptr, 3u, 1u, AbilityTarget::ENEMY, AbilityScalar::INT);
            increaseStats(0, 1);
        }

    if (currentLevel == 4) {
        abilities.emplace_back("Invisibility", 0u, new Buff("Invisible", CoreStats(0, 0, 7, 0, 0), 3, false), 4u, 3u, AbilityTarget::SELF, AbilityScalar::INT);
    }

    if (currentLevel == 6) {
        abilities.emplace_back("FireStorm", 8u, nullptr, 5u, 3u, AbilityTarget::ENEMY, AbilityScalar::INT);
    }

    if (currentLevel == 8) {
        abilities.emplace_back("Conjure Rock Armor", 0u, new Buff("Armor", CoreStats(0, 0, 0, 10, 0), 4, false), 7u, 4u, AbilityTarget::SELF, AbilityScalar::INT);
    }

    if (currentLevel == 10) {
        abilities.emplace_back("Electrocution", 10u, new Buff("Dizzy", CoreStats(6, 6, 6, 0, 6), 4, true), 10u, 4u, AbilityTarget::ENEMY, AbilityScalar::INT);
    }
}
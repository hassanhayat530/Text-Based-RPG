#include "Rogue.h"

Rogue::Rogue() : PlayerCharacterDelegate() {
    AP = std::make_unique<PointWell>(BASEAP, BASEAP);
    PCCONSTRUCT

        abilities.emplace_back("Sneak Attack", 3u, nullptr, 2u, 2u, AbilityTarget::ENEMY, AbilityScalar::AGI);
}

std::string Rogue::getClassName() { return "Rogue"; }

void Rogue::levelUp() {
    LEVELUP
        if (currentLevel == 2) {
            abilities.emplace_back("Precise Attack", 6u, nullptr, 4u, 3u, AbilityTarget::ENEMY, AbilityScalar::AGI);
        }

    if (currentLevel == 4) {
        abilities.emplace_back("Fight Dirty", 4u, new Buff("Disorient", CoreStats(2, 2, 2, 0, 0), 2u, true), 4u, 4u, AbilityTarget::ENEMY, AbilityScalar::AGI);
    }

    if (currentLevel == 6) {
        abilities.emplace_back("Steal And Use Potion", 6u, nullptr, 6u, 4u, AbilityTarget::SELF, AbilityScalar::AGI);
    }

    if (currentLevel == 8) {
        abilities.emplace_back("Persistance Stabber", 8u, nullptr, 8u, 4u, AbilityTarget::SELF, AbilityScalar::AGI);
    }

    if (currentLevel == 10) {
        abilities.emplace_back("Death's Kiss", 8u, new Buff("Strong Poison", CoreStats(4, 4, 4, 0, 4), 2u, true), 10u, 6u, AbilityTarget::ENEMY, AbilityScalar::AGI);
    }
}
#ifndef WIZARD_H
#define WIZARD_H
#include "PlayerCharacter.h"
/**
 * @brief A fragile but devastatingly powerful arcane spellcaster.
 *
 * Wizards have the lowest HP and strength of all classes but the highest
 * intellect by a significant margin, making their abilities far more powerful
 * than those of any other class. Their toolkit is the most varied, covering
 * raw damage, agility buffs, and defensive armor conjuration.
 */
class Wizard : public PlayerCharacterDelegate {
public:
    /** @return @ref ClassID::WIZARD for serialisation by @ref SaveManager. */
    ClassID getClassID() const override { return ClassID::WIZARD; }
    static const welltype BASEHP = 10u;
    static const welltype BASEAP = 14u;
    static const stattype BASESTR = 1u;
    static const stattype BASEINT = 8u;
    static const stattype BASEAGI = 2u;
    /**
     * @brief Constructs a Wizard and initialises AP, base stats, and starting abilities.
     *
     * Initialises the AP pool, applies base stats via @ref PCCONSTRUCT, and grants
     * the starting ability Firebolt (INT-scaling enemy damage, AP cost 2, cooldown 1).
     */
    Wizard();

    /** @return The string "Wizard". */
    std::string getClassName() override;

    /**
     * @brief Applies level-up stat scaling and grants new abilities at key levels.
     *
     * Calls @ref LEVELUP to scale HP, AP, and stats, then checks the current level
     * to grant new abilities. Also grants a bonus intellect point at level 2
     * to reflect accelerating arcane mastery.
     * Level 2:  IceBolt (stronger INT-scaling enemy damage) + bonus INT point
     * Level 4:  Invisibility (self-buff granting high agility bonus for 3 turns)
     * Level 6:  FireStorm (heavy INT-scaling enemy damage)
     * Level 8:  Conjure Rock Armor (self-buff granting large armor bonus for 4 turns)
     * Level 10: Electrocution (INT-scaling damage + Dizzy debuff to enemy)
     */
    void levelUp() override;
};
#endif
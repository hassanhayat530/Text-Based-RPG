#ifndef CLERIC_H
#define CLERIC_H

#include "PlayerCharacter.h"

/**
 * @brief A divine warrior class balanced between melee combat and holy magic.
 *
 * Clerics have moderate HP and strength with high intellect, making their
 * abilities scale well. They are one of the only two classes with access to healing abilities
 * along with the resourceful rogue, making them resilient over long dungeon runs.
 */
class Cleric : public PlayerCharacterDelegate {
public:
    /** @return @ref ClassID::CLERIC for serialisation by @ref SaveManager. */
    ClassID getClassID() const override { return ClassID::CLERIC; }

    static const welltype BASEHP = 14u;
    static const welltype BASEAP = 10u;
    static const stattype BASESTR = 3u;
    static const stattype BASEINT = 5u;
    static const stattype BASEAGI = 1u;

    /**
     * @brief Constructs a Cleric and initialises AP, base stats, and starting abilities.
     *
     * Initialises the AP pool, applies base stats via @ref PCCONSTRUCT, and grants
     * the starting ability Smite (INT-scaling enemy damage, AP cost 2, cooldown 1).
     */
    Cleric();

    /** @return The string "Cleric". */
    std::string getClassName() override;

    /**
     * @brief Applies level-up stat scaling and grants new abilities at key levels.
     *
     * Calls @ref LEVELUP to scale HP, AP, and stats, then checks the current level
     * to grant new abilities:
     * Level 2:  Heal (self-heal scaling with INT)
     * Level 4:  Divine Onslaught (heavy INT-scaling enemy damage)
     * Level 6:  Greater Heal (stronger self-heal scaling with INT)
     * Level 8:  Pestilence (INT-scaling damage + Poison debuff to enemy)
     * Level 10: Absolute Judgement (highest damage ability, INT scaling)
     */
    void levelUp() override;
};

#endif
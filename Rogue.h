#ifndef ROGUE_H
#define ROGUE_H

#include "PlayerCharacter.h"

/**
 * @brief A swift and deceptive class that leverages agility for combat advantage.
 *
 * Rogues have low HP but the highest base agility of all classes, giving them
 * superior hit and dodge rates in combat. They are one of two classes with access
 * to healing abilities alongside the Cleric, though their recovery is tied to
 * stealing from enemies rather than holy magic.
 */
class Rogue : public PlayerCharacterDelegate {
public:
    /** @return @ref ClassID::ROGUE for serialisation by @ref SaveManager. */
    ClassID getClassID() const override { return ClassID::ROGUE; }

    static const welltype BASEHP = 12u;
    static const welltype BASEAP = 12u;
    static const stattype BASESTR = 3u;
    static const stattype BASEINT = 3u;
    static const stattype BASEAGI = 6u;

    /**
     * @brief Constructs a Rogue and initialises AP, base stats, and starting abilities.
     *
     * Initialises the AP pool, applies base stats via @ref PCCONSTRUCT, and grants
     * the starting ability Sneak Attack (AGI-scaling enemy damage, AP cost 2, cooldown 2).
     */
    Rogue();

    /** @return The string "Rogue". */
    std::string getClassName() override;

    /**
     * @brief Applies level-up stat scaling and grants new abilities at key levels.
     *
     * Calls @ref LEVELUP to scale HP, AP, and stats, then checks the current level
     * to grant new abilities:
     * Level 2:  Precise Attack (heavy AGI-scaling enemy damage)
     * Level 4:  Fight Dirty (AGI-scaling damage + Disorient debuff to enemy)
     * Level 6:  Steal And Use Potion (AGI-scaling self-heal)
     * Level 8:  Persistence Stabber (AGI-scaling enemy damage)
     * Level 10: Death's Kiss (AGI-scaling damage + Strong Poison debuff to enemy)
     */
    void levelUp() override;
};

#endif
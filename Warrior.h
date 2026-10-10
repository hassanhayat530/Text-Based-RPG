#ifndef WARRIOR_H
#define WARRIOR_H
#include "PlayerCharacter.h"
/**
 * @brief A heavily armoured frontline fighter that excels at sustained melee combat.
 *
 * Warriors have the highest base HP and strength of all classes, making them
 * the most durable and hardest hitting in direct combat. Their abilities focus
 * on debuffing enemies and buffing themselves rather than magic, reflecting their
 * purely physical fighting style.
 */
class Warrior : public PlayerCharacterDelegate {
public:
    /** @return @ref ClassID::WARRIOR for serialisation by @ref SaveManager. */
    ClassID getClassID() const override { return ClassID::WARRIOR; }
    static const welltype BASEHP = 18u;
    static const welltype BASEAP = 10u;
    static const stattype BASESTR = 6u;
    static const stattype BASEINT = 2u;
    static const stattype BASEAGI = 2u;
    /**
     * @brief Constructs a Warrior and initialises AP, base stats, and starting abilities.
     *
     * Initialises the AP pool, applies base stats via @ref PCCONSTRUCT, and grants
     * the starting ability Pommel Strike (STR-scaling enemy damage + Stun debuff, AP cost 2, cooldown 2).
     */
    Warrior();
    /** @return The string "Warrior". */
    std::string getClassName() override;
    /**
     * @brief Applies level-up stat scaling and grants new abilities at key levels.
     *
     * Calls @ref LEVELUP to scale HP, AP, and stats, then checks the current level
     * to grant new abilities:
     * Level 2:  Power Attack (heavy STR-scaling enemy damage)
     * Level 4:  Crush Armor (STR-scaling damage + Broken Armor debuff to enemy)
     * Level 6:  Gather Courage (STR-scaling self-buff granting Courage)
     * Level 8:  Taunting Attack (STR-scaling damage + Demoralized debuff to enemy)
     * Level 10: DeathBlow (highest damage ability, pure STR scaling)
     */
    void levelUp() override;
};
#endif
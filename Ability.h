#ifndef ABILITY_H
#define ABILITY_H

#include <cstdint>
#include <string>
#include "Buff.h"

/**
 * @brief Determines what an ability targets when used.
 */
enum class AbilityTarget {
    SELF,   ///< Targets the user: heals or applies a buff to self or both.
    ALLY,   ///< Targets an ally. Falls back to self if no ally system is present.
    ENEMY   ///< Targets the current enemy: deals damage or applies a debuff or both.
};

/**
 * @brief Determines which stat scales the ability's HP effect.
 *
 * The scalar stat's total value is added to @ref Ability::HPEffect_ at resolution time.
 * @ref AbilityMenu reads this to calculate and display the total effect before use.
 */
enum AbilityScalar {
    NONE, ///< No scaling: HPEffect_ is used as-is.
    STR,  ///< Scales with total strength.
    AGI,  ///< Scales with total agility.
    INT   ///< Scales with total intellect.
};

/**
 * @brief Represents an active ability that a character can use in combat.
 *
 * An ability can deal damage, heal, apply a buff or debuff, or any combination.
 * It may cost AP to use and has a cooldown measured in combat turns.
 *
 * @note This struct owns its @ref givenBuff_ via a raw pointer. The copy constructor
 * and assignment operator perform deep copies to avoid double-deletion bugs when
 * abilities are stored in vectors.
 */
struct Ability {
    /**
     * @brief Constructs an ability with the given parameters.
     * @param name      Display name shown in @ref AbilityMenu.
     * @param HPEffect  Base HP effect: damage dealt or HP restored before scalar is applied.
     * @param givenBuff Optional buff or debuff applied on use. Ownership is taken by this ability.
     * @param cost      AP cost to use this ability. 0 means free.
     * @param cooldown  Number of combat turns before the ability can be used again.
     * @param target    Who the ability targets: see @ref AbilityTarget.
     * @param scalar    Which stat scales the HP effect: see @ref AbilityScalar.
     */
    Ability(std::string name = "unnamed",
        uint32_t HPEffect = 1u,
        Buff* givenBuff = nullptr,
        uint32_t cost = 0u,
        uint32_t cooldown = 1u,
        AbilityTarget target = AbilityTarget::SELF,
        AbilityScalar scalar = AbilityScalar::NONE);

    /**
     * @brief Destructor. Deletes @ref givenBuff_ if present.
     */
    ~Ability();

    /**
     * @brief Deep copy constructor.
     *
     * Creates a new @ref Buff on the heap if @ref givenBuff_ is not null,
     * so both the original and the copy own independent Buff instances.
     * @param other The ability to copy from.
     */
    Ability(const Ability& other);

    /**
     * @brief Deep copy assignment operator.
     *
     * Deletes the existing @ref givenBuff_ before copying from other.
     * Guards against self-assignment.
     * @param other The ability to assign from.
     * @return Reference to this ability.
     */
    Ability& operator=(const Ability& other);

    /** @brief Display name of this ability. Shown in @ref AbilityMenu and combat log. */
    std::string name_;

    /**
     * @brief Base HP effect before scalar contribution.
     * Positive value heals when targeting self, deals damage when targeting an enemy.
     */
    uint32_t HPEffect_;

    /**
     * @brief Optional buff or debuff applied on use.
     * Applied to self for @ref AbilityTarget::SELF and @ref AbilityTarget::ALLY targets.
     * Applied to the monster for @ref AbilityTarget::ENEMY targets.
     * Null if this ability has no buff effect.
     */
    Buff* givenBuff_;

    /** @brief AP cost to use this ability. Checked against current AP before use in @ref AbilityMenu. */
    uint32_t cost_;

    /**
     * @brief Number of combat turns this ability is unavailable after use.
     * Tracked in @ref CombatController::cooldowns_ and ticked down by
     * @ref CombatController::tickCooldowns each round.
     */
    uint32_t cooldown_;

    /** @brief Determines what this ability targets. See @ref AbilityTarget. */
    AbilityTarget target_;

    /**
     * @brief Determines which stat scales @ref HPEffect_. See @ref AbilityScalar.
     * The scaling stat's total value is added to @ref HPEffect_ at resolution time
     * inside @ref AbilityMenu::useAbility.
     */
    AbilityScalar scalar_;
};

#endif // !ABILITY_H
#ifndef CORESTATS_H
#define CORESTATS_H

#include "types.h"

/**
 * @brief A plain aggregate of the five core combat stats shared across the game.
 *
 * Used as the building block for @ref StatBlock base stats and buff modifiers,
 * @ref EquipmentDelegate item bonuses, and @ref Buff stat changes.
 * All arithmetic operators work component-wise across all five fields.
 */
struct CoreStats {
    stattype strength;   ///< Contributes to melee damage via @ref PlayerCharacter::meleeAttack.
    stattype intellect;  ///< Scales ability effects for INT-based classes.
    stattype agility;    ///< Contributes to hit chance, dodge chance, and flee success rate.
    stattype armor;      ///< Reduces incoming physical damage. Summed across all equipped armor.
    stattype elementRes; ///< Reduces incoming magic and ability damage.

    /** @brief Default constructor. Initialises all fields to 0. */
    CoreStats();

    /**
     * @brief Convenience constructor that sets all five fields to the same value.
     * @param all Value assigned to every stat.
     */
    CoreStats(stattype all);

    /**
     * @brief Constructs a CoreStats with explicit values for each field.
     * @param s     Strength.
     * @param i     Intellect.
     * @param ag    Agility.
     * @param arm   Armor.
     * @param elres Element resistance.
     */
    CoreStats(stattype s, stattype i, stattype ag, stattype arm, stattype elres);

    /**
     * @brief Adds another CoreStats component-wise.
     * Used by @ref StatBlock::recalculateBuffs to accumulate buff contributions.
     */
    CoreStats& operator+=(const CoreStats& rhs);

    /**
     * @brief Subtracts another CoreStats component-wise with underflow protection.
     *
     * Since @ref stattype is unsigned, subtracting a larger value would wrap around
     * to a very large number. This operator forces any field that would underflow to 0
     * instead. Used by @ref StatBlock::recalculateBuffs when applying debuffs.
     */
    CoreStats& operator-=(const CoreStats& rhs);

    /**
     * @brief Assigns a single scalar value to all five fields simultaneously.
     * @param rhs Value to assign to every stat.
     */
    CoreStats& operator=(const stattype& rhs);
};

#endif
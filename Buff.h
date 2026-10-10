#ifndef BUFF_H
#define BUFF_H

#include <string>
#include "CoreStats.h"

/**
 * @brief Represents a temporary stat modifier applied to a character or monster.
 *
 * A buff increases stats for a set number of turns. A debuff (when @ref isDebuff
 * is true) reduces them. Both are handled by the same struct: the sign of the
 * effect is determined by @ref StatBlock::recalculateBuffs which subtracts debuff
 * stats rather than adding them.
 *
 * Buffs are managed by @ref StatBlock::addNewBuff, which refreshes duration if a
 * buff with the same name already exists rather than stacking. Duration is ticked
 * down each combat round by @ref PlayerCharacter::tickBuffs and @ref Monster::tickBuffs.
 */
struct Buff {
    /**
     * @brief Constructs a buff with individual stat values.
     * @param nam     Display name. Used as an identifier for refresh logic in @ref StatBlock::addNewBuff.
     * @param str     Strength modifier.
     * @param intel   Intellect modifier.
     * @param agi     Agility modifier.
     * @param arm     Armor modifier.
     * @param elres   Element resistance modifier.
     * @param dur     Duration in combat turns before the buff expires.
     * @param isdebuff If true, all stat modifiers are subtracted rather than added.
     */
    Buff(const char* nam = "undefined",
        stattype str = 0, stattype intel = 0,
        stattype agi = 0, stattype arm = 0,
        stattype elres = 0,
        uint16_t dur = 2u, bool isdebuff = false);

    /**
     * @brief Constructs a buff from a @ref CoreStats struct.
     * @param nam      Display name.
     * @param cs       Stat modifiers packed into a CoreStats struct.
     * @param dur      Duration in combat turns.
     * @param isdebuff If true, all stat modifiers are subtracted rather than added.
     */
    Buff(const char* nam, CoreStats cs, uint16_t dur = 2u, bool isdebuff = false);

    /** @brief Display name. Also used as the unique key for refresh logic in @ref StatBlock::addNewBuff. */
    std::string name;

    /**
     * @brief Remaining duration in combat turns.
     * Decremented each round by @ref PlayerCharacter::tickBuffs.
     * When it reaches 0 the buff is removed and stats are recalculated.
     */
    uint16_t duration;

    /** @brief The stat values this buff modifies. Added or subtracted depending on @ref isDebuff. */
    CoreStats buffedStats;

    /**
     * @brief Whether this buff acts as a debuff.
     * If true, @ref StatBlock::recalculateBuffs subtracts @ref buffedStats
     * rather than adding them, reducing the target's effective stats.
     */
    bool isDebuff;
};

#endif
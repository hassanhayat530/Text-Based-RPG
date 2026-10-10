#ifndef STATBLOCK_H
#define STATBLOCK_H

#include "Buff.h"
#include "Types.h"
#include <vector>

/**
 * @brief Stores and calculates base and buffed stats for any entity in the game.
 *
 * Base stats are permanent and modified by @ref increaseStats.
 * Buff stats are recalculated automatically whenever a buff is added via @ref addNewBuff.
 * This class is inherited by @ref PlayerCharacterDelegate and @ref Monster.
 */
class StatBlock {
    CoreStats base;
    CoreStats fromBuffs;

public:
    /**
     * @brief Constructs a StatBlock with optional base stat values.
     * @param s     Base strength (default 1)
     * @param i     Base intellect (default 1)
     * @param a     Base agility (default 1)
     * @param arm   Base armor (default 0)
     * @param elres Base element resistance (default 0)
     */
    explicit StatBlock(stattype s = 1, stattype i = 1, stattype a = 1,
        stattype arm = 0, stattype elres = 0);

    void setBaseStats(stattype s, stattype i, stattype a,
        stattype arm, stattype elres);

    /** @brief Returns the base strength before buffs. */
    stattype getBaseStrength() const;
    stattype getBaseIntellect() const;
    stattype getBaseAgility() const;
    stattype getBaseArmor() const;
    stattype getBaseElementRes() const;

    /** @brief Returns total strength including active buff contributions. */
    stattype getTotalStrength() const;
    stattype getTotalIntellect() const;
    stattype getTotalAgility() const;
    stattype getTotalArmor() const;
    stattype getTotalElementRes() const;

    void tickBuffs(); // decrements buff durations, removes expired ones

protected:
    /** @brief Active buffs applied to this entity. */
    std::vector<Buff> buffs;

    /**
     * @brief Adds or refreshes a buff and recalculates buffed stats.
     * @param b The buff to apply. If a buff with the same name exists, its duration is reset.
     */
    void addNewBuff(Buff b);

    /**
     * @brief Permanently increases base stats by the given amounts.
     * @param s     Strength increase
     * @param i     Intellect increase
     * @param a     Agility increase
     * @param arm   Armor increase
     * @param elres Element resistance increase
     */
    void increaseStats(stattype s = 0, stattype i = 0, stattype a = 0,
        stattype arm = 0, stattype elres = 0);

    /** @brief Overload that takes a @ref CoreStats struct directly. */
    void increaseStats(CoreStats cs);

private:
    /** @brief Recalculates @ref fromBuffs from the current buff list. Called automatically. */
    void recalculateBuffs();
};

#endif
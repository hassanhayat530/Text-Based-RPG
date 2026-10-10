#ifndef POINTWELL_H
#define POINTWELL_H

#include "Types.h"

/**
 * @brief Stores and modifies point pools for any entity in the game.
 * 
 * Used to track HP and MP for both player characters and monsters.
 * The current value is always kept between 0 and max — it can
 * never exceed max or drop below 0.
 */
class PointWell {
public:
    /**
     * @brief Default constructor. Initialises current and max to 0.
     */
    PointWell();

    /**
     * @brief Constructs a PointWell with a given current and maximum value.
     * @param c Starting current value.
     * @param m Maximum value. Current is set to this if c exceeds it.
     */
    PointWell(welltype c, welltype m);

    /**
     * @brief Sets a new maximum value.
     * @param newMax The new maximum. If current exceeds the new max it is clamped down.
     * @return True if the max was changed successfully, false if newMax was invalid.
     */
    bool setMax(welltype newMax);

    /**
     * @brief Directly sets the current value.
     * @param newCurrent The value to set. Stays within [0, max].
     * @note Prefer @ref increaseCurrent and @ref reduceCurrent for normal gameplay use.
     *       This method is intended for save/load restoration.
     */
    void setCurrent(welltype newCurrent);

    /**
     * @brief Returns the maximum value of this well.
     * @return The maximum value.
     */
    welltype getMax() const;

    /**
     * @brief Returns the current value of this well.
     * @return The current value.
     */
    welltype getCurrent() const;

    /**
     * @brief Returns whether the current value equals the maximum.
     * @return True if full, false otherwise.
     */
    bool isFull() const;

    /**
     * @brief Reduces the current value by the given amount.
     * @param damage Amount to reduce by. Current will not drop below 0.
     */
    void reduceCurrent(welltype damage);

    /**
     * @brief Increases the current value by the given amount.
     * @param amount Amount to increase by. Current will not exceed max.
     */
    void increaseCurrent(welltype amount);

private:
    /** @brief The current value of this well. Always in the range [0, max]. */
    welltype current;

    /** @brief The maximum value this well can hold. */
    welltype max;
};

#endif
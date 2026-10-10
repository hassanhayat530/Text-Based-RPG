#ifndef ITEM_H
#define ITEM_H

#include "CoreStats.h"
#include "buff.h"
#include <string>
#include <typeinfo>
#include <iostream>

/**
 * @brief Abstract base class for all item types.
 *
 * Holds only the item's display name. Concrete item types (Armor, Weapon, Potion)
 * inherit from this or from @ref EquipmentDelegate. The delegate is always owned
 * by an @ref Item which manages its lifetime.
 *
 * @note The constructor is protected: only @ref Item and its subclasses
 * may construct delegates directly.
 */
class ItemDelegate {
public:
    /** @brief Display name of this item. Used in inventory display and save serialisation. */
    std::string name;

    virtual ~ItemDelegate() = 0;

protected:
    /**
     * @brief Constructs an ItemDelegate with the given name.
     * @param nam Display name of the item.
     */
    ItemDelegate(std::string nam);

    /** @brief Grants @ref Item access to construct delegates. */
    friend class Item;
};

/**
 * @brief Abstract base class for equippable items (Armor and Weapon).
 *
 * Extends @ref ItemDelegate with stat bonuses and a unique ID. The unique ID
 * is assigned automatically at construction from a static counter, ensuring
 * every equipment piece can be identified individually even if two items share
 * the same name and stats.
 */
class EquipmentDelegate : public ItemDelegate {
public:
    /**
     * @brief Unique identifier assigned at construction.
     * Incremented from a static counter in Item.cpp. Used to distinguish
     * two otherwise identical items.
     */
    const std::uint32_t uniqueId;

    /** @brief Stat bonuses granted to the character while this item is equipped. */
    CoreStats stats;

    virtual ~EquipmentDelegate() = 0;

protected:
    /**
     * @brief Constructs an EquipmentDelegate with a name, stats, and an auto-assigned unique ID.
     * @param nam    Display name of the item.
     * @param cstats Stat bonuses granted when equipped.
     */
    EquipmentDelegate(std::string nam, CoreStats cstats);
};

/**
 * @brief The owning subclass around an @ref ItemDelegate.
 *
 * All items in the game are accessed and managed through this class.
 * It owns its delegate via a raw pointer and deletes it in the destructor.
 *
 * Construction is restricted to @ref ItemManager which is the sole factory
 * for all item instances. @ref PlayerCharacter is also a friend to allow
 * direct access during backpack cleanup.
 *
 * The @ref markedForDeletion flag signals that the item should be removed from
 * the backpack at the next cleanup - set by @ref ItemManager::use when the last
 * potion charge is consumed.
 *
 * @note The destructor is private — items must never be deleted directly.
 * Use @ref ItemManager::deleteItem for explicit deletion.
 */
class Item final {
public:
    /** @return Const pointer to the underlying @ref ItemDelegate for reading item data. */
    const ItemDelegate* getItem() const;

    /** @return True if this item has been flagged for removal from the backpack. */
    const bool getmarkedForDeletion() const;

private:
    /**
     * @brief Destructor. Deletes the owned @ref ItemDelegate.
     * Private — items are never deleted directly; use @ref ItemManager::deleteItem.
     */
    ~Item();

    /** @brief Whether this item is pending removal. Set by @ref ItemManager::use. */
    bool markedForDeletion;

    /** @brief The owned item delegate. Deleted in the destructor. */
    ItemDelegate* item;

    /**
     * @brief Constructs an Item taking ownership of the given delegate.
     * @param itm Delegate to wrap. Must not be null.
     */
    Item(ItemDelegate* itm);

    /** @brief Sole factory - only @ref ItemManager may construct Item instances. */
    friend class ItemManager;

    /** @brief Grants @ref PlayerCharacter access for backpack cleanup. */
    friend class PlayerCharacter;

    // friend std::ostream& operator<<(std::ostream& os, const Item& t);
};

// std::ostream& operator<<(std::ostream& os, const Item& t);

#endif
#ifndef ARMOR_H
#define ARMOR_H

#include "Item.h"

/**
 * @brief Identifies which equipment slot a piece of armor occupies.
 *
 * Used by @ref ItemManager::equip to place armor in the correct slot
 * of @ref PlayerCharacter, and by @ref SaveManager to serialise and
 * restore equipped armor.
 */
enum class ArmorSlot {
    HEAD,      ///< Helmet, hood, or headgear.
    CHEST,     ///< Chestplate, robe, or tunic.
    LEGS,      ///< Leggings, greaves, or trousers.
    FEET,      ///< Boots or sabatons.
    HANDS,     ///< Gauntlets or gloves.
    NUM_SLOTS  ///< used to size equipment arrays. Not a valid slot.
};

/**
 * @brief Represents a piece of equippable armor.
 *
 * Inherits stat storage from @ref EquipmentDelegate. The slot field
 * determines where this item is placed when equipped.
 *
 * @note Construction is restricted to @ref ItemManager via a private constructor
 * and friend declaration. Armor should never be allocated directly — always use
 * @ref ItemManager::createArmor.
 */
class Armor final : public EquipmentDelegate {
public:
    /** @brief The equipment slot this armor occupies. Set at construction and never changed. */
    ArmorSlot slot;

private:
    /**
     * @brief Constructs an Armor item with the given name, stats, and slot.
     * @param nam    Display name of the armor.
     * @param cstats Stat bonuses granted when equipped.
     * @param as     The slot this armor occupies.
     */
    Armor(std::string nam, CoreStats cstats, ArmorSlot as);

    Armor() = delete;
    Armor(const Armor&) = delete;
    Armor(const Armor&&) = delete;

    /** @brief Only @ref ItemManager may construct Armor instances. */
    friend class ItemManager;
};

#endif
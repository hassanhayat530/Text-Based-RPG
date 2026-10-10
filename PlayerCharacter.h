#ifndef PLAYERCHARACTER_H
#define PLAYERCHARACTER_H

#include "Types.h"
#include "PointWell.h"
#include "StatBlock.h"
#include "Ability.h"
#include "Buff.h"
#include "Armor.h"
#include "Potion.h"
#include "Weapon.h"
#include "Races.h"
#include <cstdint>
#include <string>
#include <memory>
#include <vector>
#include <algorithm>

/**
* @brief Player class IDs used to identify delegated classes of @ref PlayerCharacterDelegate
*/
enum class ClassID { CLERIC, WARRIOR, ROGUE, WIZARD };

/**
 * @brief Tag type used to select the load constructor of @ref PlayerCharacter.
 *
 * Passing @ref LoadTag{} to the @ref PlayerCharacter constructor signals that the
 * character is being restored from a save file. In this path, race bonuses are
 * NOT applied because they are already baked into the restored stat values.
 * The default constructor (without this tag) applies race bonuses as part of
 * normal new character construction.
 */
enum class LoadTag {};


/**
 * @brief Abstract base class for all playable character classes.
 *
 * Uses the delegate pattern. @ref PlayerCharacter holds a pointer to one of these
 * and forwards all class-specific behaviour through it. Subclasses
 * (Cleric, Warrior, Rogue, Wizard) implement @ref levelUp and @ref getClassName.
 *
 * Inherits stat storage and buff management from @ref StatBlock.
 */
class PlayerCharacterDelegate : public StatBlock {
public:
    /**
     * @brief Virtual function that is used to determine the type of delgated class.
     * @return Serialisation ID for this class. Used by @ref SaveManager.
     */
    virtual ClassID getClassID() const = 0;

    /** 
    * @brief const data used to determine the EXP required to reach level 2.
    * Also determines EXP for other levels as they scale off of this value by modifying
    * EXPToNextLevel in @ref checkIfLeveled
    */
    static const exptype LEVEL2AT = 100u;

    /**
     * @brief Default constructor of the class. Initializes data members as follows:
     * base stats inherited for @ref StatBlock as 0
     * @ref currentLevel as 1
     * @ref EXPToNextLevel as LEVEL2AT
     * @ref HP as a unique pointer held by this class with current and max set to 0.
     * Subclass constructors are expected to call @ref PCCONSTRUCT immediately
     * after to apply their base stats.
     */
    PlayerCharacterDelegate();

    virtual ~PlayerCharacterDelegate() = default;

    /**
     * @brief additive method to increase @ref currentEXP.
     * @param gainedEXP EXP to add to @ref currentEXP.
     */
    void gainEXP(exptype gainedEXP);

    /** 
     * @brief Returns the current level of this player character.
     * @return the current level.
     */
    leveltype getLevel() const;

    /**
     * @brief Returns the current EXP of this character.
     * @return the current accumulated EXP for this level.
     */
    exptype getCurrentEXP() const;

    /** 
     * @brief Returns the EXP required to level up.
     * @return EXP needed to reach next level.
     */
    exptype getEXPtoNextLevel() const;

    /**
     * @brief Return the active buffs applied to this character.
     * @return Copy of the active buff list.
     */
    std::vector<Buff> getBuffList() const;

    /** @brief Calls @ref addNewBuff from inherited class @ref StatBlock. */
    void applyBuff(Buff b);

    /** 
     * @brief Applies race bonus stats and increases HP/AP pools accordingly.
     *
     * Called once during normal character construction by @ref PlayerCharacter.
     * Must NOT be called when loading from a save file. Use the @ref LoadTag in such case
     * 
     * @param bonusHP  Hit point pool increase
     * @param bonusAP  Ability point pool increase
     * @param bonusStr Strength increase
     * @param bonusInt Intellect increase
     * @param bonusAgi Agility increase
     */
    void applyRaceBonuses(welltype bonusHP, welltype bonusAP, stattype bonusStr, stattype bonusInt, stattype bonusAgi);

    /**
     * @brief Applied when the character levels up.
     *
     * Subclasses implement this using the @ref LEVELUP macro to scale HP, AP,
     * and stats by half their base values.
     */
    virtual void levelUp() = 0;

    /** @return Display name of this class (e.g. "Cleric"). */
    virtual std::string getClassName() = 0;

    /** 
     * @brief checks if accumulated EXP is more than or equal to the EXP needed to level up.
     * 
     * If true, increases level by 1, runs the @ref levelUp() method and updates the EXP needed to level up
     * using a static const scalar present at the start of this method.
     * @return true if levelled up, otherwise false.
     */
    bool checkIfLeveled();

    /** @brief Hit point pool. Direct access is intentional for subclass construction. */
    std::unique_ptr<PointWell> HP;

    /** @brief Ability point pool. */
    std::unique_ptr<PointWell> AP;

    /** @brief All abilities available to this character. Populated by subclass constructors and @ref levelUp. */
    std::vector<Ability> abilities;

    /** @brief Active buffs. Managed via @ref applyBuff and @ref tickBuffs. */
    std::vector<Buff> buffs;

    /**
     * @brief Restores all stats from serialised save data.
     *
     * Bypasses normal construction logic: directly sets HP, AP, level, EXP,
     * and base stats. Should only be called by @ref SaveManager during loading,
     * before passing the delegate to the @ref LoadTag constructor of @ref PlayerCharacter.
     * @param currentHP Current HP at time of save.
     * @param maxHP     Maximum HP at time of save.
     * @param currentAP Current AP at time of save.
     * @param maxAP     Maximum AP at time of save.
     * @param level     Character level at time of save.
     * @param exp       Accumulated EXP at time of save.
     * @param expToNext EXP require to level up at time of save.
     * @param str       Base strength at time of save.
     * @param intel     Base intellect at time of save.
     * @param agi       Base agility at time of save.
     * @param arm       Base armor at time of save.
     * @param elres     Base element resistance at time of save.
     */
    void restoreStats(
        welltype currentHP, welltype maxHP,
        welltype currentAP, welltype maxAP,
        leveltype level, exptype exp, exptype expToNext,
        stattype str, stattype intel, stattype agi,
        stattype arm, stattype elres);

protected:
    /** @brief the current level of the character*/
    leveltype currentLevel;

    /** @brief the current EXP of the character*/
    exptype currentEXP;

    /**
     * @brief the exp required to level up the character
     * Updated in the @ref checkIfLeveled whenever the player levels up by a static const scalar
     */
    exptype EXPToNextLevel;
};

/**
 * @brief Macro that initialises a subclass delegate's HP, AP, and base stats.
 *
 * Must be called at the end of every @ref PlayerCharacterDelegate subclass constructor.
 * Requires the subclass to define BASEHP, BASEAP, BASESTR, BASEINT, BASEAGI constants.
 */
#define PCCONSTRUCT \
HP->setMax(BASEHP);\
HP->increaseCurrent(BASEHP);\
if (AP) { \
    AP->setMax(BASEAP); \
    AP->increaseCurrent(BASEAP); \
}\
increaseStats(BASESTR, BASEINT, BASEAGI);

 /**
  * @brief Macro that scales HP, AP, and stats on level-up.
  *
  * Called inside each subclass's @ref levelUp implementation.
  * Increases max and current pools by half the base value, and stats by half
  * their base value rounded up.
  */
#define LEVELUP \
HP->setMax((welltype)((BASEHP / 2.f) + HP->getMax()));\
HP->increaseCurrent((welltype)(BASEHP / 2.f));\
if (AP) {\
    AP->setMax((welltype)((BASEAP / 2.f) + AP->getMax()));\
    AP->increaseCurrent((welltype)(BASEAP / 2.f));\
}\
increaseStats((stattype)((BASESTR + 1u) / 2.f), (stattype)((BASEINT + 1u) / 2.f), (stattype)((BASEAGI + 1u) / 2.f));

/**
 * @brief The player's character: owns a class delegate and a race.
 *
 * Acts as the stable public interface for all character data. Class-specific
 * behaviour is delegated to @ref PlayerCharacterDelegate. Race bonuses are
 * applied once at construction.
 *
 * Inventory management (backpack and equipped items) is handled here with
 * @ref ItemManager as a friend class for direct access.
 *
 * @note This class is non-copyable and non-movable by design. it must always
 * be held by pointer or as a value member of @ref Player.
 */
class PlayerCharacter {
private:
    std::unique_ptr<PlayerCharacterDelegate> pcClass;
    std::unique_ptr<Race> race_;
    
    Item* equippedArmors[static_cast<unsigned long long>(ArmorSlot::NUM_SLOTS)];
    Item* equippedWeapons[static_cast<unsigned long long>(WeaponSlot::NUM_SLOTS)];
    std::vector<Item*> backpack;

    /**
     * @brief Removes and deletes all backpack items marked for deletion.
     * Called automatically by @ref ItemManager::use when a potion is consumed.
     */
    void cleanUpBackpack();

    /** @brief Grants ItemManager direct access to backpack and equipment slots. */
    friend class ItemManager;

public:
    /**
     * @brief Normal constructor — applies race bonuses on top of class base stats.
     * @param pc   Class delegate (e.g. Cleric, Warrior). Ownership transferred.
     * @param race Race instance. Ownership transferred.
     */
    PlayerCharacter(std::unique_ptr<PlayerCharacterDelegate> pc, std::unique_ptr<Race> race);

    /**
     * @brief Load constructor — skips race bonus application.
     *
     * Used by @ref SaveManager when restoring a character whose stats already
     * include race bonuses from a previous save.
     * @param pc   Class delegate with stats pre-restored via @ref restoreStats.
     * @param race Race instance. Ownership transferred.
     */
    PlayerCharacter(std::unique_ptr<PlayerCharacterDelegate> pc, std::unique_ptr<Race> race, LoadTag);
    ~PlayerCharacter();

    // Deleted constructors
    PlayerCharacter() = delete;
    PlayerCharacter(const PlayerCharacter&) = delete;
    PlayerCharacter(const PlayerCharacter&&) = delete;

    // Getters

    /** @return Display name of the character's class. */
    const std::string getClassName() const noexcept;

    /** @return Serialisation ID of the character's class. */
    const ClassID getClassID() const noexcept{ return pcClass->getClassID(); }

    /** @return Display name of the character's race. */
    const std::string getRaceName() const noexcept { return race_->getRaceName(); }

    /** @return Serialisation ID of the character's race. */
    const RaceID getRaceID() const noexcept { return race_->getRaceID(); }

    /** @return Current character level. */
    const leveltype getLevel() const noexcept;

    /** @return Current accumulated EXP. */
    const exptype getCurrentEXP() const noexcept;

    /** @return EXP required to reach the next level. */
    const exptype getEXPToNextLevel() const noexcept;

    /** @return Current HP. */
    const welltype getCurrentHP() const noexcept;

    /** @return Maximum HP. */
    const welltype getMaxHP() const noexcept;
    /** @return Current AP. Returns 0 if the class has no AP pool. */
    const welltype getCurrentAP() const noexcept;

    /** @return Maximum AP. Returns 0 if the class has no AP pool. */
    const welltype getMaxAP() const noexcept;

    /** @return Base strength before equipment and buff contributions. */
    const stattype getBaseStrength() const noexcept;

    /** @return Base intellect before equipment and buff contributions. */
    const stattype getBaseIntellect() const noexcept;

    /** @return Base agility before equipment and buff contributions. */
    const stattype getBaseAgility() const noexcept;

    /** @return Base armor before equipment contributions. */
    const stattype getBaseArmor() const noexcept;

    /** @return Base element resistance before equipment contributions. */
    const stattype getBaseElementRes() const noexcept;

    /**
     * @return Total strength including equipment bonuses and active buffs.
     * Used for melee damage calculation and ability scaling.
     */
    const stattype getTotalStrength() const noexcept;

    /** @return Total intellect including equipment bonuses and active buffs. */
    const stattype getTotalIntellect() const noexcept;

    /**
     * @return Total agility including equipment bonuses and active buffs.
     * Used for hit chance and flee chance calculations.
     */
    const stattype getTotalAgility() const noexcept;

    /**
     * @return Total armor including all equipped armor slot contributions.
     * Subtracted from incoming physical damage in @ref CombatController.
     */
    const stattype getTotalArmor() const noexcept;

    /**
     * @return Total element resistance including all equipped armor contributions.
     * Subtracted from incoming magic/ability damage in @ref CombatController.
     */
    const stattype getTotalElementRes() const noexcept;

    /** @return Reference to the ability list. Returned by reference to avoid copying @ref Ability objects with raw Buff pointers. */
    const std::vector<Ability>& getAbilityList() const noexcept;

    /** @return Copy of the active buff list. */
    const std::vector<Buff> getBuffList() const noexcept;

    /** @return Reference to the backpack item list. */
    const std::vector<Item*>& getBackpackList() const noexcept;

    /**
     * @return Pointer to the equipped armor in slot i, or nullptr if the slot is empty.
     * @param i Slot index cast from @ref ArmorSlot.
     */
    const Armor* getEquippedArmorAt(unsigned long long i) const noexcept;

    /**
     * @return Pointer to the equipped weapon in slot i, or nullptr if the slot is empty.
     * @param i Slot index cast from @ref WeaponSlot.
     */
    const Weapon* getEquippedWeaponAt(unsigned long long i) const noexcept;

    /**
     * @brief Calculates melee attack damage.
     *
     * Returns equipped melee weapon damage plus one quarter of total strength.
     * Returns strength-only damage if no melee weapon is equipped.
     * @return Total melee damage value before armor reduction.
     */
    const damagetype meleeAttack() const noexcept;

    /**
     * @brief Calculates ranged attack damage.
     *
     * Returns equipped ranged weapon damage plus one quarter of total agility.
     * Returns 0 if no ranged weapon is equipped.
     * @return Total ranged damage value before armor reduction, or 0.
     */
    const damagetype rangedAttack() const noexcept;

    // Modifiers

    /**
     * @brief Manually triggers a level-up check.
     * Calls the @ref PlayerCharacterDelegate::checkIfLeveled recurrently until the player stops
     * levelling up in case the accumulated EXP is enough for multiple levels.
     * Called by @ref Town when the player sleeps.
     */
    void levelUp();
    
    /** @brief Adds EXP to accumulation. */
    void gainEXP(exptype amount);

    /** @brief Reduces HP. HP will not drop below 0. */
    void takeDamage(welltype amount);

    /**
     * @brief Reduces AP by the given amount.
     * @note Does nothing if the class has no AP pool.
     */
    void spendAP(welltype amount);

    /** @brief Increases current HP up to max. */
    void heal(welltype amount);

    /**
     * @brief Increases current AP up to max.
     * @note Does nothing if the class has no AP pool.
     */
    void regenerateAP(welltype amount);

    /** @brief Applies a buff to the character's stat block. */
    void applyBuff(Buff buff);

    /**
     * @brief Appends an ability to the ability list.
     * Used by @ref SaveManager when restoring saved abilities.
     * @param ability Ability to add. Moved to avoid copying the raw Buff pointer.
     */
    void addAbility(Ability ability);

    /**
     * @brief Clears all abilities from the list.
     * Called by @ref SaveManager before loading saved abilities to prevent
     * duplicating abilities that were granted by the class constructor.
     */
    void clearAbilities();

    /**
     * @brief Decrements all active buff durations and removes any that have expired.
     * Should be called once per combat round at the end of @ref CombatController::showMonsterTurn.
     */
    void tickBuffs();
};

#endif
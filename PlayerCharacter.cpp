#include "PlayerCharacter.h"
#include "Random.h"

// ----- PlayerCharacterDelegate implementation ---------------------------------
PlayerCharacterDelegate::PlayerCharacterDelegate()
    : StatBlock(0u, 0u), currentLevel(1u), currentEXP(0u), EXPToNextLevel(LEVEL2AT) {
    HP = std::make_unique<PointWell>(1u, 1u);
}

void PlayerCharacterDelegate::gainEXP(exptype gainedEXP) {
    currentEXP += gainedEXP;
}

leveltype PlayerCharacterDelegate::getLevel() const {
    return currentLevel;
}

exptype PlayerCharacterDelegate::getCurrentEXP() const {
    return currentEXP;
}

exptype PlayerCharacterDelegate::getEXPtoNextLevel() const {
    return EXPToNextLevel;
}

std::vector<Buff> PlayerCharacterDelegate::getBuffList() const {
    return buffs;
}

void PlayerCharacterDelegate::applyBuff(Buff b) {
    addNewBuff(b);
}

void PlayerCharacterDelegate::applyRaceBonuses(welltype bonusHP, welltype bonusMP,
    stattype bonusStr, stattype bonusInt, stattype bonusAgi) {
    HP->setMax(HP->getMax() + bonusHP);
    HP->increaseCurrent(bonusHP);
    if (AP) {
        AP->setMax(AP->getMax() + bonusMP);
        AP->increaseCurrent(bonusMP);
    }
    increaseStats(bonusStr, bonusInt, bonusAgi);
}

bool PlayerCharacterDelegate::checkIfLeveled() {
    static const leveltype LEVELSCALAR = 2u;
    if (currentEXP >= EXPToNextLevel) {
        ++currentLevel;
        levelUp();
        EXPToNextLevel *= LEVELSCALAR;
        return true;
    }
    return false;
}

void PlayerCharacterDelegate::restoreStats(
    welltype currentHP, welltype maxHP,
    welltype currentMP, welltype maxMP,
    leveltype level, exptype exp, exptype expToNext,
    stattype str, stattype intel, stattype agi,
    stattype arm, stattype elres)
{
    currentLevel = level;
    currentEXP = exp;
    EXPToNextLevel = expToNext;

    HP->setMax(maxHP);
    HP->setCurrent(currentHP);

    if (AP) {
        AP->setMax(maxMP);
        AP->setCurrent(currentMP);
    }

    setBaseStats(str, intel, agi, arm, elres);
}

// ----- PlayerCharacter --------------------------------------------------------
PlayerCharacter::PlayerCharacter(std::unique_ptr<PlayerCharacterDelegate> pc, std::unique_ptr<Race> race) : pcClass(std::move(pc)), race_(std::move(race)) {
    // Apply race bonuses on top of class base stats
    pcClass->applyRaceBonuses(
        race_->getBonusHP(),
        race_->getBonusAP(),
        race_->getBonusStr(),
        race_->getBonusInt(),
        race_->getBonusAgi()
    );

    // initialize pointers
    for (auto i = 0u; i < static_cast<unsigned long long>(ArmorSlot::NUM_SLOTS); ++i)
        equippedArmors[i] = nullptr;
    for (auto i = 0u; i < static_cast<unsigned long long>(WeaponSlot::NUM_SLOTS); ++i)
        equippedWeapons[i] = nullptr;
}

PlayerCharacter::PlayerCharacter(std::unique_ptr<PlayerCharacterDelegate> pc, std::unique_ptr<Race> race, LoadTag) : pcClass(std::move(pc)), race_(std::move(race)) {
    // initialize pointers
    for (auto i = 0u; i < static_cast<unsigned long long>(ArmorSlot::NUM_SLOTS); ++i)
        equippedArmors[i] = nullptr;
    for (auto i = 0u; i < static_cast<unsigned long long>(WeaponSlot::NUM_SLOTS); ++i)
        equippedWeapons[i] = nullptr;
}

PlayerCharacter::~PlayerCharacter() {
    for (auto i = 0u; i < static_cast<unsigned long long>(ArmorSlot::NUM_SLOTS); ++i) {
        delete equippedArmors[i];
        equippedArmors[i] = nullptr;
    }
    for (auto i = 0u; i < static_cast<unsigned long long>(WeaponSlot::NUM_SLOTS); ++i) {
        delete equippedWeapons[i];
        equippedWeapons[i] = nullptr;
    }
}

void PlayerCharacter::cleanUpBackpack() {
    std::vector<Item*> toDelete;
    for (Item* i : backpack)
        if (i->getmarkedForDeletion())
            toDelete.push_back(i);

    backpack.erase(std::remove_if(backpack.begin(), backpack.end(),
        [](const Item* i) { return i->getmarkedForDeletion(); }),
        backpack.end());

    for (Item* i : toDelete)
        delete i;
}

// ----- Simple getters ---------------------------------------------------------
const std::string PlayerCharacter::getClassName() const noexcept { return pcClass->getClassName(); }
const leveltype PlayerCharacter::getLevel() const noexcept { return pcClass->getLevel(); }
const exptype PlayerCharacter::getCurrentEXP() const noexcept { return pcClass->getCurrentEXP(); }
const exptype PlayerCharacter::getEXPToNextLevel() const noexcept { return pcClass->getEXPtoNextLevel(); }
const welltype PlayerCharacter::getCurrentHP() const noexcept { return pcClass->HP->getCurrent(); }
const welltype PlayerCharacter::getMaxHP() const noexcept { return pcClass->HP->getMax(); }
const welltype PlayerCharacter::getCurrentAP() const noexcept {
    return pcClass->AP ? pcClass->AP->getCurrent() : 0;
}
const welltype PlayerCharacter::getMaxAP() const noexcept {
    return pcClass->AP ? pcClass->AP->getMax() : 0;
}
const stattype PlayerCharacter::getBaseStrength() const noexcept { return pcClass->getBaseStrength(); }
const stattype PlayerCharacter::getBaseIntellect() const noexcept { return pcClass->getBaseIntellect(); }
const stattype PlayerCharacter::getBaseAgility() const noexcept { return pcClass->getBaseAgility(); }
const stattype PlayerCharacter::getBaseArmor() const noexcept { return pcClass->getBaseArmor(); }
const stattype PlayerCharacter::getBaseElementRes() const noexcept { return pcClass->getBaseElementRes(); }

// ----- Total stat calculations ------------------------------------------------
const stattype PlayerCharacter::getTotalStrength() const noexcept {
    stattype strFromArmor = 0;
    for (auto i = 0u; i < static_cast<unsigned long long>(ArmorSlot::NUM_SLOTS); ++i) {
        if (equippedArmors[i]) {
            Armor* armor = dynamic_cast<Armor*>(equippedArmors[i]->item);
            strFromArmor += armor->stats.strength;
        }
    }
    stattype strFromWeapons = 0;
    for (auto i = 0u; i < static_cast<unsigned long long>(WeaponSlot::NUM_SLOTS); ++i) {
        if (equippedWeapons[i]) {
            Weapon* weapon = dynamic_cast<Weapon*>(equippedWeapons[i]->item);
            strFromWeapons += weapon->stats.strength;
        }
    }
    return pcClass->getTotalStrength() + strFromArmor + strFromWeapons;
}

const stattype PlayerCharacter::getTotalIntellect() const noexcept {
    stattype intFromArmor = 0;
    for (auto i = 0u; i < static_cast<unsigned long long>(ArmorSlot::NUM_SLOTS); ++i) {
        if (equippedArmors[i]) {
            Armor* armor = dynamic_cast<Armor*>(equippedArmors[i]->item);
            intFromArmor += armor->stats.intellect;
        }
    }
    stattype intFromWeapons = 0;
    for (auto i = 0u; i < static_cast<unsigned long long>(WeaponSlot::NUM_SLOTS); ++i) {
        if (equippedWeapons[i]) {
            Weapon* weapon = dynamic_cast<Weapon*>(equippedWeapons[i]->item);
            intFromWeapons += weapon->stats.intellect;
        }
    }
    return pcClass->getTotalIntellect() + intFromArmor + intFromWeapons;
}

const stattype PlayerCharacter::getTotalAgility() const noexcept {
    stattype agiFromArmor = 0;
    for (auto i = 0u; i < static_cast<unsigned long long>(ArmorSlot::NUM_SLOTS); ++i) {
        if (equippedArmors[i]) {
            Armor* armor = dynamic_cast<Armor*>(equippedArmors[i]->item);
            agiFromArmor += armor->stats.agility;
        }
    }
    stattype agiFromWeapons = 0;
    for (auto i = 0u; i < static_cast<unsigned long long>(WeaponSlot::NUM_SLOTS); ++i) {
        if (equippedWeapons[i]) {
            Weapon* weapon = dynamic_cast<Weapon*>(equippedWeapons[i]->item);
            agiFromWeapons += weapon->stats.agility;
        }
    }
    return pcClass->getTotalAgility() + agiFromArmor + agiFromWeapons;
}

const stattype PlayerCharacter::getTotalArmor() const noexcept {
    stattype armorFromArmor = 0;
    for (auto i = 0u; i < static_cast<unsigned long long>(ArmorSlot::NUM_SLOTS); ++i) {
        if (equippedArmors[i]) {
            Armor* armor = dynamic_cast<Armor*>(equippedArmors[i]->item);
            armorFromArmor += armor->stats.armor;
        }
    }
    return pcClass->getTotalArmor() + armorFromArmor;
}

const stattype PlayerCharacter::getTotalElementRes() const noexcept {
    stattype resistFromArmor = 0;
    for (auto i = 0u; i < static_cast<unsigned long long>(ArmorSlot::NUM_SLOTS); ++i) {
        if (equippedArmors[i]) {
            Armor* armor = dynamic_cast<Armor*>(equippedArmors[i]->item);
            resistFromArmor += armor->stats.elementRes;
        }
    }
    return pcClass->getTotalElementRes() + resistFromArmor;
}

const std::vector<Ability>& PlayerCharacter::getAbilityList() const noexcept {
    return pcClass->abilities;
}

const std::vector<Buff> PlayerCharacter::getBuffList() const noexcept {
    return pcClass->getBuffList();
}

const std::vector<Item*>& PlayerCharacter::getBackpackList() const noexcept {
    return backpack;
}

const Armor* PlayerCharacter::getEquippedArmorAt(unsigned long long i) const noexcept {
    return equippedArmors[i] ? dynamic_cast<Armor*>(equippedArmors[i]->item) : nullptr;
}

const Weapon* PlayerCharacter::getEquippedWeaponAt(unsigned long long i) const noexcept {
    return equippedWeapons[i] ? dynamic_cast<Weapon*>(equippedWeapons[i]->item) : nullptr;
}

const damagetype PlayerCharacter::meleeAttack() const noexcept {
    damagetype tmpDamageDone = 0;

    const Weapon* equippedWeapon = getEquippedWeaponAt((unsigned long long)WeaponSlot::MELEE);

    // if weapon exists get the damage, else the base damage stays 0
    if (equippedWeapon) {
        tmpDamageDone = randomInRange(equippedWeapon->minDamage, equippedWeapon->maxDamage);
    }

    // add 1/4 of str as bonus melle damage
    tmpDamageDone += damagetype(getTotalStrength() / 4.f);

    return tmpDamageDone;
}

const damagetype PlayerCharacter::rangedAttack() const noexcept {
    damagetype tmpDamageDone = 0;

    const Weapon* equippedWeapon = getEquippedWeaponAt((unsigned long long)WeaponSlot::RANGED);

    // if weapon exists get the damage, else the base damage stays 0
    if (equippedWeapon) {
        tmpDamageDone = randomInRange(equippedWeapon->minDamage, equippedWeapon->maxDamage);
        tmpDamageDone += damagetype(getTotalAgility() / 4.f);  // add 1/4 of agi as bonus ranged damage
    }

    return tmpDamageDone;
}

// ----- Modifiers -------------------------------------------------------------
void PlayerCharacter::levelUp() {
    while (pcClass->checkIfLeveled()) {}
}
void PlayerCharacter::gainEXP(exptype amount) { pcClass->gainEXP(amount); }
void PlayerCharacter::takeDamage(welltype amount) { pcClass->HP->reduceCurrent(amount); }
void PlayerCharacter::spendAP(welltype amount) {
    if (pcClass->AP)
        pcClass->AP->reduceCurrent(amount);
}
void PlayerCharacter::heal(welltype amount) { pcClass->HP->increaseCurrent(amount); }
void PlayerCharacter::regenerateAP(welltype amount) { pcClass->AP->increaseCurrent(amount); }
void PlayerCharacter::applyBuff(Buff buff) { pcClass->applyBuff(buff); }
void PlayerCharacter::addAbility(Ability ability) { pcClass->abilities.push_back(ability); }
void PlayerCharacter::clearAbilities() { pcClass->abilities.clear(); }
void PlayerCharacter::tickBuffs() { pcClass->tickBuffs(); }
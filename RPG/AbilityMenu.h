#ifndef ABILITYMENU_H
#define ABILITYMENU_H

#include "Player.h"
#include "Monster.h"
#include <unordered_map>
#include <string>

class AbilityMenu {
public:
    // Outside combat - view only, no monster target - not really utilized but for future possibility
    AbilityMenu(Player* player);

    // Inside combat - abilities can be used - sets inCombat_ to true
    AbilityMenu(Player* player, Monster* monster,
        std::unordered_map<std::string, int>& cooldowns);

    enum class Result { NONE, ABILITY_USED, BACK };
    Result run();

private:
    enum class Page { ABILITY_LIST, ABILITY_DETAIL };

    Page   currentPage = Page::ABILITY_LIST;
    bool   done = false;
    bool   inCombat_ = false;
    size_t selectedIndex = 0;

    Player* player_;
    Monster* monster_;  // nullptr when outside combat

    // Reference to CombatController's cooldown table so state persists
    // across turns. Uses ability name as key, value = turns remaining.
    std::unordered_map<std::string, int>* cooldowns_ = nullptr;

    // map used when outside combat (view-only mode)
    std::unordered_map<std::string, int> dummyCooldowns_;

    Result result_ = Result::BACK;

    void showAbilityList();
    void showAbilityDetail();

    // Returns remaining cooldown for an ability, 0 = ready
    int  getCooldown(const Ability& ability) const;
    bool hasSufficientMP(const Ability& ability) const;

    // Resolves and applies the ability — returns true if it was used
    bool useAbility(const Ability& ability);

    stattype getScalarValue(AbilityScalar scalar) const;
};

#endif
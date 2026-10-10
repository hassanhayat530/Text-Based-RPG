#ifndef COMBATCONTROLLER_H
#define COMBATCONTROLLER_H

#include "Player.h"
#include "Monster.h"
#include "AbilityMenu.h"
#include <unordered_map>
#include <memory>
#include <string>

class CombatController {
public:
    enum class Outcome { WON, LOST, FLED };

    CombatController(Player* player, std::unique_ptr<Monster> monster);
    Outcome run();

private:
    enum class Page {
        PLAYER_TURN,
        MONSTER_TURN,
        RESULT
    };

    Page currentPage = Page::PLAYER_TURN;
    bool done = false;
    Outcome outcome = Outcome::WON;

    Player* player_;
    std::unique_ptr<Monster> monster_;

    // Stores names of buffs that expired this round for display
    std::vector<std::string> expiredPlayerBuffs;
    std::vector<std::string> expiredMonsterBuffs;

    void tickBuffs(); // ticks both combatants, records expirations

    // Running combat log shown each round
    std::string lastPlayerAction;
    std::string lastMonsterAction;

    // Tracks remaining cooldown turns per ability name
    std::unordered_map<std::string, int> cooldowns;

    // Pages
    void showPlayerTurn();
    void showMonsterTurn();
    void showResult();

    // Helpers
    void printCombatStatus() const;
    bool rollHit(stattype attackerAgi, stattype defenderAgi) const;

    damagetype calculateMonsterDamage() const;

    void showUseItem();

    void updateCooldowns(); // call at end of each round
    void showUseAbility();
};

#endif
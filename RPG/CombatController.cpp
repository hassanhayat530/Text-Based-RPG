#include "CombatController.h"
#include "Utils.h"
#include "Inventory.h"
#include "Random.h"
#include <iostream>
#include <cstdlib>
#include <sstream>

CombatController::CombatController(Player* player, std::unique_ptr<Monster> monster) : player_(player), monster_(std::move(monster)) {}

CombatController::Outcome CombatController::run() {
    while (!done) {
        clearScreen();
        showTop(player_);
        printCombatStatus();
        switch (currentPage) {
        case Page::PLAYER_TURN:  showPlayerTurn();  break;
        case Page::MONSTER_TURN: showMonsterTurn(); break;
        case Page::RESULT:       showResult();      break;
        }
    }
    return outcome;
}

// ---- Pages ------------------------------------------------------------------

void CombatController::showPlayerTurn() {
    std::cout << "\n--- Your Turn ---\n"
        << "1 = Melee Attack\n";

    if (player_->us.rangedAttack() > 0)
        std::cout << "2 = Ranged Attack\n";

    std::cout << "3 = Use Ability\n"
        << "4 = Use Item\n"
        << "5 = Flee\n";

    char c = getchar();
    flushInput();

    damagetype dmg = 0;

    switch (c) {
    case '1': {
        if (rollHit(player_->us.getTotalAgility(), monster_->getTotalAgility())) {
            dmg = player_->us.meleeAttack();

            // Reduce by monster armor (minimum 1 damage)
            damagetype reduced = dmg > monster_->getTotalArmor()
                ? dmg - monster_->getTotalArmor() : 1;

            monster_->HP->reduceCurrent(reduced);

            std::ostringstream ss;
            ss << "You attacked " << monster_->name
                << " for " << reduced << " damage.";
            lastPlayerAction = ss.str();
        }
        else {
            lastPlayerAction = "Your attack missed!";
        }
        currentPage = Page::MONSTER_TURN;
        break;
    }
    case '2': {
        damagetype ranged = player_->us.rangedAttack();
        if (ranged == 0) { wrongInput(); return; }

        if (rollHit(player_->us.getTotalAgility(), monster_->getTotalAgility())) {
            damagetype reduced = ranged > monster_->getTotalArmor()
                ? ranged - monster_->getTotalArmor() : 1;
            monster_->HP->reduceCurrent(reduced);

            std::ostringstream ss;
            ss << "You fired a ranged attack at " << monster_->name
                << " for " << reduced << " damage.";
            lastPlayerAction = ss.str();
        }
        else {
            lastPlayerAction = "Your ranged attack missed!";
        }
        currentPage = Page::MONSTER_TURN;
        break;
    }
    case '3':
        showUseAbility();
        break;
    case '4':
        showUseItem();
        // After using an item it stays on PLAYER_TURN so the player
        // still takes their action this round
        break;
    case '5': {
        // Flee chance: player agi vs monster agi, minimum 20% chance
        int playerAgi = player_->us.getTotalAgility();
        int monsterAgi = monster_->getTotalAgility();
        int fleeChance = 50 + ((playerAgi - monsterAgi) * 5);
        if (fleeChance < 20)  fleeChance = 20;
        if (fleeChance > 90)  fleeChance = 90;

        if (chance(fleeChance)) {
            lastPlayerAction = "You successfully fled!";
            outcome = Outcome::FLED;
            currentPage = Page::RESULT;
        }
        else {
            lastPlayerAction = "You failed to flee!";
            currentPage = Page::MONSTER_TURN;
        }
        break;
    }
    default:
        wrongInput();
        break;
    }

    // Check if monster died from player's attack
    if (!monster_->isAlive() && currentPage != Page::RESULT) {
        outcome = Outcome::WON;
        currentPage = Page::RESULT;
    }
}

void CombatController::showMonsterTurn() {
    damagetype dmg = calculateMonsterDamage();

    if (rollHit(monster_->getTotalAgility(), player_->us.getTotalAgility())) {
        // Reduce by player armor (minimum 1 damage)
        damagetype reduced = dmg > player_->us.getTotalArmor()
            ? dmg - player_->us.getTotalArmor() : 1;

        player_->us.takeDamage(reduced);

        std::ostringstream ss;
        ss << monster_->name << " attacked you for " << reduced << " damage.";
        lastMonsterAction = ss.str();
    }
    else {
        lastMonsterAction = monster_->name + "'s attack missed!";
    }

    updateCooldowns();
    tickBuffs();

    if (!player_->IsAlive()) {
        outcome = Outcome::LOST;
        currentPage = Page::RESULT;
        return;
    }

    currentPage = Page::PLAYER_TURN;
}

void CombatController::tickBuffs() {
    // Snapshot active buff names before ticking so we can report expirations
    auto snapshot = [](const std::vector<Buff>& buffs) {
        std::vector<std::string> names;
        for (const auto& b : buffs) names.push_back(b.name);
        return names;
        };

    auto playerBuffsBefore = snapshot(player_->us.getBuffList());
    auto monsterBuffsBefore = snapshot(monster_->getBuffList());

    // Tick both combatants
    player_->us.tickBuffs();
    monster_->tickBuffs();

    auto playerBuffsAfter = snapshot(player_->us.getBuffList());
    auto monsterBuffsAfter = snapshot(monster_->getBuffList());

    expiredPlayerBuffs.clear();
    expiredMonsterBuffs.clear();

    // Find which names disappeared — those are the expired ones
    auto findExpired = [](const std::vector<std::string>& before,
        const std::vector<std::string>& after,
        std::vector<std::string>& expired) {
            for (const auto& name : before) {
                if (std::find(after.begin(), after.end(), name) == after.end())
                    expired.push_back(name);
            }
        };

    findExpired(playerBuffsBefore, playerBuffsAfter, expiredPlayerBuffs);
    findExpired(monsterBuffsBefore, monsterBuffsAfter, expiredMonsterBuffs);
}

void CombatController::showResult() {
    clearScreen();
    showTop(player_);

    switch (outcome) {
    case Outcome::WON:
        std::cout << "=== Victory! ===\n\n"
            << "You defeated the " << monster_->name << "!\n"
            << "You gained " << monster_->expReward << " EXP.\n\n";
        player_->us.gainEXP(monster_->expReward);
        break;
    case Outcome::LOST:
        std::cout << "=== Defeated ===\n\n"
            << "You were slain by the " << monster_->name << "...\n\n";
        break;
    case Outcome::FLED:
        std::cout << "=== Escaped ===\n\n"
            << "You fled from the " << monster_->name << ".\n\n";
        break;
    }

    std::cout << "Press any key to continue.\n";
    char _ = getchar();
    flushInput();
    done = true;
}

// ---- Helpers ----------------------------------------------------------------

void CombatController::printCombatStatus() const {
    std::cout << "--------- Combat ---------\n"
        << "You:           HP " << player_->us.getCurrentHP()
        << " / " << player_->us.getMaxHP();

    // Show active player buffs inline
    const auto& playerBuffs = player_->us.getBuffList();
    if (!playerBuffs.empty()) {
        std::cout << "  [";
        for (size_t i = 0; i < playerBuffs.size(); ++i) {
            const auto& b = playerBuffs[i];
            std::cout << (b.isDebuff ? "-" : "+") << b.name
                << "(" << b.duration << ")";
            if (i + 1 < playerBuffs.size()) std::cout << ", ";
        }
        std::cout << "]";
    }
    std::cout << "\n";

    std::cout << monster_->name << ":  HP " << monster_->HP->getCurrent()
        << " / " << monster_->HP->getMax();

    // Show active monster buffs inline
    const auto& monsterBuffs = monster_->getBuffList();
    if (!monsterBuffs.empty()) {
        std::cout << "  [";
        for (size_t i = 0; i < monsterBuffs.size(); ++i) {
            const auto& b = monsterBuffs[i];
            std::cout << (b.isDebuff ? "-" : "+") << b.name
                << "(" << b.duration << ")";
            if (i + 1 < monsterBuffs.size()) std::cout << ", ";
        }
        std::cout << "]";
    }
    std::cout << "\n--------------------------\n";

    // Last round actions
    if (!lastPlayerAction.empty())  std::cout << lastPlayerAction << "\n";
    if (!lastMonsterAction.empty()) std::cout << lastMonsterAction << "\n";

    // Buff expirations from last tick
    for (const auto& name : expiredPlayerBuffs)
        std::cout << "[" << name << " wore off]\n";
    for (const auto& name : expiredMonsterBuffs)
        std::cout << "[" << monster_->name << "'s " << name << " wore off]\n";

    std::cout << '\n';
}

bool CombatController::rollHit(stattype attackerAgi, stattype defenderAgi) const {
    // Base 70% hit chance, modified by agility difference
    // Each point of agility difference shifts chance by 3%
    int hitChance = 70 + ((int)attackerAgi - (int)defenderAgi) * 3;
    if (hitChance < 10)  hitChance = 10;  // always at least 10% chance to hit
    if (hitChance > 95)  hitChance = 95;  // never a guaranteed hit
    return chance(hitChance);
}

damagetype CombatController::calculateMonsterDamage() const {
    // Random roll between monster min and max damage
    return randomInRange(monster_->minDamage, monster_->maxDamage);
}

void CombatController::showUseItem() {
    clearScreen();
    showTop(player_);

    const auto& backpack = player_->us.getBackpackList();

    std::cout << "--- Use Item ---\n";

    // Only show potions
    bool hasPotions = false;
    for (size_t i = 0; i < backpack.size(); ++i) {
        const Potion* potion = dynamic_cast<const Potion*>(backpack[i]->getItem());
        if (potion && potion->quantity > 0) {
            std::cout << "[" << i + 1 << "] " // Start from 1 (add 1 to index)
                << potion->name
                << " (Heals " << potion->healAmount << " HP)"
                << " x" << potion->quantity << "\n";
            hasPotions = true;
        }
    }

    if (!hasPotions) {
        std::cout << "No usable items.\n"
            << "Press any key to go back.\n";
        char _ = getchar();
        flushInput();
        return;
    }

    std::cout << "b = back\n";

    char c = getchar();
    flushInput();

    if (c == 'b') return;

    size_t index = static_cast<size_t>(c - '1');  // revert to proper index through subtracting 1
    if (index >= backpack.size()) { wrongInput(); return; }  // capture bad input

    const Potion* potion = dynamic_cast<const Potion*>(backpack[index]->getItem());
    if (!potion || potion->quantity <= 0) { wrongInput(); return; }  // should not show but extra check

    player_->us.heal(potion->healAmount);
    ItemManager::use(backpack[index], &player_->us);

    std::cout << "Used " << potion->name
        << ". Healed " << potion->healAmount << " HP.\n"
        << "Press any key to continue.\n";
    char _ = getchar();
    flushInput();
}

void CombatController::updateCooldowns() {
    for (auto& [name, turns] : cooldowns)
        if (turns > 0) --turns;
}

void CombatController::showUseAbility() {
    AbilityMenu menu(player_, monster_.get(), cooldowns);
    AbilityMenu::Result result = menu.run();

    if (result == AbilityMenu::Result::ABILITY_USED) {
        // Check if monster died from the ability
        if (!monster_->isAlive()) {
            outcome = Outcome::WON;
            currentPage = Page::RESULT;
            return;
        }
        // Ability counts as the player's turn
        currentPage = Page::MONSTER_TURN;
    }
    // If BACK, stay on PLAYER_TURN so they pick something else
}
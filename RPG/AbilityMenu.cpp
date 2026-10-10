#include "AbilityMenu.h"
#include "Utils.h"
#include <iostream>
#include <sstream>

// ---- Constructors -----------------------------------------------------------

AbilityMenu::AbilityMenu(Player* player)
    : player_(player), monster_(nullptr), inCombat_(false)
{
    cooldowns_ = &dummyCooldowns_;
}

AbilityMenu::AbilityMenu(Player* player, Monster* monster,
    std::unordered_map<std::string, int>& cooldowns)
    : player_(player), monster_(monster), inCombat_(true)
{
    cooldowns_ = &cooldowns;
}

// ---- Entry point ------------------------------------------------------------

AbilityMenu::Result AbilityMenu::run() {
    while (!done) {
        clearScreen();
        showTop(player_);
        switch (currentPage) {
        case Page::ABILITY_LIST:   showAbilityList(); break;
        case Page::ABILITY_DETAIL: showAbilityDetail(); break;
        }
    }
    return result_;
}

// ---- Pages ------------------------------------------------------------------

void AbilityMenu::showAbilityList() {
    const auto& abilityList = player_->us.getAbilityList();

    std::cout << "-----------------------------Abilities-----------------------\n";

    if (abilityList.empty()) {
        std::cout << "You have no abilities.\n"
            << "Press any key to go back.\n";
        getchar();
        flushInput();
        done = true;
        return;
    }

    for (size_t i = 0; i < abilityList.size(); ++i) {
        const Ability& ability = abilityList[i];
        int cd = getCooldown(ability);

        std::cout << "  [" << i + 1 << "] " << ability.name_;

        // Show AP cost
        if (ability.cost_ > 0)
            std::cout << "  (AP: " << ability.cost_ << ")";
        else
            std::cout << "  (No AP cost)";

        // Show cooldown status
        if (cd > 0)
            std::cout << "  [Cooldown: " << cd << " turns]";
        else
            std::cout << "  [Ready]";

        // Show target
        switch (ability.target_) {
        case AbilityTarget::SELF:  std::cout << "  -> Self\n";  break;
        case AbilityTarget::ENEMY: std::cout << "  -> Enemy\n"; break;
        case AbilityTarget::ALLY:  std::cout << "  -> Ally\n";  break;
        }
    }

    std::cout << "\n-------------------------------------------------------------\n"
        << "Enter ability number to inspect";
    if (inCombat_) std::cout << " and use";
    std::cout << ", or b = back\n";

    char c = getchar();
    flushInput();

    if (c == 'b') {
        done = true;
        result_ = Result::BACK;
        return;
    }

    size_t index = static_cast<size_t>(c - '1');
    if (index >= abilityList.size()) {
        wrongInput();
        return;
    }

    selectedIndex = index;
    currentPage = Page::ABILITY_DETAIL;
}

void AbilityMenu::showAbilityDetail() {
    const auto& abilityList = player_->us.getAbilityList();
    const Ability& ability = abilityList[selectedIndex];
    int cd = getCooldown(ability);

    std::cout << "-----------------------------" << ability.name_
        << "-----------------------------\n";

    // HP effect line — positive = heal/damage, negative = costs HP
    if (ability.HPEffect_ > 0) {
        if (ability.target_ == AbilityTarget::ENEMY)
            std::cout << "Damage:     | " << ability.HPEffect_;
        else
            std::cout << "Heals:      | " << ability.HPEffect_;

        // Show scalar bonus
        switch (ability.scalar_) {
        case AbilityScalar::STR:
            std::cout << " + 1/3 of STR (" << player_->us.getTotalStrength() << ")";
            break;
        case AbilityScalar::AGI:
            std::cout << " + 1/3 of AGI (" << player_->us.getTotalAgility() << ")";
            break;
        case AbilityScalar::INT:
            std::cout << " + 1/3 of INT (" << player_->us.getTotalIntellect() << ")";
            break;
        default: break;
        }
        std::cout << '\n';
    }

    std::cout << "AP Cost:    | " << ability.cost_ << "\n"
        << "Cooldown:   | " << ability.cooldown_ << " turns\n";

    // Show buff details if any
    if (ability.givenBuff_) {
        const Buff& b = *ability.givenBuff_;
        std::cout << (b.isDebuff ? "Applies Debuff: " : "Applies Buff: ")
            << b.name << " (" << b.duration << " turns)\n";
        if (b.buffedStats.strength)   std::cout << "  STR: " << b.buffedStats.strength << "\n";
        if (b.buffedStats.intellect)  std::cout << "  INT: " << b.buffedStats.intellect << "\n";
        if (b.buffedStats.agility)    std::cout << "  AGI: " << b.buffedStats.agility << "\n";
        if (b.buffedStats.armor)      std::cout << "  ARM: " << b.buffedStats.armor << "\n";
        if (b.buffedStats.elementRes) std::cout << "  RES: " << b.buffedStats.elementRes << "\n";
    }

    std::cout << "\n-------------------------------------------------------------\n";

    if (!inCombat_) {
        std::cout << "b = back\n";
        char c = getchar();
        flushInput();
        currentPage = Page::ABILITY_LIST;
        return;
    }

    // In combat — offer use or back
    if (cd > 0) {
        std::cout << "On cooldown for " << cd << " more turns.\n"
            << "b = back\n";
        getchar();
        flushInput();
        currentPage = Page::ABILITY_LIST;
        return;
    }

    if (!hasSufficientMP(ability)) {
        std::cout << "Not enough AP! (Have "
            << player_->us.getCurrentAP() << ", need " << ability.cost_ << ")\n"
            << "b = back\n";
        getchar();
        flushInput();
        currentPage = Page::ABILITY_LIST;
        return;
    }

    std::cout << "u = use     b = back\n";
    char c = getchar();
    flushInput();

    switch (c) {
    case 'u':
        if (useAbility(ability)) {
            result_ = Result::ABILITY_USED;
            done = true;
        }
        break;
    case 'b':
        currentPage = Page::ABILITY_LIST;
        break;
    default:
        wrongInput();
        break;
    }
}

// ---- Helpers ----------------------------------------------------------------

int AbilityMenu::getCooldown(const Ability& ability) const {
    auto it = cooldowns_->find(ability.name_);
    return (it != cooldowns_->end()) ? it->second : 0;
}

bool AbilityMenu::hasSufficientMP(const Ability& ability) const {
    return player_->us.getCurrentAP() >= ability.cost_;
}

stattype AbilityMenu::getScalarValue(AbilityScalar scalar) const {
    switch (scalar) {
    case AbilityScalar::STR: return player_->us.getTotalStrength();
    case AbilityScalar::AGI: return player_->us.getTotalAgility();
    case AbilityScalar::INT: return player_->us.getTotalIntellect();
    default:                 return 0;
    }
}

bool AbilityMenu::useAbility(const Ability& ability) {
    // Deduct AP cost
    if (ability.cost_ > 0)
        player_->us.spendAP(ability.cost_);

    // Calculate total effect — base + 1/3 scalar stat
    uint32_t totalEffect = ability.HPEffect_ + (stattype)(getScalarValue(ability.scalar_) / 3);

    switch (ability.target_) {

    case AbilityTarget::SELF:
        if (totalEffect > 0)
            player_->us.heal(static_cast<welltype>(totalEffect));
        if (ability.givenBuff_)
            player_->us.applyBuff(*ability.givenBuff_);

        std::cout << "\nYou used " << ability.name_ << "!\n";
        if (totalEffect > 0) std::cout << "Healed " << totalEffect << " HP.\n";
        if (ability.givenBuff_)
            std::cout << "Applied " << ability.givenBuff_->name << ".\n";
        break;

    case AbilityTarget::ENEMY:
        if (!monster_) break;

        if (totalEffect > 0) {
            // Reduce by monster's element resistance for magic abilities
            uint32_t reduced = totalEffect > (uint32_t)monster_->getTotalElementRes()
                ? totalEffect - monster_->getTotalElementRes() : 1u;
            monster_->HP->reduceCurrent(static_cast<welltype>(reduced));

            std::cout << "\nYou used " << ability.name_
                << " on " << monster_->name << "!\n"
                << "Dealt " << reduced << " damage.\n";
        }

        // Apply debuff to monster if ability has one
        if (ability.givenBuff_)
            monster_->applyBuff(*ability.givenBuff_);
        break;
    }

    // Set cooldown
    if (ability.cooldown_ > 0)
        (*cooldowns_)[ability.name_] = static_cast<int>(ability.cooldown_);

    std::cout << "Press any key to continue.\n";
    getchar();
    flushInput();
    return true;
}
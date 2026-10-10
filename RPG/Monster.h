#ifndef MONSTER_H
#define MONSTER_H

#include "StatBlock.h"
#include "PointWell.h"
#include "Types.h"
#include <string>
#include <memory>

enum class MonsterType {
    // Tier 1 (Depths 1-3)
    GOBLIN, RAT, SKELETON,
    // Tier 2 (Depths 4-6)
    ORC, ZOMBIE, DARK_ELF,
    // Tier 3 (Depths 7-9)
    TROLL, VAMPIRE, GOLEM,
    // Tier 4 (Depths 10+)
    DRAGON, LICH, DEMON
};

struct Monster : public StatBlock {
    Monster(std::string name, MonsterType type,
        welltype hp,
        stattype str, stattype intel, stattype agi, stattype arm,
        exptype expReward, damagetype minDmg, damagetype maxDmg);

    std::string   name;
    MonsterType   type;
    exptype       expReward;
    damagetype    minDamage;
    damagetype    maxDamage;
    std::unique_ptr<PointWell> HP;

    bool isAlive() const { return HP->getCurrent() > 0; }
    void applyBuff(const Buff& b) { addNewBuff(b); }

    const std::vector<Buff>& getBuffList() const { return buffs; }
};

// Factory — picks a monster appropriate to the dungeon depth
std::unique_ptr<Monster> makeMonsterForDepth(int depth);

#endif
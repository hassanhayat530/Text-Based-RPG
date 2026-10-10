#include "StatBlock.h"

StatBlock::StatBlock(stattype s, stattype i, stattype a,
    stattype arm, stattype elres) {
    base.strength = s;
    base.intellect = i;
    base.agility = a;
    base.armor = arm;
    base.elementRes = elres;
}

void StatBlock::setBaseStats(stattype s, stattype i, stattype a,
    stattype arm, stattype elres) {
    base.strength = s;
    base.intellect = i;
    base.agility = a;
    base.armor = arm;
    base.elementRes = elres;
}

void StatBlock::tickBuffs() {
    if (buffs.empty()) return;

    for (auto& buff : buffs)
        if (buff.duration > 0) --buff.duration;

    // Remove expired buffs
    buffs.erase(
        std::remove_if(buffs.begin(), buffs.end(),
            [](const Buff& b) { return b.duration == 0; }),
        buffs.end());

    // Recalculate fromBuffs with whatever remains
    recalculateBuffs();
}

stattype StatBlock::getBaseStrength() const { return base.strength; }
stattype StatBlock::getBaseIntellect() const { return base.intellect; }
stattype StatBlock::getBaseAgility() const { return base.agility; }
stattype StatBlock::getBaseArmor() const { return base.armor; }
stattype StatBlock::getBaseElementRes() const { return base.elementRes; }

stattype StatBlock::getTotalStrength() const { return base.strength + fromBuffs.strength; }
stattype StatBlock::getTotalIntellect() const { return base.intellect + fromBuffs.intellect; }
stattype StatBlock::getTotalAgility() const { return base.agility + fromBuffs.agility; }
stattype StatBlock::getTotalArmor() const { return base.armor + fromBuffs.armor; }
stattype StatBlock::getTotalElementRes() const { return base.elementRes + fromBuffs.elementRes; }

void StatBlock::addNewBuff(Buff b) {
    for (auto& buff : buffs) {
        if (b.name == buff.name) {
            buff.duration = b.duration;
            return;
        }
    }
    buffs.push_back(b);
    recalculateBuffs();
}

void StatBlock::increaseStats(stattype s, stattype i, stattype a,
    stattype arm, stattype elres) {
    base.strength += s;
    base.intellect += i;
    base.agility += a;
    base.armor += arm;
    base.elementRes += elres;
}

void StatBlock::increaseStats(CoreStats cs) {
    base += cs;
}

void StatBlock::recalculateBuffs() {
    CoreStats tmpTotal;
    for (const auto& b : buffs) {
        if (b.isDebuff)
            tmpTotal -= b.buffedStats;
        else
            tmpTotal += b.buffedStats;
    }
    fromBuffs = tmpTotal;
}
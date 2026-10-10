#pragma once
#include "Monster.h"
#include <cstdlib>

Monster::Monster(std::string name, MonsterType type,
    welltype hp,
    stattype str, stattype intel, stattype agi, stattype arm,
    exptype expReward, damagetype minDmg, damagetype maxDmg)
    : StatBlock(str, intel, agi, arm, 0)
    , name(std::move(name))
    , type(type)
    , expReward(expReward)
    , minDamage(minDmg)
    , maxDamage(maxDmg)
{
    HP = std::make_unique<PointWell>(hp, hp);
}

std::unique_ptr<Monster> makeMonsterForDepth(int depth) {
    // Pick a random monster from the appropriate tier
    // Stats scale slightly with depth within a tier

    int scale = depth; // mild stat scaling per floor

    if (depth <= 3) {
        int roll = rand() % 3;
        switch (roll) {
        case 0: return std::make_unique<Monster>(
            "Goblin", MonsterType::GOBLIN,
            welltype(8 + scale), 2 + scale, 1, 3 + scale, 1,
            exptype(20 + depth * 5), 1, 3 + scale);
        case 1: return std::make_unique<Monster>(
            "Giant Rat", MonsterType::RAT,
            welltype(5 + scale), 1, 1, 4 + scale, 0,
            exptype(15 + depth * 5), 1, 2 + scale);
        default: return std::make_unique<Monster>(
            "Skeleton", MonsterType::SKELETON,
            welltype(10 + scale), 3 + scale, 1, 2, 2 + scale,
            exptype(25 + depth * 5), 2, 4 + scale);
        }
    }
    else if (depth <= 6) {
        int roll = rand() % 3;
        switch (roll) {
        case 0: return std::make_unique<Monster>(
            "Orc", MonsterType::ORC,
            welltype(18 + scale), 5 + scale, 1, 2, 3 + scale,
            exptype(50 + depth * 8), 3, 6 + scale);
        case 1: return std::make_unique<Monster>(
            "Zombie", MonsterType::ZOMBIE,
            welltype(15 + scale), 4 + scale, 1, 1, 2 + scale,
            exptype(40 + depth * 8), 2, 5 + scale);
        default: return std::make_unique<Monster>(
            "Dark Elf", MonsterType::DARK_ELF,
            welltype(12 + scale), 3, 4 + scale, 5 + scale, 1,
            exptype(55 + depth * 8), 2, 4 + scale);
        }
    }
    else if (depth <= 9) {
        int roll = rand() % 3;
        switch (roll) {
        case 0: return std::make_unique<Monster>(
            "Troll", MonsterType::TROLL,
            welltype(28 + scale), 7 + scale, 1, 2, 5 + scale,
            exptype(90 + depth * 10), 4, 8 + scale);
        case 1: return std::make_unique<Monster>(
            "Vampire", MonsterType::VAMPIRE,
            welltype(22 + scale), 5 + scale, 6 + scale, 5 + scale, 2,
            exptype(100 + depth * 10), 3, 7 + scale);
        default: return std::make_unique<Monster>(
            "Golem", MonsterType::GOLEM,
            welltype(30 + scale), 8 + scale, 2, 1, 8 + scale,
            exptype(95 + depth * 10), 5, 9 + scale);
        }
    }
    else {
        int roll = rand() % 3;
        switch (roll) {
        case 0: return std::make_unique<Monster>(
            "Dragon", MonsterType::DRAGON,
            welltype(50 + scale), 10 + scale, 8 + scale, 5 + scale, 8 + scale,
            exptype(200 + depth * 15), 8, 15 + scale);
        case 1: return std::make_unique<Monster>(
            "Lich", MonsterType::LICH,
            welltype(35 + scale), 4, 12 + scale, 6 + scale, 4,
            exptype(220 + depth * 15), 6, 12 + scale);
        default: return std::make_unique<Monster>(
            "Demon", MonsterType::DEMON,
            welltype(45 + scale), 9 + scale, 9 + scale, 7 + scale, 6,
            exptype(210 + depth * 15), 7, 13 + scale);
        }
    }
}
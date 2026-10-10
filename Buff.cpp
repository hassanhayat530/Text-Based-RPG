#include "Buff.h"

Buff::Buff(const char* nam, stattype str, stattype intel,
    stattype agi, stattype arm, stattype elres,
    uint16_t dur, bool isdebuff)
    : name(nam), duration(dur), isDebuff(isdebuff) {
    buffedStats.strength = str;
    buffedStats.intellect = intel;
    buffedStats.agility = agi;
    buffedStats.armor = arm;
    buffedStats.elementRes = elres;
}

Buff::Buff(const char* nam, CoreStats cs, uint16_t dur, bool isdebuff)
    : name(nam), duration(dur), isDebuff(isdebuff), buffedStats(cs) {
}
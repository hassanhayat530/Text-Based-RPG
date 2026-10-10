#include "Potion.h"

Potion::Potion(std::string nam, welltype hpHeal, itemcount quant, Buff* buf)
    : ItemDelegate(nam), buff(buf), healAmount(hpHeal), quantity(quant) {
}

void Potion::setBuff(Buff* newBuff) {
    if (buff) {
        delete buff;
        buff = nullptr;
    }
    buff = newBuff;
}

Potion::~Potion() {
    delete buff;
    buff = nullptr;
}
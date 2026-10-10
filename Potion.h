#ifndef POTION_H
#define POTION_H

#include "Item.h"

class Potion final : public ItemDelegate {
public:
    Buff* buff;
    welltype healAmount;
    itemcount quantity;
    ~Potion();

    const Buff* getBuff() const noexcept{ return buff; }
    void setBuff(Buff* newBuff);

private:
    Potion(std::string nam, welltype hpHeal = 1u, itemcount quant = 1u, Buff* buf = nullptr);
    friend class ItemManager;
};

#endif
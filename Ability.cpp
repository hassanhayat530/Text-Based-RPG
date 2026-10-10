#include "Ability.h"

Ability::Ability(std::string name,  uint32_t HPEffect, Buff* givenBuff, uint32_t cost, uint32_t cooldown,
    AbilityTarget target, AbilityScalar scalar)
    : name_(name), HPEffect_(HPEffect), givenBuff_(givenBuff), cost_(cost), cooldown_(cooldown),
    target_(target), scalar_(scalar) {
}

Ability::~Ability() {
    if (givenBuff_) {
        delete givenBuff_;
        givenBuff_ = nullptr;
    }
}

Ability::Ability(const Ability& other)
    : name_(other.name_)
    , HPEffect_(other.HPEffect_)
    , cost_(other.cost_)
    , cooldown_(other.cooldown_)
    , target_(other.target_)
    , scalar_(other.scalar_)
    , givenBuff_(other.givenBuff_ ? new Buff(*other.givenBuff_) : nullptr)
{}

Ability& Ability::operator=(const Ability& other) {
    if (this == &other) return *this;
    delete givenBuff_;
    name_ = other.name_;
    HPEffect_ = other.HPEffect_;
    cost_ = other.cost_;
    cooldown_ = other.cooldown_;
    target_ = other.target_;
    scalar_ = other.scalar_;
    givenBuff_ = other.givenBuff_ ? new Buff(*other.givenBuff_) : nullptr;
    return *this;
}

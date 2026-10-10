#ifndef PLAYER_H
#define PLAYER_H

#include "PlayerCharacter.h"
#include "Races.h"
#include "Rogue.h"
#include "Wizard.h"
#include "Warrior.h"
#include "Cleric.h"
#include "ItemManager.h"

// Stores all needed game state information in this project.
struct Player {
    Player(std::unique_ptr<PlayerCharacterDelegate> pc, std::unique_ptr<Race> race)
        : us(std::move(pc), std::move(race)) {}
	Player(std::unique_ptr<PlayerCharacterDelegate> pc, std::unique_ptr<Race> race, LoadTag)
		: us(std::move(pc), std::move(race), LoadTag{}) {}

	Player() = delete;
	
	bool IsAlive() { return (us.getCurrentHP() > 0); }
	PlayerCharacter us;

	std::string characterName;
	size_t daysAlive = 1;
	int gold = 0;
	int depthReached = 0;
};

#endif

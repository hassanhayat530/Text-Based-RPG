#ifndef DUNGEONCONTROLLER_H
#define DUNGEONCONTROLLER_H

#include "Dungeon.h"
#include "Player.h"

constexpr leveltype LEVEL_TIER_1 = 3;
constexpr leveltype LEVEL_TIER_2 = 6;
constexpr leveltype LEVEL_TIER_3 = 9;
constexpr leveltype LEVEL_TIER_4 = 12;

class DungeonController {
public:
	DungeonController(std::unique_ptr<Player>& player, int depth);
	void run();

private:
	enum class Page {
		EXPLORING,
		MONSTER_ENCOUNTER,
		TREASURE_ENCOUNTER,
		TRAP_ENCOUNTER,
		FLOOR_COMPLETE,
		EXIT,
	};

	// Data Members
	Page currentPage = Page::EXPLORING;
	bool done = false;
	int depth_;
	Dungeon dungeon;
	std::unique_ptr<Player>& player_;
	int xpos;
	int ypos;

	// Pages
	void exploring();
	void monsterEncounter();
	void treasureEncounter();
	void trapEncounter();
	void floorComplete();
	void exit();

	// Helpers
	//void revealSurroundings();
	//void discoverRoom(int x, int y);
	void revealWalls();
	void enterRoom(int nxpos, int nypos);
	void playerDeath();
};

#endif // !DUNGEONMENU_H

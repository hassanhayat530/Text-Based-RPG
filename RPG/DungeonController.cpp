#include "Utils.h"
#include "Random.h" // Allows methods for uniform randomness
#include "DungeonController.h"
#include "Inventory.h"
#include "CombatController.h"
#include "LootGenerator.h"

DungeonController::DungeonController(std::unique_ptr<Player>& player, int depth)
	: player_(player), depth_(depth), dungeon(Dungeon(depth)) {
	xpos = dungeon.getEntranceX();
	ypos = dungeon.getEntranceY();
	//revealSurroundings();
}

void DungeonController::run() {
	while (!done) {
		switch (currentPage) {
		case Page::EXPLORING:          exploring(); break;
		case Page::MONSTER_ENCOUNTER:  monsterEncounter(); break;
		case Page::TREASURE_ENCOUNTER: treasureEncounter(); break;
		case Page::TRAP_ENCOUNTER:     trapEncounter(); break;
		case Page::FLOOR_COMPLETE:     floorComplete(); break;
		case Page::EXIT:               exit(); break;
		}
	}
}

// ---------------- Pages -------------------------------------------

void DungeonController::exploring() {
	clearScreen();
	showTop(player_.get());

	std::cout << dungeon.buildMapString(xpos, ypos);

	const Room& current = dungeon.getRoom(xpos, ypos);

	std::cout << "\nChoose an action:\n";

	if (current.north) std::cout << "w = North\n";
	if (current.south) std::cout << "s = South\n";
	if (current.east)  std::cout << "d = East\n";
	if (current.west)  std::cout << "a = West\n";
	std::cout /*<< "\n\ne = exit dungeon (lose bonus of finding exit)\n"*/
		<< "\n(c = character    i = inventory)\n";

	char c = getchar();
	flushInput();

	int nxpos = xpos, nypos = ypos;
	switch (c) {
	case 'w': if (current.north) nypos -= 1; else { wrongInput(); return; } break;
	case 's': if (current.south) nypos += 1; else { wrongInput(); return; } break;
	case 'd': if (current.east)  nxpos += 1; else { wrongInput(); return; } break;
	case 'a': if (current.west)  nxpos -= 1; else { wrongInput(); return; } break;
	//case 'e': currentPage = Page::EXIT; break;
	case 'c': showCharacter(player_.get()); break;
	case 'i': {
		Inventory inv(player_.get());
		inv.run();
		break;
	}
	default:  wrongInput(); return;
	}

	if (nxpos != xpos || nypos != ypos)
		enterRoom(nxpos, nypos);
}

void DungeonController::monsterEncounter() {
	auto monster = makeMonsterForDepth(depth_);

	CombatController combat(player_.get(), std::move(monster));
	CombatController::Outcome outcome = combat.run();

	switch (outcome) {
	case CombatController::Outcome::WON:
		// Room is cleared, player got EXP inside CombatController
		dungeon.getRoom(xpos, ypos).state = RoomState::CLEARED;
		currentPage = Page::EXPLORING;
		break;
	case CombatController::Outcome::FLED:
		// Leave room uncleared so encounter remains on return
		currentPage = Page::EXPLORING;
		break;
	case CombatController::Outcome::LOST:
		playerDeath();
		break;
	}
}

void DungeonController::treasureEncounter() {
	int gold = randomInRange(0, 4) + 2 + depth_;
	player_->gold += gold;

	std::string itemName = LootGenerator::generateTreasure(&player_->us, depth_);

	std::cout << "You found a treasure chest!\n\n"
		<< "You found " << gold << " gold.\n\n"
		<< "You found: " << itemName << "\n"
		<< "You now have: " << player_->gold <<"\n"
		<< "(Check your inventory to equip or use it)\n\n"
		<< "Press any key to continue.\n";

	char _ = getchar();
	flushInput();

	dungeon.getRoom(xpos, ypos).state = RoomState::CLEARED;
	currentPage = Page::EXPLORING;
}

void DungeonController::trapEncounter() {
	int damage = (randomInRange(0, 4) + 1) * depth_;

	std::cout << "You triggered a trap!\n\n"
		<< "You took " << damage << " damage.\n\n"
		<< "Press any key to continue.\n";

	player_->us.takeDamage(static_cast<welltype>(damage));
	char _ = getchar();
	flushInput();

	dungeon.getRoom(xpos, ypos).state = RoomState::CLEARED;

	if (!player_->IsAlive()) {
		playerDeath();
		return;
	}

	currentPage = Page::EXPLORING;
}

void DungeonController::floorComplete() {
	if (depth_ + 1 > player_->depthReached) player_->depthReached = depth_ + 1;

	std::cout << "You found the exit!\n\n"
		<< "1 = Descend deeper (Floor " << depth_ + 1 << ")\n"
		<< "2 = Return to town\n";

	char c = getchar();
	flushInput();

	switch (c) {
	case '1':
		// Rebuild dungeon one floor deeper, reset player position
		++depth_;
		dungeon = Dungeon(depth_);
		xpos = dungeon.getEntranceX();
		ypos = dungeon.getEntranceY();
		//revealSurroundings();
		currentPage = Page::EXPLORING;
		break;
	case '2':
		currentPage = Page::EXIT;
		break;
	default:
		wrongInput();
		break;
	}
}

void DungeonController::exit() {
	clearScreen();
	showTop(player_.get());

	std::cout << "After your perilous adventure, you move back to gather yourself in the town.\n"
		<< "Press any key to continue.\n";

	char _ = getchar();
	done = true;
}

// --------------- Helpers ------------------------------------------
//void DungeonController::revealSurroundings() {
//	const Room& current = dungeon.getRoom(xpos, ypos);
//
//	if (current.north && ypos - 1 >= 0)
//		discoverRoom(xpos, ypos - 1);
//	if (current.south && ypos + 1 < dungeon.HEIGHT)
//		discoverRoom(xpos, ypos + 1);
//	if (current.east && xpos + 1 < dungeon.WIDTH)
//		discoverRoom(xpos + 1, ypos);
//	if (current.west && xpos - 1 >= 0)
//		discoverRoom(xpos - 1, ypos);
//}

//void DungeonController::discoverRoom(int x, int y) {
//	Room& room = dungeon.getRoom(x, y);
//	if (room.state == RoomState::UNDISCOVERED)
//		room.state = RoomState::DISCOVERED; // never overwrite CLEARED
//}

void DungeonController::enterRoom(int nxpos, int nypos) {
	xpos = nxpos;
	ypos = nypos;

	Room& room = dungeon.getRoom(xpos, ypos);
	//revealSurroundings();

	// Check BEFORE overwriting state
	bool alreadyCleared = (room.state == RoomState::CLEARED);

	switch (room.type) {
	case RoomType::MONSTER:
		currentPage = alreadyCleared ? Page::EXPLORING : Page::MONSTER_ENCOUNTER;
		break;
	case RoomType::TREASURE:
		currentPage = alreadyCleared ? Page::EXPLORING : Page::TREASURE_ENCOUNTER;
		break;
	case RoomType::TRAP:
		currentPage = alreadyCleared ? Page::EXPLORING : Page::TRAP_ENCOUNTER;
		break;
	case RoomType::EXIT:
		currentPage = Page::FLOOR_COMPLETE;
		break;
	default:
		room.state = RoomState::CLEARED;
		currentPage = Page::EXPLORING;
		break;
	}
}

void DungeonController::playerDeath() {
	std::cout << "You have Died!!!\n Your journey ends here...\n"
		<< "Press any key to return to Main Menu.\n";
	char _ = getchar();
	flushInput();
	done = true;
}
#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "CharacterCreation.h"
#include "Town.h"
#include "DungeonController.h"
#include "Player.h"

class GameController {
private:
	enum class State {
		MAIN_MENU,
		CHARACTER_CREATION,
		TOWN,
		DUNGEON,
		EXIT
	};

	std::unique_ptr<Player> mainCharacter = nullptr;
	State currentState = State::MAIN_MENU;

	std::unique_ptr<CharacterCreation> characterCreation;
	std::unique_ptr<Town> town;
	std::unique_ptr<DungeonController> dungeon;

	void showMainMenu();
	void enterGame();
	void exit();
public:
	GameController() = default;
	void run();
};



#endif


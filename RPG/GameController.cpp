#include "GameController.h"
#include "Utils.h"
#include "SaveManager.h"
#include <iostream>

void GameController::run() {
    int depth = 1;

    while (currentState != State::EXIT) {
        switch (currentState) {
        case State::MAIN_MENU:
            showMainMenu();
            break;
        case State::CHARACTER_CREATION:
            characterCreation = std::make_unique<CharacterCreation>();
            mainCharacter = characterCreation->run();
            currentState = State::TOWN; // after creation, go to town
            characterCreation.reset();  // free the memory, no longer needed
            break;
        case State::TOWN:
            town = std::make_unique<Town>(mainCharacter);
            depth = town->run();
            currentState = State::DUNGEON;
            town.reset();
            break;
        case State::DUNGEON:
            dungeon = std::make_unique<DungeonController>(mainCharacter, depth);
            dungeon->run();
            currentState = mainCharacter->IsAlive() ? State::TOWN : State::MAIN_MENU;
            dungeon.reset();
            break;
        case State::EXIT:
            exit();
            break;
        }
    }
}

void GameController::exit() {
    currentState = State::EXIT;
}

void GameController::showMainMenu() {
    clearScreen();

	std::cout << "Choose an option: \n"
		<< "1 = New Game \n"
		<< "2 = Load Game \n" 
		<< "3 = Exit \n";

	char c = getchar();
	flushInput();

	switch (c) {
	case '1':
        currentState = State::CHARACTER_CREATION;
		break;
	case '2':
        mainCharacter = SaveManager::load();
        if (!mainCharacter) {
            std::cout << "Failed to load save file. Press any key to continue.\n";
            char _ = getchar();
            flushInput();
            break; // stays on MAIN_MENU
        }
        currentState = State::TOWN;
        break;
	case '3':
		exit();
		break;
	default:
		wrongInput();
		showMainMenu();
		break;
	}
}

void GameController::enterGame() {
    exit();
}
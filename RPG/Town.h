#ifndef TOWN_H
#define TOWN_H

#include "Player.h"
#include <memory>

class Town {
public:
	Town(std::unique_ptr<Player>& mainCharacter_);
	int run();

private:
	enum class Page {
		MAIN_OPTIONS,
		TAVERN,
		SLUMS,
		NEXT_DAY,
		LEAVE_CONFIRM,
		DEPTH_SELECTION
	};

	Page currentPage = Page::MAIN_OPTIONS;
	bool enteredTown = false;
	bool done = false;
	std::unique_ptr<Player>& mainCharacter_;

	// Page Handlers
	void showMainOptions();
	void showTavern();
	void showSlums();
	void showNextDay();
	void showLeaveconfirm();
	int showDepthSelection();
};

#endif

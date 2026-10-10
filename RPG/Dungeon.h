#ifndef DUNGEON_H
#define DUNGEON_H

#include "Player.h"
#include "Room.h"

class Dungeon {
public:
	Dungeon(int depth = 1);

	static const int WIDTH = 10;
	static const int HEIGHT = 10;

	// getters
	int getEntranceX() { return enterX; }
	int getEntranceY() { return enterY; }
	Room& getRoom(int x, int y) { return grid[y][x]; }
	const Room& getRoom(int x, int y) const { return grid[y][x]; }

	// Dungeon Display
	std::string buildMapString(int playerX, int playerY) const;
private:
	enum class Page {
		EXPLORATION,
		ROOM,
		FIGHT
	};

	// Data Members
	Room grid[HEIGHT][WIDTH];
	int depth_;
	int enterX = 0; int enterY = 0;
	int exitX = WIDTH - 1; int exitY = HEIGHT - 1;

	// Main generating function
	void generate();

	// Helpers
	bool inBounds(int x, int y);
	void makeGarunteedPath();
	void addRandomPassages();
	void openAdjacentPassages(int x, int y, int nx, int ny);
	void scatterRooms();
	RoomType randomRoomType() const;
};

#endif // !DUNGEON_H

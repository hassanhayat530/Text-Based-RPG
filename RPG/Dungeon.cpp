#include "Dungeon.h"
#include "Random.cpp"
#include "sstream"
#include "iostream"

Dungeon::Dungeon(int depth) : depth_(depth) {
	generate();
}

//------------ Main Generating Function -----------------------------
void Dungeon::generate() {
	for (int y = 0; y < HEIGHT; ++y) {
		for (int x = 0; x < WIDTH; ++x) {
			grid[y][x] = Room{};
		}
	}

	enterX = randomInRange(0, WIDTH - 1); enterY = 0;
	exitX = randomInRange(0, WIDTH - 1); exitY = HEIGHT - 1;

	grid[enterY][enterX].type = RoomType::ENTRANCE;
	//grid[enterY][enterX].state = RoomState::DISCOVERED;
	grid[exitY][exitX].type = RoomType::EXIT;

	makeGarunteedPath();
	addRandomPassages();
	scatterRooms();
}

//------------ Helpers -----------------------------------------------

bool Dungeon::inBounds(int x, int y) {
	return (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT);
}

void Dungeon::makeGarunteedPath() {
	int x = enterX, y = enterY;

	while (x != exitX || y != exitY) {
		int dx = (exitX - x);
		int dy = (exitY - y);

		bool moveHorizontal;
		if (dx == 0) moveHorizontal = false;
		else if (dy == 0) moveHorizontal = true;
		else moveHorizontal = (randomInRange(0, 1) == 0);

		int nx = x, ny = y;
		if (moveHorizontal) nx += (dx > 0) ? 1 : -1;
		else ny += (dy > 0) ? 1 : -1;

		if (!inBounds(nx, ny)) continue;

		openAdjacentPassages(x, y, nx, ny);

		x = nx;
		y = ny;

		if (grid[y][x].type == RoomType::EMPTY)
			grid[y][x].type = RoomType::EMPTY;
	}
}

void Dungeon::addRandomPassages() {
	// For every cell, randomly open passages to neighbours
	// Higher chance = more connected, more open map
	for (int y = 0; y < HEIGHT; ++y) {
		for (int x = 0; x < WIDTH; ++x) {
			// Try to open each direction with some probability
			int dirs[4][2] = { {0,-1},{0,1},{1,0},{-1,0} };
			for (auto& d : dirs) {
				int nx = x + d[0];
				int ny = y + d[1];
				if (inBounds(nx, ny) && chance(40))
					openAdjacentPassages(x, y, nx, ny);
			}
		}
	}
}

void Dungeon::openAdjacentPassages(int x, int y, int nx, int ny) {
	if (nx == x + 1) { grid[y][x].east = true; grid[ny][nx].west = true; }
	if (nx == x - 1) { grid[y][x].west = true; grid[ny][nx].east = true; }
	if (ny == y + 1) { grid[y][x].south = true; grid[ny][nx].north = true; }
	if (ny == y - 1) { grid[y][x].north = true; grid[ny][nx].south = true; }

	// randomly open some extra passages to create loops
	if (chance(25)) {
		// pick a random neighbour of nx,ny
		int dirs[4][2] = { {0,-1},{0,1},{-1,0},{1,0} };
		int d = randomInRange(0, 3);
		int ex = nx + dirs[d][0];
		int ey = ny + dirs[d][1];
		if (inBounds(ex, ey))
			openAdjacentPassages(nx, ny, ex, ey);
	}
}

void Dungeon::scatterRooms() {
	for (int y = 0; y < HEIGHT; ++y) {
		for (int x = 0; x < WIDTH; ++x) {
			Room& room = grid[y][x];
			if (room.type == RoomType::ENTRANCE || room.type == RoomType::EXIT)
				continue;
			room.type = randomRoomType();
		}
	}
}

RoomType Dungeon::randomRoomType() const {
	int monsterWeight = 30 + (depth_ * 5); // more monsters deeper
	int treasureWeight = 15;
	int trapWeight = 15;
	int emptyWeight = 100 - monsterWeight - treasureWeight - trapWeight;
	if (emptyWeight < 5) emptyWeight = 5; // always some empty rooms

	int roll = randomInRange(0, 99);
	if (roll < emptyWeight)                            return RoomType::EMPTY;
	if (roll < emptyWeight + monsterWeight)            return RoomType::MONSTER;
	if (roll < emptyWeight + monsterWeight + trapWeight) return RoomType::TRAP;
	return RoomType::TREASURE;
}

std::string Dungeon::buildMapString(int playerX, int playerY) const {
	std::ostringstream ss;
	ss << "--------- Depth " << depth_ << " ---------\n";

	for (int y = 0; y < HEIGHT; ++y) {
		for (int x = 0; x < WIDTH; ++x) {
			const Room& room = grid[y][x];

			if (x == playerX && y == playerY) {
				ss << '@';
				continue;
			}

			switch (room.state) {
			case RoomState::UNDISCOVERED:
				ss << '?';
				break;
			//case RoomState::DISCOVERED:
			//	switch (room.type) {
			//	case RoomType::EMPTY:    ss << '.'; break;
			//	case RoomType::MONSTER:  ss << 'M'; break;
			//	case RoomType::TREASURE: ss << 'T'; break;
			//	case RoomType::TRAP:     ss << '^'; break;
			//	case RoomType::ENTRANCE: ss << 'E'; break;
			//	case RoomType::EXIT:     ss << 'X'; break;
			//	}
			//	break;
			case RoomState::CLEARED:
				ss << ' ';
				break;
			}
			ss << ' '; // spacing between cells
		}
		ss << '\n';
	}

	//ss << "\n@ = You   . = Empty   M = Monster   T = Treasure   ^ = Trap   X = Exit\n";
	return ss.str();
}
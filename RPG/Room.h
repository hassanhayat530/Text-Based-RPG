#ifndef ROOM_H
#define ROOM_H

enum class RoomType { EMPTY, MONSTER, TREASURE, TRAP, ENTRANCE, EXIT };
enum class RoomState { UNDISCOVERED, CLEARED };

struct Room {
    RoomType  type = RoomType::EMPTY;
    RoomState state = RoomState::UNDISCOVERED;
    bool north = false;
    bool south = false;
    bool east = false;
    bool west = false;
};

#endif
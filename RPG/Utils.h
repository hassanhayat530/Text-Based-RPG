#ifndef UTILS_H
#define UTILS_H

#include "Player.h"

void clearScreen();

void showTop(const Player* player);

void showCharacter(const Player* player);

void flushInput();

void wrongInput();

int readInt();

#endif

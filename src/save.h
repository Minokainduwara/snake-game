#ifndef SAVE_H
#define SAVE_H

#include "game.h"

#define SAVE_FILE "saves/snake_save.dat"

void Save_SaveSettings(const GameSettings *settings);
void Save_LoadSettings(GameSettings *settings);

#endif // SAVE_H
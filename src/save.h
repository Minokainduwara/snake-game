#ifndef SAVE_H
#define SAVE_H

#include "game.h"

#define SAVE_FILE "saves/snake_save.dat"
#define GAME_SAVE_FILE "saves/snake_game.dat"

void Save_SaveSettings(const GameSettings *settings);
void Save_LoadSettings(GameSettings *settings);
void Save_SaveGameState(const GameSnapshot *snap);
int Save_LoadGameState(GameSnapshot *snap);
void Save_ClearGameState(void);

#endif // SAVE_H
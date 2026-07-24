#include "save.h"
#include <stdio.h>
#include <string.h>

void Save_SaveSettings(const GameSettings *settings) {
    FILE *f = fopen(SAVE_FILE, "wb");
    if (!f) return;

    fwrite(settings, sizeof(GameSettings), 1, f);
    fclose(f);
}

void Save_LoadSettings(GameSettings *settings) {
    // Set defaults first
    settings->high_score = 0;
    settings->difficulty = DIFF_MEDIUM;
    settings->sound_enabled = 1;
    settings->show_grid = 1;

    FILE *f = fopen(SAVE_FILE, "rb");
    if (!f) return;

    fread(settings, sizeof(GameSettings), 1, f);
    fclose(f);
}

void Save_SaveGameState(const GameSnapshot *snap) {
    FILE *f = fopen(GAME_SAVE_FILE, "wb");
    if (!f) return;

    fwrite(snap, sizeof(GameSnapshot), 1, f);
    fclose(f);
}

int Save_LoadGameState(GameSnapshot *snap) {
    FILE *f = fopen(GAME_SAVE_FILE, "rb");
    if (!f) {
        snap->valid = 0;
        return 0;
    }

    int result = fread(snap, sizeof(GameSnapshot), 1, f);
    fclose(f);

    if (result != 1 || !snap->valid) {
        snap->valid = 0;
        return 0;
    }
    return 1;
}

void Save_ClearGameState(void) {
    remove(GAME_SAVE_FILE);
}
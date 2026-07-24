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
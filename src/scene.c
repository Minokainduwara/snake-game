#include "scene.h"
#include "audio.h"
#include "save.h"
#include <string.h>
#include <stdio.h>

#define MENU_ITEM_COUNT 6
static const char *menu_items[MENU_ITEM_COUNT] = {
    "Play Game",
    "Difficulty",
    "Options",
    "High Scores",
    "Save & Quit",
    "Exit"
};

#define DIFF_ITEM_COUNT 4
static const char *diff_names[DIFF_ITEM_COUNT] = {
    "Easy",
    "Medium",
    "Hard",
    "Expert"
};

#define OPT_ITEM_COUNT 3
static const char *opt_names[OPT_ITEM_COUNT] = {
    "Sound Effects",
    "Show Grid",
    "Back"
};

void Scene_Init(SceneManager *sm) {
    sm->current_scene = SCENE_MENU;
    sm->menu_selection = 0;
    sm->diff_selection = 0;
    sm->opt_selection = 0;
    sm->highscores_selection = 0;
}

// ─── Menu ────────────────────────────────────────────────
void Scene_UpdateMenu(SceneManager *sm, GameSettings *settings) {
    (void)settings;

    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        sm->menu_selection = (sm->menu_selection - 1 + MENU_ITEM_COUNT) % MENU_ITEM_COUNT;
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        sm->menu_selection = (sm->menu_selection + 1) % MENU_ITEM_COUNT;
    }

    // Mouse hover detection
    Vector2 mouse = GetMousePosition();
    int start_y = 200;
    for (int i = 0; i < MENU_ITEM_COUNT; i++) {
        Rectangle rect = {
            (GRID_WIDTH * CELL_SIZE) / 2 - 100,
            start_y + i * 50,
            200, 40
        };
        if (CheckCollisionPointRec(mouse, rect)) {
            sm->menu_selection = i;
        }
    }

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        switch (sm->menu_selection) {
            case 0: sm->current_scene = SCENE_PLAYING; break;
            case 1: sm->current_scene = SCENE_DIFFICULTY; break;
            case 2: sm->current_scene = SCENE_OPTIONS; break;
            case 3: sm->current_scene = SCENE_HIGH_SCORES; break;
            case 4: // Save & Quit
                Save_SaveSettings(settings);
                sm->current_scene = SCENE_EXIT;
                break;
            case 5: // Exit without saving
                sm->current_scene = SCENE_EXIT;
                break;
        }
    }
}

void Scene_DrawMenu(const SceneManager *sm, const GameSettings *settings) {
    int screen_w = GRID_WIDTH * CELL_SIZE;
    int screen_h = GRID_HEIGHT * CELL_SIZE + 50;

    ClearBackground((Color){ 20, 20, 30, 255 });

    // Title
    DrawText("SNAKE GAME", screen_w / 2 - MeasureText("SNAKE GAME", 50) / 2, 80, 50, GREEN);
    DrawText("by C & Raylib", screen_w / 2 - MeasureText("by C & Raylib", 20) / 2, 130, 20, GRAY);

    // Difficulty indicator
    DrawText(TextFormat("Difficulty: %s", Game_DifficultyName(settings->difficulty)),
             10, screen_h - 30, 15, DARKGRAY);
    DrawText(TextFormat("High Score: %d", settings->high_score),
             screen_w - MeasureText(TextFormat("High Score: %d", settings->high_score), 15) - 10,
             screen_h - 30, 15, DARKGRAY);

    // Menu items
    int start_y = 200;
    for (int i = 0; i < MENU_ITEM_COUNT; i++) {
        int x = screen_w / 2;
        int y = start_y + i * 50;
        int font_size = 25;
        int text_w = MeasureText(menu_items[i], font_size);
        Color c = (i == sm->menu_selection) ? GREEN : LIGHTGRAY;

        // Draw selection background
        if (i == sm->menu_selection) {
            DrawRectangle(x - text_w / 2 - 10, y - 5, text_w + 20, 35, (Color){ 50, 80, 50, 255 });
        }

        DrawText(menu_items[i], x - text_w / 2, y, font_size, c);
    }

    // Controls hint
    DrawText("Use ARROWS/WASD + ENTER  or  CLICK", 10, 10, 15, GRAY);
}

// ─── Difficulty ──────────────────────────────────────────
void Scene_UpdateDifficulty(SceneManager *sm, GameSettings *settings) {
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        sm->diff_selection = (sm->diff_selection - 1 + DIFF_ITEM_COUNT) % DIFF_ITEM_COUNT;
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        sm->diff_selection = (sm->diff_selection + 1) % DIFF_ITEM_COUNT;
    }

    Vector2 mouse = GetMousePosition();
    int start_y = 200;
    for (int i = 0; i < DIFF_ITEM_COUNT; i++) {
        Rectangle rect = { (GRID_WIDTH * CELL_SIZE) / 2 - 100, start_y + i * 50, 200, 40 };
        if (CheckCollisionPointRec(mouse, rect)) sm->diff_selection = i;
    }

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        settings->difficulty = (Difficulty)sm->diff_selection;
        Save_SaveSettings(settings);
        sm->current_scene = SCENE_MENU;
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        sm->current_scene = SCENE_MENU;
    }
}

void Scene_DrawDifficulty(const SceneManager *sm, const GameSettings *settings) {
    int screen_w = GRID_WIDTH * CELL_SIZE;

    ClearBackground((Color){ 20, 20, 30, 255 });

    DrawText("SELECT DIFFICULTY", screen_w / 2 - MeasureText("SELECT DIFFICULTY", 40) / 2, 100, 40, GREEN);

    int start_y = 200;
    for (int i = 0; i < DIFF_ITEM_COUNT; i++) {
        int x = screen_w / 2;
        int y = start_y + i * 50;
        int font_size = 25;
        int text_w = MeasureText(diff_names[i], font_size);
        int is_selected = (i == sm->diff_selection);
        int is_current = (i == (int)settings->difficulty);
        Color c = is_selected ? GREEN : LIGHTGRAY;

        if (is_selected) {
            DrawRectangle(x - text_w / 2 - 10, y - 5, text_w + 20, 35, (Color){ 50, 80, 50, 255 });
        }
        DrawText(diff_names[i], x - text_w / 2, y, font_size, c);

        if (is_current && !is_selected) {
            DrawText("(current)", x + text_w / 2 + 10, y, 15, DARKGRAY);
        }
    }

    DrawText("Press ESC to go back", 10, 10, 15, GRAY);
}

// ─── Options ─────────────────────────────────────────────
void Scene_UpdateOptions(SceneManager *sm, GameSettings *settings) {
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        sm->opt_selection = (sm->opt_selection - 1 + OPT_ITEM_COUNT) % OPT_ITEM_COUNT;
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        sm->opt_selection = (sm->opt_selection + 1) % OPT_ITEM_COUNT;
    }

    Vector2 mouse = GetMousePosition();
    int start_y = 200;
    for (int i = 0; i < OPT_ITEM_COUNT; i++) {
        Rectangle rect = { (GRID_WIDTH * CELL_SIZE) / 2 - 100, start_y + i * 50, 200, 40 };
        if (CheckCollisionPointRec(mouse, rect)) sm->opt_selection = i;
    }

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        switch (sm->opt_selection) {
            case 0: settings->sound_enabled = !settings->sound_enabled; break;
            case 1: settings->show_grid = !settings->show_grid; break;
            case 2: sm->current_scene = SCENE_MENU; break;
        }
        Save_SaveSettings(settings);
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        sm->current_scene = SCENE_MENU;
    }
}

void Scene_DrawOptions(const SceneManager *sm, const GameSettings *settings) {
    int screen_w = GRID_WIDTH * CELL_SIZE;

    ClearBackground((Color){ 20, 20, 30, 255 });

    DrawText("OPTIONS", screen_w / 2 - MeasureText("OPTIONS", 40) / 2, 100, 40, GREEN);

    // Toggle options
    int start_y = 200;
    for (int i = 0; i < OPT_ITEM_COUNT; i++) {
        int x = screen_w / 2;
        int y = start_y + i * 50;
        int font_size = 25;
        Color c = (i == sm->opt_selection) ? GREEN : LIGHTGRAY;

        if (i == sm->opt_selection) {
            DrawRectangle(x - 120, y - 5, 240, 35, (Color){ 50, 80, 50, 255 });
        }

        // For toggle items, show ON/OFF state
        if (i == 0) {
            const char *status = settings->sound_enabled ? "ON" : "OFF";
            Color status_c = settings->sound_enabled ? GREEN : RED;
            DrawText(TextFormat("%s: ", opt_names[i]), x - 100, y, font_size, c);
            DrawText(status, x + 60, y, font_size, status_c);
        } else if (i == 1) {
            const char *status = settings->show_grid ? "ON" : "OFF";
            Color status_c = settings->show_grid ? GREEN : RED;
            DrawText(TextFormat("%s: ", opt_names[i]), x - 100, y, font_size, c);
            DrawText(status, x + 60, y, font_size, status_c);
        } else {
            DrawText(opt_names[i], x - MeasureText(opt_names[i], font_size) / 2, y, font_size, c);
        }
    }

    DrawText("Press ESC to go back", 10, 10, 15, GRAY);
}

// ─── High Scores ─────────────────────────────────────────
void Scene_UpdateHighScores(SceneManager *sm) {
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ESCAPE) ||
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        sm->current_scene = SCENE_MENU;
    }
}

void Scene_DrawHighScores(const SceneManager *sm, const GameSettings *settings) {
    (void)sm;
    int screen_w = GRID_WIDTH * CELL_SIZE;

    ClearBackground((Color){ 20, 20, 30, 255 });

    DrawText("HIGH SCORES", screen_w / 2 - MeasureText("HIGH SCORES", 40) / 2, 100, 40, GREEN);

    char score_text[64];
    snprintf(score_text, sizeof(score_text), "Best Score: %d", settings->high_score);
    DrawText(score_text, screen_w / 2 - MeasureText(score_text, 30) / 2, 220, 30, RAYWHITE);

    DrawText("Press ENTER/ESC to go back", screen_w / 2 - MeasureText("Press ENTER/ESC to go back", 15) / 2, 350, 15, GRAY);
}
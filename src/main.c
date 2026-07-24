#include "raylib.h"
#include "game.h"
#include "audio.h"
#include "scene.h"
#include "save.h"
#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH  (GRID_WIDTH * CELL_SIZE)
#define SCREEN_HEIGHT (GRID_HEIGHT * CELL_SIZE + 50)  // extra space for score

int main(void) {
    srand((unsigned int)time(NULL));

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Snake Game");
    SetTargetFPS(60);

    Audio_Init();

    // Load settings
    GameSettings settings;
    Save_LoadSettings(&settings);

    // Scene manager
    SceneManager sm;
    Scene_Init(&sm);

    // Game state
    SnakeGame game;
    float tick_timer = 0.0f;
    int food_needs_respawn = 0;

    while (!WindowShouldClose() && sm.current_scene != SCENE_EXIT) {
        // ─── SCENE: MENU ──────────────────────────────────
        if (sm.current_scene == SCENE_MENU) {
            Scene_UpdateMenu(&sm, &settings);

            BeginDrawing();
            Scene_DrawMenu(&sm, &settings);
            EndDrawing();
        }

        // ─── SCENE: PLAYING ───────────────────────────────
        else if (sm.current_scene == SCENE_PLAYING) {
            // Initialize or restart game
            if (game.game_over) {
                Game_Init(&game);
                Audio_PlayStart();
                tick_timer = 0.0f;
                food_needs_respawn = 0;
            }

            // Input
            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))    game.next_dir = DIR_UP;
            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))  game.next_dir = DIR_DOWN;
            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A))  game.next_dir = DIR_LEFT;
            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) game.next_dir = DIR_RIGHT;

            // ESC to return to menu
            if (IsKeyPressed(KEY_ESCAPE)) {
                sm.current_scene = SCENE_MENU;
            }

            // Update
            float tick_rate = Game_GetTickRate(settings.difficulty);
            tick_timer += GetFrameTime();
            if (tick_timer >= tick_rate) {
                tick_timer = 0.0f;

                if (food_needs_respawn) {
                    game.food.x = rand() % GRID_WIDTH;
                    game.food.y = rand() % GRID_HEIGHT;
                    food_needs_respawn = 0;
                }

                int prev_length = game.length;
                Game_Update(&game);

                if (game.length > prev_length) {
                    if (settings.sound_enabled) Audio_PlayEat();
                    food_needs_respawn = 1;
                }

                if (game.game_over && prev_length > 0) {
                    if (settings.sound_enabled) Audio_PlayDeath();

                    // Update high score
                    if (game.score > settings.high_score) {
                        settings.high_score = game.score;
                        Save_SaveSettings(&settings);
                    }
                }
            }

            // Restart on R
            if (game.game_over && IsKeyPressed(KEY_R)) {
                Game_Init(&game);
                if (settings.sound_enabled) Audio_PlayStart();
                tick_timer = 0.0f;
                food_needs_respawn = 0;
            }

            // Back to menu on ESC when game over
            if (game.game_over && IsKeyPressed(KEY_ESCAPE)) {
                sm.current_scene = SCENE_MENU;
            }

            // Draw
            BeginDrawing();
            ClearBackground(RAYWHITE);
            Game_Draw(&game, &settings);

            // Show difficulty in corner
            DrawText(TextFormat("[%s]", Game_DifficultyName(settings.difficulty)),
                     SCREEN_WIDTH - MeasureText(TextFormat("[%s]", Game_DifficultyName(settings.difficulty)), 15) - 10,
                     GRID_HEIGHT * CELL_SIZE + 10, 15, GRAY);

            EndDrawing();
        }

        // ─── SCENE: DIFFICULTY ────────────────────────────
        else if (sm.current_scene == SCENE_DIFFICULTY) {
            Scene_UpdateDifficulty(&sm, &settings);

            BeginDrawing();
            Scene_DrawDifficulty(&sm, &settings);
            EndDrawing();
        }

        // ─── SCENE: OPTIONS ───────────────────────────────
        else if (sm.current_scene == SCENE_OPTIONS) {
            Scene_UpdateOptions(&sm, &settings);

            BeginDrawing();
            Scene_DrawOptions(&sm, &settings);
            EndDrawing();
        }

        // ─── SCENE: HIGH SCORES ───────────────────────────
        else if (sm.current_scene == SCENE_HIGH_SCORES) {
            Scene_UpdateHighScores(&sm);

            BeginDrawing();
            Scene_DrawHighScores(&sm, &settings);
            EndDrawing();
        }
    }

    // Save settings on exit
    Save_SaveSettings(&settings);

    Audio_Unload();
    CloseWindow();

    return 0;
}
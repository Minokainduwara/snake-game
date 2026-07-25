#include "raylib.h"
#include "game.h"
#include "audio.h"
#include "scene.h"
#include "save.h"
#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH  (GRID_WIDTH * CELL_SIZE)
#define SCREEN_HEIGHT (GRID_HEIGHT * CELL_SIZE + 100)  // extra space for score + powerup bars

int main(void) {
    srand((unsigned int)time(NULL));

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Snake Game");
    SetTargetFPS(60);
    SetExitKey(0);  // Disable ESC from closing window — we handle ESC ourselves

    Audio_Init();

    // Load settings
    GameSettings settings;
    Save_LoadSettings(&settings);

    // Scene manager
    SceneManager sm;
    Scene_Init(&sm);

    // Check if there's a saved game to resume
    GameSnapshot saved_snap;
    sm.has_saved_game = Save_LoadGameState(&saved_snap);

    // Game state
    SnakeGame game;
    int game_was_initialized = 0;  // 0 = not yet, 1 = ready to play
    float tick_timer = 0.0f;
    int food_needs_respawn = 0;

    while (!WindowShouldClose() && sm.current_scene != SCENE_EXIT) {
        // ─── SCENE: MENU ──────────────────────────────────
        if (sm.current_scene == SCENE_MENU) {
            Scene_UpdateMenu(&sm, &settings);

            // If user selected Play/Resume, prepare the game
            if (sm.current_scene == SCENE_PLAYING) {
                if (sm.has_saved_game) {
                    // Resume from saved state
                    Game_RestoreSnapshot(&game, &saved_snap, settings.mode);
                    game.difficulty = settings.difficulty;  // Ensure difficulty is set from settings
                    game_was_initialized = 1;
                    tick_timer = 0.0f;
                    food_needs_respawn = 0;
                } else {
                    // Start fresh
                    game_was_initialized = 0;
                }
            }

            BeginDrawing();
            Scene_DrawMenu(&sm, &settings);
            EndDrawing();
        }
        // ─── SCENE: MODE SELECT ────────────────────────────
        else if (sm.current_scene == SCENE_MODE_SELECT) {
            Scene_UpdateModeSelect(&sm, &settings);

            BeginDrawing();
            Scene_DrawModeSelect(&sm, &settings);
            EndDrawing();
        }

        // ─── SCENE: PLAYING ───────────────────────────────
        else if (sm.current_scene == SCENE_PLAYING) {
            // Initialize game if not yet initialized
            if (!game_was_initialized) {
                Game_Init(&game, settings.mode);
                game.difficulty = settings.difficulty;
                if (settings.sound_enabled) Audio_PlayStart();
                game_was_initialized = 1;
                tick_timer = 0.0f;
                food_needs_respawn = 0;
            }

            // Input
            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))    game.next_dir = DIR_UP;
            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))  game.next_dir = DIR_DOWN;
            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A))  game.next_dir = DIR_LEFT;
            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) game.next_dir = DIR_RIGHT;

            // ESC to save game and return to menu
            if (IsKeyPressed(KEY_ESCAPE)) {
                Game_TakeSnapshot(&game, &saved_snap, settings.mode);
                Save_SaveGameState(&saved_snap);
                sm.has_saved_game = 1;
                sm.current_scene = SCENE_MENU;
            }

            // Update
            float tick_rate = Game_GetTickRate(settings.difficulty, settings.mode, 
                                               game.survival_level, game.speed_boost_timer, game.slow_timer);
            tick_timer += GetFrameTime();
            if (tick_timer >= tick_rate) {
                tick_timer = 0.0f;

                if (food_needs_respawn) {
                    // Ensure food doesn't spawn on snake or obstacles
                    int attempts = 0;
                    do {
                        game.food.x = rand() % GRID_WIDTH;
                        game.food.y = rand() % GRID_HEIGHT;
                        attempts++;
                    } while (Game_ShouldSpawnFood(&game) && attempts < 100);
                    food_needs_respawn = 0;
                }

                int prev_length = game.length;
                Game_Update(&game, settings.mode, tick_rate);

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

                    // Clear saved game state on game over
                    Save_ClearGameState();
                    sm.has_saved_game = 0;
                }
            }

            // Restart on R
            if (game.game_over && IsKeyPressed(KEY_R)) {
                Game_Init(&game, settings.mode);
                game.difficulty = settings.difficulty;
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

            // Show difficulty and mode in corner
            DrawText(TextFormat("[%s] [%s]", Game_DifficultyName(settings.difficulty), GameModeName(settings.mode)),
                     SCREEN_WIDTH - MeasureText(TextFormat("[%s] [%s]", Game_DifficultyName(settings.difficulty), GameModeName(settings.mode)), 15) - 10,
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

        // ─── SCENE: MODE SELECT ────────────────────────────
        else if (sm.current_scene == SCENE_MODE_SELECT) {
            Scene_UpdateModeSelect(&sm, &settings);

            BeginDrawing();
            Scene_DrawModeSelect(&sm, &settings);
            EndDrawing();
        }

        // ─── SCENE: HIGH SCORES ───────────────────────────
        else if (sm.current_scene == SCENE_HIGH_SCORES) {
            Scene_UpdateHighScores(&sm);

            BeginDrawing();
            Scene_DrawHighScores(&sm, &settings);
            EndDrawing();
        }

        // ─── SCENE: MANUAL ─────────────────────────────────────────
        else if (sm.current_scene == SCENE_MANUAL) {
            Scene_UpdateManual(&sm, &settings);

            BeginDrawing();
            Scene_DrawManual(&sm, &settings);
            EndDrawing();
        }
    }

    // Save settings on exit
    Save_SaveSettings(&settings);

    Audio_Unload();
    CloseWindow();

    return 0;
}
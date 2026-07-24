#include "raylib.h"
#include "game.h"
#include "audio.h"
#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH  (GRID_WIDTH * CELL_SIZE)
#define SCREEN_HEIGHT (GRID_HEIGHT * CELL_SIZE + 50)  // extra space for score

#define TICK_RATE 0.15f  // seconds per game tick

int main(void) {
    srand((unsigned int)time(NULL));

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Snake Game");
    SetTargetFPS(60);

    Audio_Init();

    SnakeGame game;
    Game_Init(&game);
    Audio_PlayStart();

    float tick_timer = 0.0f;
    int food_needs_respawn = 0;

    while (!WindowShouldClose()) {
        // --- Input ---
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))    game.next_dir = DIR_UP;
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))  game.next_dir = DIR_DOWN;
        if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A))  game.next_dir = DIR_LEFT;
        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) game.next_dir = DIR_RIGHT;

        if (game.game_over && IsKeyPressed(KEY_R)) {
            Game_Init(&game);
            Audio_PlayStart();
            tick_timer = 0.0f;
            food_needs_respawn = 0;
        }

        // --- Update ---
        tick_timer += GetFrameTime();
        if (tick_timer >= TICK_RATE) {
            tick_timer = 0.0f;

            // Check if food needs respawn (overlaps snake)
            if (food_needs_respawn) {
                game.food.x = rand() % GRID_WIDTH;
                game.food.y = rand() % GRID_HEIGHT;
                food_needs_respawn = 0;
            }

            // Store previous length to detect eating
            int prev_length = game.length;

            Game_Update(&game);

            // Play eat sound if snake grew
            if (game.length > prev_length) {
                Audio_PlayEat();
                food_needs_respawn = 1;
            }

            // Play death sound
            if (game.game_over && prev_length > 0) {
                Audio_PlayDeath();
            }
        }

        // --- Draw ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        Game_Draw(&game);

        EndDrawing();
    }

    Audio_Unload();
    CloseWindow();

    return 0;
}
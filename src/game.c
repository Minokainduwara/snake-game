#include "game.h"
#include <stdlib.h>
#include <time.h>
#include "raylib.h"

void Game_Init(SnakeGame *game) {
    game->length = 3;
    game->dir = DIR_RIGHT;
    game->next_dir = DIR_RIGHT;
    game->score = 0;
    game->game_over = 0;

    // Initialize snake body
    for (int i = 0; i < game->length; i++) {
        game->body[i].x = GRID_WIDTH / 2 - i;
        game->body[i].y = GRID_HEIGHT / 2;
    }

    // Spawn first food
    game->food.x = rand() % GRID_WIDTH;
    game->food.y = rand() % GRID_HEIGHT;
}

void Game_Update(SnakeGame *game) {
    if (game->game_over) return;

    // Apply queued direction (no 180-degree turns)
    if ((game->next_dir == DIR_UP && game->dir != DIR_DOWN) ||
        (game->next_dir == DIR_DOWN && game->dir != DIR_UP) ||
        (game->next_dir == DIR_LEFT && game->dir != DIR_RIGHT) ||
        (game->next_dir == DIR_RIGHT && game->dir != DIR_LEFT)) {
        game->dir = game->next_dir;
    }

    // Calculate new head position
    Position new_head = game->body[0];
    switch (game->dir) {
        case DIR_UP:    new_head.y--; break;
        case DIR_DOWN:  new_head.y++; break;
        case DIR_LEFT:  new_head.x--; break;
        case DIR_RIGHT: new_head.x++; break;
        default: break;
    }

    // Check wall collision
    if (new_head.x < 0 || new_head.x >= GRID_WIDTH ||
        new_head.y < 0 || new_head.y >= GRID_HEIGHT) {
        game->game_over = 1;
        return;
    }

    // Check self collision
    for (int i = 0; i < game->length; i++) {
        if (game->body[i].x == new_head.x && game->body[i].y == new_head.y) {
            game->game_over = 1;
            return;
        }
    }

    // Check food collision
    int ate = (new_head.x == game->food.x && new_head.y == game->food.y);

    // Move body: shift all segments forward
    for (int i = game->length - 1; i > 0; i--) {
        game->body[i] = game->body[i - 1];
    }
    game->body[0] = new_head;

    if (ate) {
        // Grow snake by adding a new segment at the tail
        game->length++;
        game->body[game->length - 1] = game->body[game->length - 2];
        game->score += 10;
    }
}

void Game_Draw(SnakeGame *game) {
    // Draw grid lines
    for (int x = 0; x <= GRID_WIDTH; x++) {
        DrawLine(x * CELL_SIZE, 0, x * CELL_SIZE, GRID_HEIGHT * CELL_SIZE, LIGHTGRAY);
    }
    for (int y = 0; y <= GRID_HEIGHT; y++) {
        DrawLine(0, y * CELL_SIZE, GRID_WIDTH * CELL_SIZE, y * CELL_SIZE, LIGHTGRAY);
    }

    // Draw snake
    for (int i = 0; i < game->length; i++) {
        Color c = (i == 0) ? DARKGREEN : GREEN;
        DrawRectangle(game->body[i].x * CELL_SIZE + 1,
                      game->body[i].y * CELL_SIZE + 1,
                      CELL_SIZE - 2, CELL_SIZE - 2, c);
    }

    // Draw food
    DrawCircle(game->food.x * CELL_SIZE + CELL_SIZE / 2,
               game->food.y * CELL_SIZE + CELL_SIZE / 2,
               CELL_SIZE / 2 - 2, RED);

    // Draw score
    DrawText(TextFormat("Score: %d", game->score), 10, GRID_HEIGHT * CELL_SIZE + 10, 20, DARKGRAY);

    // Draw game over
    if (game->game_over) {
        const char *msg = "GAME OVER - Press R to restart";
        int text_w = MeasureText(msg, 30);
        DrawRectangle(0, GRID_HEIGHT * CELL_SIZE / 2 - 30,
                      GRID_WIDTH * CELL_SIZE, 60, Fade(BLACK, 0.7f));
        DrawText(msg, (GRID_WIDTH * CELL_SIZE - text_w) / 2,
                 GRID_HEIGHT * CELL_SIZE / 2 - 10, 30, RED);
    }
}

int Game_ShouldSpawnFood(SnakeGame *game) {
    // Check if food overlaps with snake body
    for (int i = 0; i < game->length; i++) {
        if (game->body[i].x == game->food.x && game->body[i].y == game->food.y) {
            return 1;
        }
    }
    return 0;
}
#include "game.h"
#include <stdlib.h>
#include <time.h>
#include "raylib.h"

void Game_Init(SnakeGame *game, GameMode mode) {
    game->length = 3;
    game->dir = DIR_RIGHT;
    game->next_dir = DIR_RIGHT;
    game->score = 0;
    game->game_over = 0;
    game->obstacle_count = 0;
    game->time_remaining = 120.0f;  // 2 minutes for Time Attack
    game->survival_timer = 0.0f;
    game->survival_level = 0;

    // Initialize snake body
    for (int i = 0; i < game->length; i++) {
        game->body[i].x = GRID_WIDTH / 2 - i;
        game->body[i].y = GRID_HEIGHT / 2;
    }

    // Spawn first food
    game->food.x = rand() % GRID_WIDTH;
    game->food.y = rand() % GRID_HEIGHT;

    // Generate obstacles for Maze mode
    if (mode == MODE_MAZE) {
        game->obstacle_count = 15;
        for (int i = 0; i < game->obstacle_count; i++) {
            // Avoid spawning near snake start position
            do {
                game->obstacles[i].x = rand() % GRID_WIDTH;
                game->obstacles[i].y = rand() % GRID_HEIGHT;
            } while (game->obstacles[i].x < GRID_WIDTH / 2 + 3 && game->obstacles[i].y == GRID_HEIGHT / 2);
        }
    }
}

int Game_IsObstacle(const SnakeGame *game, int x, int y) {
    for (int i = 0; i < game->obstacle_count; i++) {
        if (game->obstacles[i].x == x && game->obstacles[i].y == y) {
            return 1;
        }
    }
    return 0;
}

void Game_Update(SnakeGame *game, GameMode mode, float tick_rate) {
    if (game->game_over) return;

    // Time Attack mode: countdown timer
    if (mode == MODE_TIME_ATTACK) {
        game->time_remaining -= tick_rate;
        if (game->time_remaining <= 0.0f) {
            game->time_remaining = 0.0f;
            game->game_over = 1;
            return;
        }
    }

    // Survival mode: increase speed every 30 seconds
    if (mode == MODE_SURVIVAL) {
        game->survival_timer += tick_rate;
        if (game->survival_timer >= 30.0f) {
            game->survival_timer = 0.0f;
            game->survival_level++;
        }
    }

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

    // No-Wall Mode: wrap around edges
    if (mode == MODE_NO_WALL) {
        if (new_head.x < 0) new_head.x = GRID_WIDTH - 1;
        if (new_head.x >= GRID_WIDTH) new_head.x = 0;
        if (new_head.y < 0) new_head.y = GRID_HEIGHT - 1;
        if (new_head.y >= GRID_HEIGHT) new_head.y = 0;
    } 
    // Other modes: wall collision
    else {
        if (new_head.x < 0 || new_head.x >= GRID_WIDTH ||
            new_head.y < 0 || new_head.y >= GRID_HEIGHT) {
            game->game_over = 1;
            return;
        }
    }

    // Check obstacle collision (Maze mode)
    if (mode == MODE_MAZE && Game_IsObstacle(game, new_head.x, new_head.y)) {
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

void Game_Draw(SnakeGame *game, const GameSettings *settings) {
    // Draw grid lines (optional)
    if (!settings || settings->show_grid) {
        for (int x = 0; x <= GRID_WIDTH; x++) {
            DrawLine(x * CELL_SIZE, 0, x * CELL_SIZE, GRID_HEIGHT * CELL_SIZE, LIGHTGRAY);
        }
        for (int y = 0; y <= GRID_HEIGHT; y++) {
            DrawLine(0, y * CELL_SIZE, GRID_WIDTH * CELL_SIZE, y * CELL_SIZE, LIGHTGRAY);
        }
    }

    // Draw obstacles (Maze mode)
    if (settings->mode == MODE_MAZE) {
        for (int i = 0; i < game->obstacle_count; i++) {
            DrawRectangle(game->obstacles[i].x * CELL_SIZE + 1,
                          game->obstacles[i].y * CELL_SIZE + 1,
                          CELL_SIZE - 2, CELL_SIZE - 2, DARKGRAY);
        }
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

    // Draw mode-specific HUD
    if (settings->mode == MODE_TIME_ATTACK) {
        DrawText(TextFormat("Time: %.1f", game->time_remaining), 
                 GRID_WIDTH * CELL_SIZE / 2 - 50, GRID_HEIGHT * CELL_SIZE + 10, 20, 
                 game->time_remaining < 10.0f ? RED : DARKGRAY);
    } else if (settings->mode == MODE_SURVIVAL) {
        DrawText(TextFormat("Level: %d", game->survival_level + 1),
                 GRID_WIDTH * CELL_SIZE / 2 - 50, GRID_HEIGHT * CELL_SIZE + 10, 20, DARKGRAY);
    }

    // Draw game over
    if (game->game_over) {
        const char *msg = "GAME OVER - Press R to restart or ESC for menu";
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
    // Check if food overlaps with obstacles (Maze mode)
    for (int i = 0; i < game->obstacle_count; i++) {
        if (game->obstacles[i].x == game->food.x && game->obstacles[i].y == game->food.y) {
            return 1;
        }
    }
    return 0;
}

float Game_GetTickRate(Difficulty diff, GameMode mode, int survival_level) {
    float base_rate;
    switch (diff) {
        case DIFF_EASY:   base_rate = 0.20f; break;
        case DIFF_MEDIUM: base_rate = 0.15f; break;
        case DIFF_HARD:   base_rate = 0.10f; break;
        case DIFF_EXPERT: base_rate = 0.06f; break;
        default:          base_rate = 0.15f; break;
    }

    // Survival mode: speed increases with level
    if (mode == MODE_SURVIVAL && survival_level > 0) {
        base_rate *= (1.0f - (survival_level * 0.1f));  // 10% faster each level
        if (base_rate < 0.03f) base_rate = 0.03f;  // cap max speed
    }

    // Hardcore mode: always fast
    if (mode == MODE_HARDCORE) {
        base_rate = 0.06f;
    }

    return base_rate;
}

const char *Game_DifficultyName(Difficulty diff) {
    switch (diff) {
        case DIFF_EASY:   return "Easy";
        case DIFF_MEDIUM: return "Medium";
        case DIFF_HARD:   return "Hard";
        case DIFF_EXPERT: return "Expert";
        default:          return "Unknown";
    }
}

const char *GameModeName(GameMode mode) {
    switch (mode) {
        case MODE_CLASSIC:     return "Classic";
        case MODE_TIME_ATTACK: return "Time Attack";
        case MODE_SURVIVAL:    return "Survival";
        case MODE_MAZE:        return "Maze";
        case MODE_NO_WALL:     return "No-Wall";
        case MODE_HARDCORE:    return "Hardcore";
        default:               return "Unknown";
    }
}

void Game_TakeSnapshot(const SnakeGame *game, GameSnapshot *snap, GameMode mode) {
    for (int i = 0; i < game->length; i++) {
        snap->body[i] = game->body[i];
    }
    snap->length = game->length;
    snap->dir = game->dir;
    snap->next_dir = game->next_dir;
    snap->food = game->food;
    snap->score = game->score;
    snap->game_over = game->game_over;
    snap->valid = 1;
    snap->mode = mode;
    snap->time_remaining = game->time_remaining;
    snap->survival_timer = game->survival_timer;
    snap->survival_level = game->survival_level;
    for (int i = 0; i < game->obstacle_count; i++) {
        snap->obstacles[i] = game->obstacles[i];
    }
    snap->obstacle_count = game->obstacle_count;
}

void Game_RestoreSnapshot(SnakeGame *game, const GameSnapshot *snap, GameMode mode) {
    for (int i = 0; i < snap->length; i++) {
        game->body[i] = snap->body[i];
    }
    game->length = snap->length;
    game->dir = snap->dir;
    game->next_dir = snap->next_dir;
    game->food = snap->food;
    game->score = snap->score;
    game->game_over = snap->game_over;
    game->time_remaining = snap->time_remaining;
    game->survival_timer = snap->survival_timer;
    game->survival_level = snap->survival_level;
    for (int i = 0; i < snap->obstacle_count; i++) {
        game->obstacles[i] = snap->obstacles[i];
    }
    game->obstacle_count = snap->obstacle_count;
}
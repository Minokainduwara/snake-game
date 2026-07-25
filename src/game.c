#include "game.h"
#include <stdlib.h>
#include <time.h>
#include "raylib.h"

// Forward declarations
float Game_GetPowerupDurationForDifficulty(Difficulty diff);
static void DrawHealthBar(int x, int y, int width, int height, float percentage, Color bar_color);

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
    game->speed_boost_timer = 0.0f;
    game->slow_timer = 0.0f;
    game->special_food_active = 0;
    game->special_food_timer = 5.0f;  // First special food appears after 5 seconds
    game->special_food_duration = 8.0f;  // Special food stays for 8 seconds
    game->difficulty = DIFF_MEDIUM;  // Default, will be set by caller

    // Initialize snake body
    for (int i = 0; i < game->length; i++) {
        game->body[i].x = GRID_WIDTH / 2 - i;
        game->body[i].y = GRID_HEIGHT / 2;
    }

    // Spawn regular apple (always present)
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

// Helper function to spawn regular apple at random location
static void SpawnApple(SnakeGame *game) {
    int attempts = 0;
    int max_attempts = 1000;
    
    do {
        game->food.x = rand() % GRID_WIDTH;
        game->food.y = rand() % GRID_HEIGHT;
        attempts++;
    } while (Game_ShouldSpawnFood(game) && attempts < max_attempts);
}

void Game_Update(SnakeGame *game, GameMode mode, float tick_rate) {
    if (game->game_over) return;

    // Update effect timers
    if (game->speed_boost_timer > 0.0f) {
        game->speed_boost_timer -= tick_rate;
        if (game->speed_boost_timer <= 0.0f) {
            game->speed_boost_timer = 0.0f;
        }
    }
    if (game->slow_timer > 0.0f) {
        game->slow_timer -= tick_rate;
        if (game->slow_timer <= 0.0f) {
            game->slow_timer = 0.0f;
        }
    }

    // Update special food timer
    if (game->special_food_active) {
        game->special_food_duration -= tick_rate;
        if (game->special_food_duration <= 0.0f) {
            // Special food expired, set timer for next one
            game->special_food_active = 0;
            game->special_food_timer = 8.0f + (rand() % 7);  // 8-15 seconds until next
        }
    } else {
        game->special_food_timer -= tick_rate;
        if (game->special_food_timer <= 0.0f) {
            // Spawn new special food
            game->special_food_active = 1;
            game->special_food_duration = 8.0f;  // Lasts 8 seconds
            
            // Random position
            int attempts = 0;
            do {
                game->special_food.x = rand() % GRID_WIDTH;
                game->special_food.y = rand() % GRID_HEIGHT;
                attempts++;
            } while (attempts < 100 && 
                     (game->special_food.x == game->food.x && game->special_food.y == game->food.y));
            
            // Random special type (not apple)
            int rand_val = rand() % 5;
            if (rand_val == 0) game->special_food_type = FOOD_GOLDEN_APPLE;
            else if (rand_val == 1) game->special_food_type = FOOD_SPEED;
            else if (rand_val == 2) game->special_food_type = FOOD_ICE;
            else if (rand_val == 3) game->special_food_type = FOOD_POISON;
            else game->special_food_type = FOOD_HEART;
        }
    }

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

    // Check food collision (regular apple)
    int ate_apple = (new_head.x == game->food.x && new_head.y == game->food.y);
    
    // Check special food collision
    int ate_special = 0;
    if (game->special_food_active) {
        ate_special = (new_head.x == game->special_food.x && new_head.y == game->special_food.y);
    }

    // Move body: shift all segments forward
    for (int i = game->length - 1; i > 0; i--) {
        game->body[i] = game->body[i - 1];
    }
    game->body[0] = new_head;

    // Handle apple collision
    if (ate_apple) {
        game->score += 10;
        game->length++;
        game->body[game->length - 1] = game->body[game->length - 2];
        SpawnApple(game);
    }
    
    // Handle special food collision
    if (ate_special) {
        switch (game->special_food_type) {
            case FOOD_GOLDEN_APPLE:
                game->score += 50;
                game->length++;
                game->body[game->length - 1] = game->body[game->length - 2];
                break;
                
            case FOOD_SPEED:
                game->score += 10;
                game->length++;
                game->body[game->length - 1] = game->body[game->length - 2];
                // Duration scales with difficulty
                game->speed_boost_timer = 5.0f * Game_GetPowerupDurationForDifficulty(game->difficulty);
                game->slow_timer = 0.0f;
                break;
                
            case FOOD_ICE:
                game->score += 10;
                game->length++;
                game->body[game->length - 1] = game->body[game->length - 2];
                // Duration scales with difficulty
                game->slow_timer = 5.0f * Game_GetPowerupDurationForDifficulty(game->difficulty);
                game->speed_boost_timer = 0.0f;
                break;
                
            case FOOD_POISON:
                game->score -= 10;
                if (game->score < 0) game->score = 0;
                if (game->length > 3) {
                    game->length -= 2;
                }
                break;
                
            case FOOD_HEART:
                game->score += 20;
                game->length++;
                game->body[game->length - 1] = game->body[game->length - 2];
                break;
        }
        game->special_food_active = 0;
        game->special_food_timer = 8.0f + (rand() % 7);  // 8-15 seconds until next
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

    // Draw regular apple (always present)
    int food_x = game->food.x * CELL_SIZE + CELL_SIZE / 2;
    int food_y = game->food.y * CELL_SIZE + CELL_SIZE / 2;
    int food_radius = CELL_SIZE / 2 - 4;
    
    // Apple: red circle with stem
    DrawCircle(food_x, food_y, food_radius, RED);
    DrawCircleLines(food_x, food_y, food_radius, (Color){ 150, 0, 0, 255 });
    DrawLine(food_x, food_y - food_radius, food_x + 3, food_y - food_radius - 5, BROWN);

    // Draw special food (if active)
    if (game->special_food_active) {
        int special_x = game->special_food.x * CELL_SIZE + CELL_SIZE / 2;
        int special_y = game->special_food.y * CELL_SIZE + CELL_SIZE / 2;
        
        switch (game->special_food_type) {
            case FOOD_GOLDEN_APPLE: {
                DrawCircle(special_x, special_y, food_radius + 2, GOLD);
                DrawCircle(special_x, special_y, food_radius, YELLOW);
                DrawCircleLines(special_x, special_y, food_radius, ORANGE);
                DrawText("★", special_x - 8, special_y - 8, 16, WHITE);
                break;
            }
            case FOOD_SPEED: {
                DrawCircle(special_x, special_y, food_radius, YELLOW);
                DrawCircleLines(special_x, special_y, food_radius, ORANGE);
                DrawText("!", special_x - 6, special_y - 10, 20, BLACK);
                break;
            }
            case FOOD_ICE: {
                DrawCircle(special_x, special_y, food_radius, SKYBLUE);
                DrawCircleLines(special_x, special_y, food_radius, BLUE);
                DrawLine(special_x, special_y - food_radius, special_x, special_y + food_radius, (Color){ 200, 240, 255, 255 });
                DrawLine(special_x - food_radius, special_y, special_x + food_radius, special_y, (Color){ 200, 240, 255, 255 });
                break;
            }
            case FOOD_POISON: {
                DrawCircle(special_x, special_y, food_radius, DARKGREEN);
                DrawCircleLines(special_x, special_y, food_radius, (Color){ 0, 80, 0, 255 });
                DrawText("X", special_x - 8, special_y - 8, 16, BLACK);
                break;
            }
            case FOOD_HEART: {
                DrawCircle(special_x - 6, special_y - 4, food_radius / 2, PINK);
                DrawCircle(special_x + 6, special_y - 4, food_radius / 2, PINK);
                DrawTriangle((Vector2){special_x, special_y + 8},
                            (Vector2){special_x - food_radius, special_y - 4},
                            (Vector2){special_x + food_radius, special_y - 4},
                            PINK);
                DrawCircleLines(special_x - 6, special_y - 4, food_radius / 2, RED);
                DrawCircleLines(special_x + 6, special_y - 4, food_radius / 2, RED);
                break;
            }
        }
    }

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
    
    // Draw powerup health bars (HUD area below grid)
    int bar_x = 10;
    int bar_width = 150;
    int bar_height = 14;
    int hud_start_y = GRID_HEIGHT * CELL_SIZE + 5;
    int current_y = hud_start_y + 30;  // Below score which is at hud_start_y + 5
    
    // Speed boost health bar
    if (game->speed_boost_timer > 0.0f) {
        float speed_percentage = game->speed_boost_timer / (5.0f * Game_GetPowerupDurationForDifficulty(settings->difficulty));
        DrawHealthBar(bar_x, current_y, bar_width, bar_height, speed_percentage, YELLOW);
        DrawText("SPEED", bar_x, current_y - 12, 12, YELLOW);
        current_y += bar_height + 8;
    }
    
    // Slow effect health bar
    if (game->slow_timer > 0.0f) {
        float slow_percentage = game->slow_timer / (5.0f * Game_GetPowerupDurationForDifficulty(settings->difficulty));
        DrawHealthBar(bar_x, current_y, bar_width, bar_height, slow_percentage, SKYBLUE);
        DrawText("SLOW", bar_x, current_y - 12, 12, SKYBLUE);
    }

    // Draw game over
    if (game->game_over) {
        const char *line1 = "GAME OVER";
        const char *line2 = "Press R to restart or ESC for menu";
        int text_w1 = MeasureText(line1, 40);
        int text_w2 = MeasureText(line2, 20);
        int screen_h = GRID_HEIGHT * CELL_SIZE + 100;
        int popup_height = 120;
        int popup_y = (screen_h - popup_height) / 2;
        
        // Draw semi-transparent background covering the entire screen
        DrawRectangle(0, 0, GRID_WIDTH * CELL_SIZE, screen_h, Fade(BLACK, 0.7f));
        
        // Draw popup box
        DrawRectangle(0, popup_y, GRID_WIDTH * CELL_SIZE, popup_height, (Color){ 40, 20, 20, 255 });
        DrawRectangleLines(0, popup_y, GRID_WIDTH * CELL_SIZE, popup_height, RED);
        
        // Draw text centered in popup (two lines)
        DrawText(line1, (GRID_WIDTH * CELL_SIZE - text_w1) / 2,
                 popup_y + 30, 40, RED);
        DrawText(line2, (GRID_WIDTH * CELL_SIZE - text_w2) / 2,
                 popup_y + 75, 20, RAYWHITE);
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

float Game_GetTickRate(Difficulty diff, GameMode mode, int survival_level, float speed_boost_timer, float slow_timer) {
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

    // Apply speed boost (2x faster = half the tick rate)
    if (speed_boost_timer > 0.0f) {
        base_rate *= 0.5f;
    }
    
    // Apply slow effect (0.5x speed = double the tick rate)
    if (slow_timer > 0.0f) {
        base_rate *= 2.0f;
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

float Game_GetPowerupDuration(GameMode mode) {
    (void)mode;  // Unused for now, but kept for consistency
    // Base duration is 5 seconds
    // This will be multiplied by difficulty factor in the caller
    return 1.0f;
}

float Game_GetPowerupDurationForDifficulty(Difficulty diff) {
    switch (diff) {
        case DIFF_EASY:   return 1.6f;  // 8 seconds
        case DIFF_MEDIUM: return 1.0f;  // 5 seconds
        case DIFF_HARD:   return 0.75f; // 3.75 seconds
        case DIFF_EXPERT: return 0.5f;  // 2.5 seconds
        default:          return 1.0f;
    }
}

// Helper function to draw a health bar
static void DrawHealthBar(int x, int y, int width, int height, float percentage, Color bar_color) {
    // Clamp percentage
    if (percentage < 0.0f) percentage = 0.0f;
    if (percentage > 1.0f) percentage = 1.0f;
    
    // Draw background
    DrawRectangle(x, y, width, height, (Color){ 50, 50, 50, 255 });
    DrawRectangleLines(x, y, width, height, (Color){ 150, 150, 150, 255 });
    
    // Draw filled portion
    int fill_width = (int)(width * percentage);
    if (fill_width > 0) {
        DrawRectangle(x + 1, y + 1, fill_width - 2, height - 2, bar_color);
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
    snap->speed_boost_timer = game->speed_boost_timer;
    snap->slow_timer = game->slow_timer;
    snap->special_food = game->special_food;
    snap->special_food_type = game->special_food_type;
    snap->special_food_timer = game->special_food_timer;
    snap->special_food_duration = game->special_food_duration;
    snap->special_food_active = game->special_food_active;
    snap->difficulty = game->difficulty;
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
    game->speed_boost_timer = snap->speed_boost_timer;
    game->slow_timer = snap->slow_timer;
    game->special_food = snap->special_food;
    game->special_food_type = snap->special_food_type;
    game->special_food_timer = snap->special_food_timer;
    game->special_food_duration = snap->special_food_duration;
    game->special_food_active = snap->special_food_active;
    game->difficulty = snap->difficulty;
}

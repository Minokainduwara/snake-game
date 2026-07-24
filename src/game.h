#ifndef GAME_H
#define GAME_H

#define GRID_WIDTH  20
#define GRID_HEIGHT 20
#define CELL_SIZE   30
#define MAX_SNAKE_LENGTH (GRID_WIDTH * GRID_HEIGHT)

typedef enum {
    DIR_NONE = 0,
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction;

typedef enum {
    DIFF_EASY = 0,
    DIFF_MEDIUM,
    DIFF_HARD,
    DIFF_EXPERT
} Difficulty;

typedef struct {
    int x;
    int y;
} Position;

typedef struct {
    Position body[MAX_SNAKE_LENGTH];
    int length;
    Direction dir;
    Direction next_dir;
    Position food;
    int score;
    int game_over;
} SnakeGame;

typedef struct {
    int high_score;
    Difficulty difficulty;
    int sound_enabled;
    int show_grid;
} GameSettings;

void Game_Init(SnakeGame *game);
void Game_Update(SnakeGame *game);
void Game_Draw(SnakeGame *game, const GameSettings *settings);
int Game_ShouldSpawnFood(SnakeGame *game);
float Game_GetTickRate(Difficulty diff);
const char *Game_DifficultyName(Difficulty diff);

#endif // GAME_H
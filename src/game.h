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

void Game_Init(SnakeGame *game);
void Game_Update(SnakeGame *game);
void Game_Draw(SnakeGame *game);
int Game_ShouldSpawnFood(SnakeGame *game);

#endif // GAME_H
#ifndef GAME_H
#define GAME_H

#define GRID_WIDTH  20
#define GRID_HEIGHT 20
#define CELL_SIZE   30
#define MAX_SNAKE_LENGTH (GRID_WIDTH * GRID_HEIGHT)
#define MAX_OBSTACLES 50

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

typedef enum {
    FOOD_APPLE = 0,        // +10 points, normal growth
    FOOD_GOLDEN_APPLE,     // +50 points, normal growth
    FOOD_SPEED,            // +10 points, temporary speed boost
    FOOD_ICE,              // +10 points, temporary slow
    FOOD_POISON,           // -10 points, lose length
    FOOD_HEART             // +20 points
} FoodType;

typedef enum {
    MODE_CLASSIC = 0,
    MODE_TIME_ATTACK,
    MODE_SURVIVAL,
    MODE_MAZE,
    MODE_NO_WALL,
    MODE_HARDCORE
} GameMode;

typedef struct {
    int x;
    int y;
} Position;

typedef struct {
    Position body[MAX_SNAKE_LENGTH];
    int length;
    Direction dir;
    Direction next_dir;
    Position food;              // Regular apple (always present)
    Position special_food;      // Special food (temporary)
    FoodType special_food_type;
    int score;
    int game_over;
    Position obstacles[MAX_OBSTACLES];
    int obstacle_count;
    float time_remaining;
    float survival_timer;
    int survival_level;
    float speed_boost_timer;    // Remaining time for speed boost
    float slow_timer;           // Remaining time for slow effect
    float special_food_timer;   // Time until special food appears
    float special_food_duration; // How long special food stays
    int special_food_active;    // 1 if special food is on screen
    Difficulty difficulty;      // Current difficulty for powerup scaling
} SnakeGame;

typedef struct {
    int high_score;
    Difficulty difficulty;
    int sound_enabled;
    int show_grid;
    GameMode mode;
} GameSettings;

// Snapshot of game state for save/resume
typedef struct {
    Position body[MAX_SNAKE_LENGTH];
    int length;
    Direction dir;
    Direction next_dir;
    Position food;
    Position special_food;
    FoodType special_food_type;
    int score;
    int game_over;
    int valid;
    GameMode mode;
    float time_remaining;
    float survival_timer;
    int survival_level;
    Position obstacles[MAX_OBSTACLES];
    int obstacle_count;
    float speed_boost_timer;
    float slow_timer;
    float special_food_timer;
    float special_food_duration;
    int special_food_active;
    Difficulty difficulty;       // Difficulty for powerup scaling on resume
} GameSnapshot;

void Game_Init(SnakeGame *game, GameMode mode);
void Game_Update(SnakeGame *game, GameMode mode, float tick_rate);
void Game_Draw(SnakeGame *game, const GameSettings *settings);
int Game_ShouldSpawnFood(SnakeGame *game);
float Game_GetTickRate(Difficulty diff, GameMode mode, int survival_level, float speed_boost_timer, float slow_timer);
const char *Game_DifficultyName(Difficulty diff);
const char *GameModeName(GameMode mode);
void Game_TakeSnapshot(const SnakeGame *game, GameSnapshot *snap, GameMode mode);
void Game_RestoreSnapshot(SnakeGame *game, const GameSnapshot *snap, GameMode mode);

#endif // GAME_H
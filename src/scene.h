#ifndef SCENE_H
#define SCENE_H

#include "game.h"
#include "raylib.h"

typedef enum {
    SCENE_MENU = 0,
    SCENE_PLAYING,
    SCENE_GAME_OVER,
    SCENE_DIFFICULTY,
    SCENE_OPTIONS,
    SCENE_HIGH_SCORES,
    SCENE_EXIT
} Scene;

typedef struct {
    Scene current_scene;
    int menu_selection;       // which menu item is highlighted
    int diff_selection;       // which difficulty is highlighted
    int opt_selection;        // which option is highlighted
    int highscores_selection; // which high score item is highlighted
} SceneManager;

void Scene_Init(SceneManager *sm);
void Scene_UpdateMenu(SceneManager *sm, GameSettings *settings);
void Scene_UpdateDifficulty(SceneManager *sm, GameSettings *settings);
void Scene_UpdateOptions(SceneManager *sm, GameSettings *settings);
void Scene_UpdateHighScores(SceneManager *sm);
void Scene_DrawMenu(const SceneManager *sm, const GameSettings *settings);
void Scene_DrawDifficulty(const SceneManager *sm, const GameSettings *settings);
void Scene_DrawOptions(const SceneManager *sm, const GameSettings *settings);
void Scene_DrawHighScores(const SceneManager *sm, const GameSettings *settings);

#endif // SCENE_H
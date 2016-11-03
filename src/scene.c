// scene.c

#include "scene.h"

#include "menu.h"
#include "game.h"
#include "replay.h"


GameScene* createGameScene(enum GameSceneKind const kind, void* const data) {
    GameScene* const scene = malloc(sizeof(GameScene));
    scene->kind = kind;
    scene->data = data;
    return scene;
}

void destroyGameScene(GameScene* const scene) {
    switch (scene->kind) {
        case GSK_MENU:      destroyGameSceneMenu(scene->data);      break;
        case GSK_GAME:      destroyGameSceneGame(scene->data);      break;
        case GSK_REPLAY:    destroyGameSceneReplay(scene->data);    break;
    }
    free(scene);
}

void gameSceneDraw(GameScene* const scene, Surface* const backSurface) {
    switch (scene->kind) {
        case GSK_MENU:      gameSceneMenuDraw(scene->data, backSurface);    break;
        case GSK_GAME:      gameSceneGameDraw(scene->data, backSurface);    break;
        case GSK_REPLAY:    gameSceneReplayDraw(scene->data, backSurface);  break;
    }
}

void gameSceneMouseMove(GameScene* const scene, POINT const point) {
    switch (scene->kind) {
        case GSK_MENU:      gameSceneMenuMouseMove(scene->data, point);     break;
        case GSK_GAME:      gameSceneGameMouseMove(scene->data, point);     break;
        case GSK_REPLAY:    gameSceneReplayMouseMove(scene->data, point);   break;
    }
}

unsigned char gameSceneMouseLDown(GameScene* const scene) {
    switch (scene->kind) {
        case GSK_MENU:      return gameSceneMenuMouseLDown(scene->data);
        case GSK_GAME:      return gameSceneGameMouseLDown(scene->data);
        case GSK_REPLAY:    return gameSceneReplayMouseLDown(scene->data);
    }
    return 0;
}

unsigned char gameSceneKeyDown(GameScene* const scene, WPARAM const key) {
    switch (scene->kind) {
        case GSK_MENU:      return gameSceneMenuKeyDown(scene->data, key);
        case GSK_GAME:      return gameSceneGameKeyDown(scene->data, key);
        case GSK_REPLAY:    return gameSceneReplayKeyDown(scene->data, key);
    }
    return 0;
}

void gameSceneOnTimer(GameScene* const scene) {
    switch (scene->kind) {
        case GSK_MENU:      break;
        case GSK_GAME:      break;
        case GSK_REPLAY:    gameSceneReplayOnTimer(scene->data);    break;
    }
}

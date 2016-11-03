// scene.h

#pragma once

#include "gtips.h"


enum GameSceneKind {
    GSK_MENU,
    GSK_GAME,
    GSK_REPLAY,
};

typedef struct tagGameScene {
    enum GameSceneKind kind;
    void* data;
} GameScene;

GameScene* createGameScene(enum GameSceneKind kind, void* data);
void destroyGameScene(GameScene* scene);

void gameSceneDraw(GameScene* scene, Surface* backSurface);
void gameSceneMouseMove(GameScene* scene, POINT point);
unsigned char gameSceneMouseLDown(GameScene* scene);
unsigned char gameSceneKeyDown(GameScene* scene, WPARAM key);
void gameSceneOnTimer(GameScene* scene);

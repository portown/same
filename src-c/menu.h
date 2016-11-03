// menu.h

#pragma once

#include "gtips.h"


typedef struct tagGameSceneMenu {
    Surface* surface;
    unsigned char m_Level; // 隠し要素レベル（＝プレイヤーレベル）
    unsigned short m_Width; // 幅
    unsigned short m_Height; // 高さ
    unsigned char  m_Sel; // カーソル位置
    char           m_MaskNum; // マスクレベル
    char           m_RepNum; // リプレイナンバー
    RECT           m_rcMenu; // メニューエリア
    RECT           m_rcLeft; // 左矢印エリア
    RECT           m_rcRight; // 右矢印エリア
    Surface* menuSurface;
} GameSceneMenu;

GameSceneMenu* createGameSceneMenu(unsigned int width, unsigned int height);
void destroyGameSceneMenu(GameSceneMenu* scene);

void gameSceneMenuDraw(GameSceneMenu* scene, Surface* backSurface);
void gameSceneMenuMouseMove(GameSceneMenu* scene, POINT point);
unsigned char gameSceneMenuMouseLDown(GameSceneMenu* scene);
unsigned char gameSceneMenuKeyDown(GameSceneMenu* scene, WPARAM key);

// game.h

#pragma once

#include "gtips.h"


typedef struct tagGameSceneGame {
    Surface* surface;
    unsigned char m_Level; // 隠し要素レベル（＝プレイヤーレベル）
    unsigned char* m_Played; // リプレイ用データ
    size_t m_PlayedCapacity;
    unsigned short             m_bx, m_by; // 前回のマウス位置
    unsigned short previousPosition;
    unsigned short             m_Num; // 選択中の駒数
    unsigned short             m_Tries; // 手数
    unsigned short             m_Width; // 幅
    unsigned short             m_Height; // 高さ
    unsigned char*             m_Area; // 駒情報
    unsigned char              m_Pieces; // 残り駒数
    unsigned char              m_Groups; // 残り塊数
    unsigned char              m_Status; // ゲームの状態
    unsigned long              m_HighScore; // ハイスコア
    unsigned long              m_Score; // プレイヤースコア
    unsigned long              m_GameNum; // ゲームナンバー（乱数の種）
    RECT                       m_rcArea; // ゲーム盤エリア
    _Bool                       m_bDraw; // 連打抑え
    _Bool                       m_bReGame; // クリックフラグ
    char                       m_cMaskNum; // マスクレベル
} GameSceneGame;

GameSceneGame* createGameSceneGame(unsigned int width, unsigned int height, int maskLevel, unsigned long seed);
void destroyGameSceneGame(GameSceneGame* scene);

void gameSceneGameDraw(GameSceneGame* scene, Surface* backSurface);
void gameSceneGameMouseMove(GameSceneGame* scene, POINT point);
unsigned char gameSceneGameMouseLDown(GameSceneGame* scene);
unsigned char gameSceneGameKeyDown(GameSceneGame* scene, WPARAM key);

// menu.h

#pragma once

#include <windows.h>


typedef struct SurfaceTag* SurfacePtr;


typedef struct tagGameSceneMenu {
    SurfacePtr surface;
    unsigned char m_Level; // 隠し要素レベル（＝プレイヤーレベル）
    unsigned short m_Width; // 幅
    unsigned short m_Height; // 高さ
    unsigned char  m_Sel; // カーソル位置
    char           m_MaskNum; // マスクレベル
    char           m_RepNum; // リプレイナンバー
    RECT*          m_rcMenu; // メニューエリア
    RECT*          m_rcLeft; // 左矢印エリア
    RECT*          m_rcRight; // 右矢印エリア
    SurfacePtr menuSurface;
} GameSceneMenu;

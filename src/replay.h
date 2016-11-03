// replay.h

#pragma once

#ifdef __cplusplus
extern "C" {
#endif


typedef struct tagGameSceneReplay {
    Surface* surface;
    std::vector<unsigned char> m_Played; // リプレイ用データ
    unsigned short             m_bx, m_by; // 前回のマウス位置
    unsigned short             m_Num; // 選択中の駒数
    unsigned short             m_Tries; // 手数
    unsigned short             m_Width; // 幅
    unsigned short             m_Height; // 高さ
    unsigned char*             m_Area; // 駒情報
    unsigned char              m_Pieces; // 残り駒数
    unsigned char              m_Groups; // 残り塊数
    unsigned char              m_Status; // ゲームの状態
    unsigned long              m_Score; // プレイヤースコア
    unsigned long              m_GameNum; // ゲームナンバー（乱数の種）
    RECT                       m_rcArea; // ゲーム盤エリア
    HWND                       m_hWnd; // ウィンドウハンドル
    Surface* cursorSurface;
    char                       m_cRepNum; // リプレイナンバー
    bool                       m_bErase; // 消去フラグ
} GameSceneReplay;

GameSceneReplay* createGameSceneReplay(HWND hWnd, unsigned int width, unsigned int height, int replayNumber);
void destroyGameSceneReplay(GameSceneReplay* scene);

void gameSceneReplayDraw(GameSceneReplay* scene, Surface* backSurface);
void gameSceneReplayMouseMove(GameSceneReplay* scene, POINT point);
unsigned char gameSceneReplayMouseLDown(GameSceneReplay* scene);
unsigned char gameSceneReplayKeyDown(GameSceneReplay* scene, WPARAM key);
void gameSceneReplayOnTimer(GameSceneReplay* scene);


#ifdef __cplusplus
}
#endif

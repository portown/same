// structs.h

#pragma once

#include "gtips.h"
#include "game.h"
#include "menu.h"


// 基本クラス
class CGAME
{
public:
    virtual void draw(Surface* backSurface) = 0;
    virtual void Select(POINT) = 0;
    virtual unsigned char Click(void) = 0;

    virtual unsigned char KeyDown(WPARAM) = 0;
    virtual ~CGAME(void) {}
};

// ゲーム盤クラス
class CSAME : public CGAME
{
    GameSceneGame* data;

public:
    CSAME(unsigned short, unsigned short, char);
    void draw(Surface* backSurface) override;
    void Select(POINT) override;
    unsigned char Click(void) override;

    unsigned char KeyDown(WPARAM) override;
    ~CSAME(void) override;
};

// タイトルメニュークラス
class CMENU : public CGAME
{
    GameSceneMenu* data;

public:
    CMENU(unsigned short, unsigned short);
    void draw(Surface* backSurface) override;
    void Select(POINT) override;
    unsigned char Click(void) override;

    unsigned char KeyDown(WPARAM) override;
    ~CMENU(void) override;
};

// リプレイ再生クラス
class CREPLAY : public CGAME
{
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

    void          Onselect(unsigned short);
    void          Unselect(void);
    void          Explore(unsigned short, unsigned char);
    void          Inexplore(unsigned short);
    void          Exexplore(unsigned short);
    unsigned char Onclick(void);
    void          Check(void);
    void          VShift(unsigned short);
    void          HShift(unsigned short);
    unsigned char EndCheck(void);
    bool          CntGroups(void);
    void          AddScore(unsigned long);
    bool          LoadReplay(char);
    void          SaveReplay(char);

public:
    CREPLAY(HWND, unsigned short, unsigned short, char);
    void draw(Surface* backSurface) override;
    void Select(POINT) override;
    unsigned char Click(void) override;

    unsigned char KeyDown(WPARAM) override;
    void Replay(void);

    ~CREPLAY(void) override;
};


// EOF

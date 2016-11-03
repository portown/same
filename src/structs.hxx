// structs.h

#pragma once

#include "gtips.h"
#include "game.h"
#include "menu.h"
#include "replay.h"


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
    GameSceneReplay* data;

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

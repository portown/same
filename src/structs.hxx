// structs.h

#pragma once

#include "gtips.h"
#include "game.h"
#include "menu.h"
#include "replay.h"


// ��{�N���X
class CGAME
{
public:
    virtual void draw(Surface* backSurface) = 0;
    virtual void Select(POINT) = 0;
    virtual unsigned char Click(void) = 0;

    virtual unsigned char KeyDown(WPARAM) = 0;
    virtual ~CGAME(void) {}
};

// �Q�[���ՃN���X
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

// �^�C�g�����j���[�N���X
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

// ���v���C�Đ��N���X
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

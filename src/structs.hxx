// structs.h

#pragma once

#include "gtips.h"
#include "game.h"
#include "menu.h"


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
    Surface* surface;
    std::vector<unsigned char> m_Played; // ���v���C�p�f�[�^
    unsigned short             m_bx, m_by; // �O��̃}�E�X�ʒu
    unsigned short             m_Num; // �I�𒆂̋
    unsigned short             m_Tries; // �萔
    unsigned short             m_Width; // ��
    unsigned short             m_Height; // ����
    unsigned char*             m_Area; // ����
    unsigned char              m_Pieces; // �c��
    unsigned char              m_Groups; // �c���
    unsigned char              m_Status; // �Q�[���̏��
    unsigned long              m_Score; // �v���C���[�X�R�A
    unsigned long              m_GameNum; // �Q�[���i���o�[�i�����̎�j
    RECT                       m_rcArea; // �Q�[���ՃG���A
    HWND                       m_hWnd; // �E�B���h�E�n���h��
    Surface* cursorSurface;
    char                       m_cRepNum; // ���v���C�i���o�[
    bool                       m_bErase; // �����t���O

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

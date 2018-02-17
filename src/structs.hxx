// structs.h

#pragma once

#include "same/ui/surface.hxx"


struct CGAME
{
    virtual void Draw(same::ui::Surface& backSurface) = 0;

    virtual void          Select(POINT) = 0;
    virtual unsigned char Click(void)   = 0;

    virtual unsigned char KeyDown(WPARAM) = 0;
    virtual ~CGAME(void) {}
};

class CSAME : public CGAME
{
    std::shared_ptr<same::ui::Surface> surface_;
    unsigned char                      m_Level; // �B���v�f���x���i���v���C���[���x���j

    std::vector<unsigned char> m_Played; // ���v���C�p�f�[�^
    unsigned short             m_bx, m_by; // �O��̃}�E�X�ʒu
    unsigned short             m_Num; // �I�𒆂̋
    unsigned short             m_Tries; // �萔
    unsigned short             m_Width; // ��
    unsigned short             m_Height; // ����
    std::vector<unsigned char> m_Area; // ����
    unsigned char              m_Pieces; // �c��
    unsigned char              m_Groups; // �c���
    unsigned char              m_Status; // �Q�[���̏��
    unsigned long              m_HighScore; // �n�C�X�R�A
    unsigned long              m_Score; // �v���C���[�X�R�A
    unsigned long              m_GameNum; // �Q�[���i���o�[�i�����̎�j
    RECT                       m_rcArea; // �Q�[���ՃG���A
    bool                       m_bDraw; // �A�ŗ}��
    bool                       m_bReGame; // �N���b�N�t���O
    char                       m_cMaskNum; // �}�X�N���x��

    void          Unselect(void);
    void          Explore(unsigned short, unsigned char);
    void          Inexplore(unsigned short);
    void          Exexplore(unsigned short);
    void          Check(void);
    void          VShift(unsigned short);
    void          HShift(unsigned short);
    unsigned char EndCheck(void);
    bool          CntGroups(void);
    void          AddScore(unsigned long);
    void          LoadStatus(void);
    void          SaveStatus(void);
    void          SaveReplay(char);

public:
    CSAME(unsigned short, unsigned short, char);
    void Draw(same::ui::Surface& backSurface) override;

    void Select(POINT) override;
    unsigned char Click(void) override;

    unsigned char KeyDown(WPARAM) override;
    ~CSAME(void) override;

public:     // for tests
    CSAME(unsigned short wx, unsigned short wy, char cMaskNum, unsigned long gameNum);
    std::vector<unsigned char> makeArea(unsigned short width, unsigned short height, unsigned long gameNumber) const;
    unsigned char              getAt(unsigned short x, unsigned short y) const;
    bool                       selectsAt(unsigned short x, unsigned short y) const;
};

class CMENU : public CGAME
{
    std::shared_ptr<same::ui::Surface> surface_;
    unsigned char                      m_Level; // �B���v�f���x���i���v���C���[���x���j

    unsigned short                     m_Width; // ��
    unsigned short                     m_Height; // ����
    unsigned char                      m_Sel; // �J�[�\���ʒu
    char                               m_MaskNum; // �}�X�N���x��
    char                               m_RepNum; // ���v���C�i���o�[
    std::shared_ptr<same::ui::Surface> menuSurface_;
    RECT                               m_rcMenu; // ���j���[�G���A
    RECT                               m_rcLeft; // �����G���A
    RECT                               m_rcRight; // �E���G���A

    virtual void LoadStatus(void);
    virtual void SaveStatus(void);

public:
    CMENU(unsigned short, unsigned short);
    void Draw(same::ui::Surface& backSurface) override;

    void Select(POINT) override;
    unsigned char Click(void) override;

    unsigned char KeyDown(WPARAM) override;
    ~CMENU(void) override;
};

class CREPLAY : public CGAME
{
    std::shared_ptr<same::ui::Surface> surface_;

    std::vector<unsigned char>         m_Played; // ���v���C�p�f�[�^
    unsigned short                     m_bx, m_by; // �O��̃}�E�X�ʒu
    unsigned short                     m_Num; // �I�𒆂̋
    unsigned short                     m_Tries; // �萔
    unsigned short                     m_Width; // ��
    unsigned short                     m_Height; // ����
    std::vector<unsigned char>         m_Area; // ����
    unsigned char                      m_Pieces; // �c��
    unsigned char                      m_Groups; // �c���
    unsigned char                      m_Status; // �Q�[���̏��
    unsigned long                      m_Score; // �v���C���[�X�R�A
    unsigned long                      m_GameNum; // �Q�[���i���o�[�i�����̎�j
    std::shared_ptr<same::ui::Surface> cursorSurface_;
    RECT                               m_rcArea; // �Q�[���ՃG���A
    HWND                               m_hWnd; // �E�B���h�E�n���h��
    char                               m_cRepNum; // ���v���C�i���o�[
    bool                               m_bErase; // �����t���O

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
    void Draw(same::ui::Surface& backSurface) override;

    void Select(POINT) override;
    unsigned char Click(void) override;

    unsigned char KeyDown(WPARAM) override;
    void Replay(void);

    ~CREPLAY(void) override;
};

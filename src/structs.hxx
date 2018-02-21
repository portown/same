// structs.hxx

#ifndef STRUCTS_HXX
#define STRUCTS_HXX

#include <memory>
#include <vector>

#include <windows.h>

#include "same/game_context.hxx"
#include "same/ui/surface.hxx"


class CSAME : public same::GameState
{
    std::shared_ptr<same::ui::Surface> surface_;
    unsigned char m_Level;                      // 隠し要素レベル（＝プレイヤーレベル）

    std::vector<unsigned char> m_Played; // リプレイ用データ
    unsigned short m_bx, m_by;             // 前回のマウス位置
    unsigned short m_Num;             // 選択中の駒数
    unsigned short m_Width;             // 幅
    unsigned short m_Height;             // 高さ
    std::vector<unsigned char> m_Area; // 駒情報
    unsigned char m_Pieces;              // 残り駒数
    unsigned char m_Groups;              // 残り塊数
    unsigned char m_Status;              // ゲームの状態
    unsigned long m_HighScore;              // ハイスコア
    unsigned long m_Score;              // プレイヤースコア
    unsigned long m_GameNum;              // ゲームナンバー（乱数の種）
    RECT m_rcArea;                       // ゲーム盤エリア
    bool m_bDraw;                       // 連打抑え
    bool m_bReGame;                       // クリックフラグ
    char m_cMaskNum;                       // マスクレベル

    void          Unselect();
    void          Explore(unsigned short, unsigned char);
    void          Inexplore(unsigned short);
    void          Exexplore(unsigned short);
    void          Check();
    void          VShift(unsigned short);
    void          HShift(unsigned short);
    unsigned char EndCheck();
    bool          CntGroups();
    void          AddScore(unsigned long);
    void          LoadStatus();
    void          SaveStatus();
    void          SaveReplay(char);

public:
    CSAME(unsigned short, unsigned short, char);
    void draw(same::ui::Surface& backSurface) override;

    void          onMouseMove(::POINT const& point) override;
    unsigned char onMouseLButtonUp() override;

    unsigned char onKeyDown(::WPARAM keyCode) override;

    ~CSAME();

public:     // for tests
    CSAME(unsigned short wx, unsigned short wy, char cMaskNum, unsigned long gameNum);
    std::vector<unsigned char> makeArea(unsigned short width, unsigned short height, unsigned long gameNumber) const;
    unsigned char              getAt(unsigned short x, unsigned short y) const;
    bool                       selectsAt(unsigned short x, unsigned short y) const;
};

class CMENU : public same::GameState
{
    std::shared_ptr<same::ui::Surface> surface_;
    unsigned char m_Level;                      // 隠し要素レベル（＝プレイヤーレベル）

    unsigned short m_Width;                     // 幅
    unsigned short m_Height;                     // 高さ
    unsigned char m_Sel;                      // カーソル位置
    char m_MaskNum;                               // マスクレベル
    char m_RepNum;                               // リプレイナンバー
    std::shared_ptr<same::ui::Surface> menuSurface_;
    RECT m_rcMenu;                               // メニューエリア
    RECT m_rcLeft;                               // 左矢印エリア
    RECT m_rcRight;                               // 右矢印エリア

    virtual void LoadStatus();
    virtual void SaveStatus();

public:
    CMENU(unsigned short, unsigned short);
    void draw(same::ui::Surface& backSurface) override;

    void          onMouseMove(::POINT const& point) override;
    unsigned char onMouseLButtonUp() override;

    unsigned char onKeyDown(::WPARAM keyCode) override;

    ~CMENU();
};

class CREPLAY : public same::GameState
{
    std::shared_ptr<same::ui::Surface> surface_;

    std::vector<unsigned char> m_Played;         // リプレイ用データ
    unsigned short m_bx, m_by;                     // 前回のマウス位置
    unsigned short m_Num;                     // 選択中の駒数
    unsigned short m_Tries;                     // 手数
    unsigned short m_Width;                     // 幅
    unsigned short m_Height;                     // 高さ
    std::vector<unsigned char> m_Area;         // 駒情報
    unsigned char m_Pieces;                      // 残り駒数
    unsigned char m_Groups;                      // 残り塊数
    unsigned char m_Status;                      // ゲームの状態
    unsigned long m_Score;                      // プレイヤースコア
    unsigned long m_GameNum;                      // ゲームナンバー（乱数の種）
    std::shared_ptr<same::ui::Surface> cursorSurface_;
    RECT m_rcArea;                               // ゲーム盤エリア
    HWND m_hWnd;                               // ウィンドウハンドル
    char m_cRepNum;                               // リプレイナンバー
    bool m_bErase;                               // 消去フラグ

    void          Onselect(unsigned short);
    void          Unselect();
    void          Explore(unsigned short, unsigned char);
    void          Inexplore(unsigned short);
    void          Exexplore(unsigned short);
    unsigned char Onclick();
    void          Check();
    void          VShift(unsigned short);
    void          HShift(unsigned short);
    unsigned char EndCheck();
    bool          CntGroups();
    void          AddScore(unsigned long);
    bool          LoadReplay(char);
    void          SaveReplay(char);

public:
    CREPLAY(HWND, unsigned short, unsigned short, char);
    void draw(same::ui::Surface& backSurface) override;

    void          onMouseMove(::POINT const& point) override;
    unsigned char onMouseLButtonUp() override;

    unsigned char onKeyDown(::WPARAM keyCode) override;
    void          Replay();

    ~CREPLAY();
};

#endif  // STRUCTS_HXX

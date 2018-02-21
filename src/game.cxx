// game.cpp

#include <ctime>
#include <random>

#include "defines.hxx"
#include "structs.hxx"
#include "funcs.hxx"
#include "tstringstream.hxx"
#include "win_utils.hxx"

#include "resource.h"


CSAME::CSAME(unsigned short wx, unsigned short wy, char cMaskNum)
    : CSAME(wx, wy, cMaskNum, static_cast<unsigned long>(std::time(nullptr)))
{
}

CSAME::CSAME(unsigned short wx, unsigned short wy, char cMaskNum, unsigned long gameNum)
{
    surface_ = same::ui::Surface::fromBitmapFile(DATA(TEXT("system.bmp")));

    m_Level     = 0;
    m_HighScore = 0;

    LoadStatus();

    m_Status = GS_PLAYING;

    m_cMaskNum = cMaskNum;

    m_Width   = wx / PIX;
    m_Height  = wy / PIY;
    m_bx      = m_Width;
    m_by      = m_Height;
    m_bReGame = false;

    SetRect(&m_rcArea, 0, 0, wx, wy);

    m_Num    = 0;
    m_Pieces = m_Width * m_Height;
    m_Groups = 0;
    m_Score  = 0;

    m_GameNum = gameNum;
    m_Area    = makeArea(m_Width, m_Height, m_GameNum);

    CntGroups();

    m_Played.clear();
}

void CSAME::onFrame(same::GameContext& context, same::Input const& input)
{
    if (input.isMouseMoved())
    {
        onMouseMove(input.getMousePosition());
    }

    if (input.isMouseLButtonUp())
    {
        if (m_Status == GS_CLEAR || m_Status == GS_ALLCLEAR)
        {
            if (m_bReGame) context.changeState<CSAME>(GAMEX, GAMEY, m_cMaskNum);
            return;
        }

        if (m_Num < 2) return;
        if (m_bx >= m_Width || m_by >= m_Height) return;
        if (m_Area[m_by * m_Width + m_bx] == 0) return;

        Exexplore(m_by * m_Width + m_bx);

        m_Pieces -= m_Num;
        AddScore(ADDSCORE(m_Num));
        m_Played.push_back(static_cast<unsigned char>(m_by * m_Width + m_bx));

        Check();

        auto const cRet = EndCheck();
        switch (cRet)
        {
            case CR_NOSTATUS:
                PlaySound(ERSWAV, nullptr, SND_FILENAME | SND_SYNC);
                m_Num = 0;
                Explore(m_by * m_Width + m_bx, m_Area[m_by * m_Width + m_bx]);
                if (m_Num == 1) Unselect();
                break;

            case CR_CLEAR:
                PlaySound(CLRWAV, nullptr, SND_FILENAME | SND_ASYNC);
                m_Status = GS_CLEAR;
                break;

            case CR_ALLCLEAR:
                AddScore(1000000);
                PlaySound(CLRWAV, nullptr, SND_FILENAME | SND_ASYNC);
                m_Status = GS_ALLCLEAR;
                break;
        }

        if (cRet == CR_CLEAR || cRet == CR_ALLCLEAR)
        {
            SaveReplay('\0');
            if (m_Score >= 10000 && m_Level == m_cMaskNum)
            {
                if (++m_Level > MASKMAX + 1) m_Level = MASKMAX + 1;
            }
        }
    }
    else
    {
        if (input.isKeyDown(VK_RETURN) && (m_Status == GS_CLEAR || m_Status == GS_ALLCLEAR))
        {
            context.changeState<CSAME>(GAMEX, GAMEY, m_cMaskNum);
            return;
        }
        if (input.isKeyDown('0')) SaveReplay('0');
        if (input.isKeyDown('1')) SaveReplay('1');
        if (input.isKeyDown('2')) SaveReplay('2');
        if (input.isKeyDown('3')) SaveReplay('3');
        if (input.isKeyDown('4')) SaveReplay('4');
        if (input.isKeyDown('5')) SaveReplay('5');
        if (input.isKeyDown('6')) SaveReplay('6');
        if (input.isKeyDown('7')) SaveReplay('7');
        if (input.isKeyDown('8')) SaveReplay('8');
        if (input.isKeyDown('9')) SaveReplay('9');
        if (input.isKeyDown(VK_F8))
        {
            context.changeState<CSAME>(GAMEX, GAMEY, m_cMaskNum);
            return;
        }
        if (input.isKeyDown(VK_F12))
        {
            context.changeState<CMENU>(WINX, WINY);
            return;
        }
        if (input.isKeyDown(VK_ESCAPE))
        {
            context.finish();
            return;
        }
    }
}

void CSAME::draw(same::ui::Surface& backSurface)
{
    // ゲーム盤の描画
    if (m_cMaskNum < 4)
    {
        for (auto i = 0; i < m_Height; ++i)
        {
            for (auto j = 0; j < m_Width; ++j)
            {
                unsigned char tmp = m_Area[i * m_Width + j];

                if (tmp != 0)
                {
                    --tmp;
                    if (m_cMaskNum == 3) tmp &= 0x80;

                    if (selectsAt(j, i))
                    {
                        if (m_cMaskNum == 1) continue;

                        tmp = tmp ^ static_cast<unsigned char>(0x80);

                        BitBlt(backSurface.getDC(), j * PIX, i * PIY, PIX, PIY, surface_->getDC(), tmp * PIX, PIY * 2,
                               SRCAND);
                        BitBlt(backSurface.getDC(), j * PIX, i * PIY, PIX, PIY, surface_->getDC(), tmp * PIX, PIY,
                               SRCPAINT);
                    }
                    else
                    {
                        if (m_cMaskNum == 2) continue;

                        BitBlt(backSurface.getDC(), j * PIX, i * PIY, PIX, PIY, surface_->getDC(), tmp * 32, PIY * 2,
                               SRCAND);
                        BitBlt(backSurface.getDC(), j * PIX, i * PIY, PIX, PIY, surface_->getDC(), tmp * 32, 0,
                               SRCPAINT);
                    }
                }
            }
        }
    }

    tch::tostringstream oss;
    auto const          instance = ::GetModuleHandle(nullptr);

    // その他の描画
    oss.str({});
    oss << win::loadString(instance, IDS_SCORE).value() << m_Score;
    PutText(backSurface.getDC(), m_rcArea.right, 0, 20, RGB(255, 255, 255), oss.str());
    oss.str({});
    oss << win::loadString(instance, IDS_HIGH_SCORE).value() << m_HighScore;
    PutText(backSurface.getDC(), m_rcArea.right, 20, 20, RGB(255, 255, 255), oss.str());

    oss.str({});
    oss << win::loadString(instance, IDS_SELECTED_AREA).value() << m_Num;
    PutText(backSurface.getDC(), m_rcArea.right, 60, 20, RGB(255, 255, 255), oss.str());
    oss.str({});
    oss << win::loadString(instance, IDS_TRIES).value() << m_Played.size();
    PutText(backSurface.getDC(), m_rcArea.right, 80, 20, RGB(255, 255, 255), oss.str());
    oss.str({});
    oss << win::loadString(instance, IDS_REST_PIECES).value() << static_cast<unsigned int>(m_Pieces);
    PutText(backSurface.getDC(), m_rcArea.right, 100, 20, RGB(255, 255, 255), oss.str());
    oss.str({});
    oss << win::loadString(instance, IDS_REST_GROUPS).value() << static_cast<unsigned int>(m_Groups);
    PutText(backSurface.getDC(), m_rcArea.right, 120, 20, RGB(255, 255, 255), oss.str());

    oss.str({});
    oss << win::loadString(instance, IDS_CURSOR_X).value() << (m_bx < m_Width ? m_bx : 0);
    PutText(backSurface.getDC(), m_rcArea.right + 152, 60, 20, RGB(255, 255, 255), oss.str());
    oss.str({});
    oss << win::loadString(instance, IDS_CURSOR_Y).value() << (m_by < m_Height ? m_by : 0);
    PutText(backSurface.getDC(), m_rcArea.right + 152, 80, 20, RGB(255, 255, 255), oss.str());

    oss.str({});
    oss << win::loadString(instance, IDS_GAME_NUMBER).value() << m_GameNum;
    PutText(backSurface.getDC(), m_rcArea.right + 160, m_rcArea.bottom - 12, 12, RGB(255, 255, 255), oss.str());

    // クリア後の描画
    if (m_Status == GS_CLEAR || m_Status == GS_ALLCLEAR)
    {
        if (m_Status == GS_CLEAR)
            PutText(backSurface.getDC(), m_rcArea.right + 100, 180, 40, RGB(255, 255, 255),
                    win::loadString(instance, IDS_FINISH).value());
        else
            PutText(backSurface.getDC(), m_rcArea.right + 80, 180, 40, RGB(255, 255, 255),
                    win::loadString(instance, IDS_ALL_CLEAR).value());

        PutText(backSurface.getDC(), m_rcArea.right, 260, 20, RGB(255, 255, 255),
                win::loadString(instance, IDS_RESTART).value());
        PutText(backSurface.getDC(), m_rcArea.right, 280, 20, RGB(255, 255, 255),
                win::loadString(instance, IDS_BACK_TO_TITLE).value());
        PutText(backSurface.getDC(), m_rcArea.right, 300, 20, RGB(255, 255, 255),
                win::loadString(instance, IDS_END).value());

        PutText(backSurface.getDC(), m_rcArea.right, 340, 20, RGB(255, 255, 255),
                win::loadString(instance, IDS_SAVE_REPLAY_0).value());
        PutText(backSurface.getDC(), m_rcArea.right + 40, 360, 20, RGB(255, 255, 255),
                win::loadString(instance, IDS_SAVE_REPLAY_1).value());

        if (m_Score >= 10000 && m_Level - 1 == m_cMaskNum)
        {
            switch (m_Level)
            {
                case 1:
                    PutText(backSurface.getDC(), m_rcArea.right + 40, 400, 20, RGB(0, 255, 255),
                            win::loadString(instance, IDS_CLEAR_BONUS_0).value());
                    break;

                case 2:
                    PutText(backSurface.getDC(), m_rcArea.right + 20, 400, 20, RGB(0, 255, 255),
                            win::loadString(instance, IDS_CLEAR_BONUS_1).value());
                    break;

                case 3:
                    PutText(backSurface.getDC(), m_rcArea.right + 20, 400, 20, RGB(0, 255, 255),
                            win::loadString(instance, IDS_CLEAR_BONUS_2).value());
                    break;

                case 4:
                    PutText(backSurface.getDC(), m_rcArea.right + 20, 400, 20, RGB(0, 255, 255),
                            win::loadString(instance, IDS_CLEAR_BONUS_3).value());
                    break;
            }
        }
    }
}

// マス目チェック
void CSAME::onMouseMove(::POINT const& pt)
{
    static unsigned short s_sBef = m_Width * m_Height;

    m_bReGame = false;
    if (m_Status == GS_CLEAR || m_Status == GS_ALLCLEAR)
    {
        RECT rc;
        SetRect(&rc, m_rcArea.right, 260, m_rcArea.right * 2, 320);
        if (PtInRect(&rc, pt))
        {
            m_bReGame = true;
        }
        return;
    }

    Inexplore(m_by * m_Width + m_bx);
    m_Num = 0;
    m_bx  = m_Width;
    m_by  = m_Height;

    if (PtInRect(&m_rcArea, pt))
    {
        unsigned short const x = static_cast<unsigned short>(pt.x) / PIX;
        unsigned short const y = static_cast<unsigned short>(pt.y) / PIY;
        m_bx = x;
        m_by = y;

        unsigned short const pos = y * m_Width + x;
        if (pos >= m_Width * m_Height) return;
        if (m_Area[pos] == 0)
        {
            s_sBef = pos;
            return;
        }
        Explore(pos, m_Area[pos]);

        if (m_Num == 1) Unselect();

        if (selectsAt(x, y))
        {
            if (!(m_Area[s_sBef] & 0x80))
            {
                PlaySound(SELWAV, nullptr, SND_FILENAME | SND_ASYNC);
            }
        }
        s_sBef = pos;
    }
    else
    {
        s_sBef = m_Width * m_Height;
    }
}

// マス目チェック・再帰
void CSAME::Explore(unsigned short pos, unsigned char piece)
{
    if (pos >= m_Width * m_Height) return;
    if (piece == 0) return;
    if (m_Area[pos] & static_cast<unsigned char>(0x80)) return;

    if (m_Area[pos] == piece)
    {
        m_Area[pos] |= static_cast<unsigned char>(0x80);
        ++m_Num;

        if (pos >= m_Width) Explore(pos - m_Width, piece);
        if ((pos % m_Width) != 0) Explore(pos - 1, piece);
        if ((pos % m_Width) < m_Width - 1) Explore(pos + 1, piece);
        if ((pos / m_Width) < m_Height - 1) Explore(pos + m_Width, piece);
    }
}

// マス目選択消去
void CSAME::Unselect()
{
    Inexplore(m_by * m_Width + m_bx);
}

// マス目チェック・反転再帰
void CSAME::Inexplore(unsigned short pos)
{
    if (pos >= m_Width * m_Height) return;
    if (m_Area[pos] & static_cast<unsigned char>(0x80))
    {
        m_Area[pos] ^= static_cast<unsigned char>(0x80);

        if (pos >= m_Width) Inexplore(pos - m_Width);
        if ((pos % m_Width) != 0) Inexplore(pos - 1);
        if ((pos % m_Width) < m_Width - 1) Inexplore(pos + 1);
        if ((pos / m_Width) < m_Height - 1) Inexplore(pos + m_Width);
    }
}

// 駒消去・再帰
void CSAME::Exexplore(unsigned short pos)
{
    if (pos >= m_Width * m_Height) return;

    if (m_Area[pos] & static_cast<unsigned char>(0x80))
    {
        m_Area[pos] = 0;

        if (pos >= m_Width) Exexplore(pos - m_Width);
        if ((pos % m_Width) != 0) Exexplore(pos - 1);
        if ((pos % m_Width) < m_Width - 1) Exexplore(pos + 1);
        if ((pos / m_Width) < m_Height - 1) Exexplore(pos + m_Width);
    }
}

// 駒落ちチェック
void CSAME::Check()
{
    auto const max = m_Width * m_Height;

    for (auto i = 0; i < max; ++i)
    {
        if (m_Area[i] == 0) VShift(i);
    }

    for (auto i = 0; i < m_Width - 1; ++i)
    {
        if (m_Area[(m_Height - 1) * m_Width + i] != 0) continue;

        unsigned short j;

        for (j = i; j < m_Width; ++j)
        {
            if (m_Area[(m_Height - 1) * m_Width + j] != 0)
                break;
        }

        if (j == m_Width) break;

        for (auto k = 0; k < m_Height; ++k)
            HShift(k * m_Width + i);

        --i;
    }
}

// 縦方向シフト・再帰
void CSAME::VShift(unsigned short pos)
{
    if (pos >= m_Width * m_Height) return;

    if (pos >= m_Width)
    {
        m_Area[pos] = m_Area[pos - m_Width];
        VShift(pos - m_Width);
    }
    else
        m_Area[pos] = 0;
}

// 横方向シフト・再帰
void CSAME::HShift(unsigned short pos)
{
    if (pos >= m_Width * m_Height) return;

    if (pos % m_Width < m_Width - 1)
    {
        m_Area[pos] = m_Area[pos + 1];
        HShift(pos + 1);
    }
    else
        m_Area[pos] = 0;
}

// 終了チェック
unsigned char CSAME::EndCheck()
{
    auto const bAll = CntGroups();

    if (bAll) return CR_ALLCLEAR;
    else if (m_Groups == 0)
        return CR_CLEAR;

    return CR_NOSTATUS;
}

// 塊の個数計算
bool CSAME::CntGroups()
{
    unsigned char cPiece = 0;

    auto const max = m_Width * m_Height;
    m_Groups = 0;

    for (auto i = 0; i < max; ++i)
    {
        cPiece |= (m_Area[i] != 0);
        if (m_Area[i] == 0) continue;
        if (m_Area[i] & static_cast<unsigned char>(0x80)) continue;

        m_Num = 0;
        Explore(i, m_Area[i]);
        if (m_Num > 1)
        {
            ++m_Groups;
        }
    }

    // 後始末
    for (auto i = 0; i < max; ++i)
    {
        if (m_Area[i] == 0) continue;
        if (!(m_Area[i] & static_cast<unsigned char>(0x80))) continue;

        Inexplore(i);
    }

    return cPiece == 0;
}

void CSAME::AddScore(unsigned long add)
{
    m_Score += add;
    if (m_Score > m_HighScore) m_HighScore = m_Score;
}

CSAME::~CSAME()
{
    SaveStatus();
}

void CSAME::LoadStatus()
{
    auto const hFile = CreateFile(DATFILE, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) return;

    SetFilePointer(hFile, 0, nullptr, FILE_BEGIN);
    DWORD dwRead;
    ReadFile(hFile, &m_Level, 1, &dwRead, nullptr);

    DWORD const dwSize = sizeof(unsigned long);
    ReadFile(hFile, &m_HighScore, dwSize, &dwRead, nullptr);
    auto const data = reinterpret_cast<char*>(&m_HighScore);
    for (auto i = 0; i < static_cast<int>(sizeof(unsigned long)); ++i)
        data[i] -= CODE(i + 1);

    CloseHandle(hFile);
}

void CSAME::SaveStatus()
{
    auto const data = reinterpret_cast<char*>(&m_HighScore);
    for (auto i = 0; i < static_cast<int>(sizeof(unsigned long)); ++i)
        data[i] += CODE(i + 1);

    auto const hFile = CreateFile(DATFILE, GENERIC_WRITE, 0, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) return;

    SetFilePointer(hFile, 0, nullptr, FILE_BEGIN);
    DWORD dwWritten;
    WriteFile(hFile, &m_Level, 1, &dwWritten, nullptr);

    DWORD const dwSize = sizeof(unsigned long);
    WriteFile(hFile, &m_HighScore, dwSize, &dwWritten, nullptr);

    CloseHandle(hFile);
}

void CSAME::SaveReplay(char cNum)
{
    TCHAR strFName[0x100];

    lstrcpy(strFName, REPFILE);
    strFName[lstrlen(strFName) + 1] = '\0';
    strFName[lstrlen(strFName)]     = cNum;

    auto const hFile = CreateFile(strFName, GENERIC_WRITE, 0, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) return;

    SetFilePointer(hFile, 0, nullptr, FILE_BEGIN);
    DWORD dwWritten;
    WriteFile(hFile, &m_GameNum, sizeof(unsigned long), &dwWritten, nullptr);
    auto const tries = static_cast<unsigned short>(m_Played.size());
    WriteFile(hFile, &tries, sizeof(tries), &dwWritten, nullptr);

    for (auto i = 0; i < tries; ++i)
    {
        WriteFile(hFile, &m_Played[i], 1, &dwWritten, nullptr);
    }

    CloseHandle(hFile);
}

std::vector<unsigned char> CSAME::makeArea(unsigned short const width,
                                           unsigned short const height,
                                           unsigned long const gameNumber) const
{
    std::vector<unsigned char> area(width* height);

    std::mt19937 engine { gameNumber };
    for (auto y = 0; y < height; ++y)
    {
        for (auto x = 0; x < width; ++x)
        {
            // for backward compatibility, not using uniform_int_distribution
            area[x + y * m_Width] = static_cast<unsigned char>(engine() % 5 + 1);
        }
    }

    return area;
}

unsigned char CSAME::getAt(unsigned short const x, unsigned short const y) const
{
    return m_Area[x + y * m_Width];
}

bool CSAME::selectsAt(unsigned short const x, unsigned short const y) const
{
    return (m_Area[x + y * m_Width] & 0x80) != 0;
}

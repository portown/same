// replay.cpp

#include <random>

#include "defines.hxx"
#include "structs.hxx"
#include "funcs.hxx"
#include "tstringstream.hxx"
#include "win_utils.hxx"

#include "resource.h"


CREPLAY::CREPLAY(unsigned short wx, unsigned short wy, char cNum)
{
    unsigned short i, j;

    m_Status = GS_PLAYING;

    m_Width  = wx / PIX;
    m_Height = wy / PIY;
    m_bx     = m_Width;
    m_by     = m_Height;

    SetRect(&m_rcArea, 0, 0, wx, wy);

    m_Num    = 0;
    m_Tries  = 0;
    m_Pieces = m_Width * m_Height;
    m_Groups = 0;
    m_Score  = 0;

    m_Played.clear();
    m_GameNum = 0;
    if (cNum == -1) cNum = '\0';
    else cNum += '0';
    if (!LoadReplay(cNum))
    {
        m_Status = GS_NOREPLAY;
        return;
    }
    m_Tries  = 0;
    m_bErase = false;

    m_Area = std::vector<unsigned char>(m_Width * m_Height);
    std::mt19937 engine { m_GameNum };
    for (i = 0; i < m_Height; ++i)
    {
        for (j = 0; j < m_Width; ++j)
        {
            // for backward compatibility, not using uniform_int_distribution
            m_Area[i * m_Width + j] = static_cast<unsigned char>(engine() % 5 + 1);
        }
    }

    CntGroups();

    surface_ = same::ui::Surface::fromBitmapFile(DATA(TEXT("system.bmp")));

    cursorSurface_ = same::ui::Surface::fromBitmapResource(
        reinterpret_cast<HINSTANCE>(::GetModuleHandle(nullptr)),
        IDB_SAMECUR);

    previousTime_ = Clock::now();
}

void CREPLAY::initializeGraphics()
{
}

void CREPLAY::releaseGraphics()
{
}

void CREPLAY::onFrame(same::GameContext& context, same::Input const& input)
{
    using namespace std::literals::chrono_literals;
    auto const now = Clock::now();
    if (now - previousTime_ >= 1s)
    {
        Replay();
        previousTime_ = now;
    }

    if (input.isMouseLButtonUp() && m_Status == GS_NOREPLAY)
    {
        context.changeState<CMENU>(WINX, WINY);
        return;
    }
    else
    {
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

void CREPLAY::draw(same::ui::Surface& backSurface)
{
    unsigned short i, j;
    unsigned char  tmp;
    auto const     instance = ::GetModuleHandle(nullptr);

    if (m_Status == GS_NOREPLAY)
    {
        PutText(backSurface.getDC(), 320 - 13 * 20, 220, 40, RGB(255, 255, 255),
                win::loadString(instance, IDS_NO_REPLAY).value());
        PutText(backSurface.getDC(), 320, 272, 20, RGB(255, 255, 255),
                win::loadString(instance, IDS_CLICK_TO_TITLE).value());
        return;
    }

    // ゲーム盤の描画
    for (i = 0; i < m_Height; ++i)
    {
        for (j = 0; j < m_Width; ++j)
        {
            tmp = m_Area[i * m_Width + j];

            if (tmp != 0)
            {
                --tmp;

                if (tmp & static_cast<unsigned char>(0x80))
                {
                    tmp = tmp ^ static_cast<unsigned char>(0x80);

                    BitBlt(backSurface.getDC(), j * PIX, i * PIY, PIX, PIY, surface_->getDC(), tmp * PIX, PIY * 2,
                           SRCAND);
                    BitBlt(backSurface.getDC(), j * PIX, i * PIY, PIX, PIY, surface_->getDC(), tmp * PIX, PIY,
                           SRCPAINT);
                }
                else
                {
                    BitBlt(backSurface.getDC(), j * PIX, i * PIY, PIX, PIY, surface_->getDC(), tmp * 32, PIY * 2,
                           SRCAND);
                    BitBlt(backSurface.getDC(), j * PIX, i * PIY, PIX, PIY, surface_->getDC(), tmp * 32, 0, SRCPAINT);
                }
            }
        }
    }

    // カーソルの描画
    if (m_bx < m_Width && m_by < m_Height)
    {
        BitBlt(backSurface.getDC(), m_bx * 32 + PIX / 2, m_by * 32 + PIY / 2, 32, 32, cursorSurface_->getDC(), 32, 0,
               SRCAND);
        BitBlt(backSurface.getDC(), m_bx * 32 + PIX / 2, m_by * 32 + PIY / 2, 32, 32, cursorSurface_->getDC(), 0, 0,
               SRCPAINT);
    }

    // その他の描画
    tch::tostringstream oss;
    oss.str({});
    oss << win::loadString(instance, IDS_SCORE).value() << m_Score;
    PutText(backSurface.getDC(), m_rcArea.right, 0, 20, RGB(255, 255, 255), oss.str());
    PutText(backSurface.getDC(), m_rcArea.right + 80, 24, 24, RGB(255, 255, 255),
            win::loadString(instance, IDS_REPLAYING).value());

    oss.str({});
    oss << win::loadString(instance, IDS_SELECTED_AREA).value() << m_Num;
    PutText(backSurface.getDC(), m_rcArea.right, 60, 20, RGB(255, 255, 255), oss.str());
    oss.str({});
    oss << win::loadString(instance, IDS_TRIES).value() << m_Tries;
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

        PutText(backSurface.getDC(), m_rcArea.right, 280, 20, RGB(255, 255, 255),
                win::loadString(instance, IDS_BACK_TO_TITLE).value());
        PutText(backSurface.getDC(), m_rcArea.right, 300, 20, RGB(255, 255, 255),
                win::loadString(instance, IDS_END).value());

        PutText(backSurface.getDC(), m_rcArea.right, 340, 20, RGB(255, 255, 255),
                win::loadString(instance, IDS_SAVE_REPLAY_0).value());
        PutText(backSurface.getDC(), m_rcArea.right + 40, 360, 20, RGB(255, 255, 255),
                win::loadString(instance, IDS_SAVE_REPLAY_1).value());
    }
}

// マス目チェック
void CREPLAY::Onselect(unsigned short pos)
{
    static unsigned short s_sBef = 0;
    unsigned short        x, y;

    if (m_Status != GS_PLAYING) return;

    Inexplore(m_by * m_Width + m_bx);
    m_Num = 0;
    m_bx  = 0;
    m_by  = 0;

    x = pos % m_Width;
    y = pos / m_Width;

    if (pos >= m_Width * m_Height) return;
    if (m_Area[pos] == 0) return;
    Explore(pos, m_Area[pos]);

    m_bx = x;
    m_by = y;

    if (m_Num == 1) Unselect();

    if (m_Area[pos] & 0x80)
    {
        if (!(m_Area[s_sBef] & 0x80))
        {
            PlaySound(SELWAV, nullptr, SND_FILENAME | SND_ASYNC);
        }
    }
    s_sBef = pos;
}

// マス目チェック・再帰
void CREPLAY::Explore(unsigned short pos, unsigned char piece)
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
void CREPLAY::Unselect()
{
    Inexplore(m_by * m_Width + m_bx);
}

// マス目チェック・反転再帰
void CREPLAY::Inexplore(unsigned short pos)
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

// 駒消去
unsigned char CREPLAY::Onclick()
{
    unsigned char cRet;

    if (m_Status != GS_PLAYING) return CR_NOSTATUS;

    if (m_Num < 2) return CR_NOSTATUS;
    if (m_bx >= m_Width || m_by >= m_Height) return CR_NOSTATUS;
    if (m_Area[m_by * m_Width + m_bx] == 0) return CR_NOSTATUS;

    Exexplore(m_by * m_Width + m_bx);

    ++m_Tries;
    m_Pieces -= m_Num;
    AddScore(ADDSCORE(m_Num));

    Check();

    cRet = EndCheck();
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

    return cRet;
}

// 駒消去・再帰
void CREPLAY::Exexplore(unsigned short pos)
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
void CREPLAY::Check()
{
    unsigned short i, j, max;

    max = m_Width * m_Height;

    for (i = 0; i < max; ++i)
    {
        if (m_Area[i] == 0) VShift(i);
    }

    for (i = 0; i < m_Width - 1; ++i)
    {
        if (m_Area[(m_Height - 1) * m_Width + i] == 0)
        {
            for (j = i; j < m_Width; ++j)
            {
                if (m_Area[(m_Height - 1) * m_Width + j] != 0)
                    break;
            }

            if (j == m_Width) break; ;

            for (j = 0; j < m_Height; ++j)
                HShift(j * m_Width + i);

            --i;
        }
    }
}

// 縦方向シフト・再帰
void CREPLAY::VShift(unsigned short pos)
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
void CREPLAY::HShift(unsigned short pos)
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
unsigned char CREPLAY::EndCheck()
{
    bool bAll;

    bAll = CntGroups();

    if (bAll) return CR_ALLCLEAR;
    else if (m_Groups == 0)
        return CR_CLEAR;

    return CR_NOSTATUS;
}

// 塊の個数計算
bool CREPLAY::CntGroups()
{
    unsigned short max, i;
    unsigned char  cPiece = 0;

    max      = m_Width * m_Height;
    m_Groups = 0;

    for (i = 0; i < max; ++i)
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
    for (i = 0; i < max; ++i)
    {
        if (m_Area[i] == 0) continue;
        if (!(m_Area[i] & static_cast<unsigned char>(0x80))) continue;

        Inexplore(i);
    }

    return cPiece == 0;
}

void CREPLAY::AddScore(unsigned long add)
{
    m_Score += add;
}

// リプレイ再生中
void CREPLAY::Replay()
{
    unsigned short i = static_cast<unsigned short>(m_Played.size());
    if (m_Tries > i) return;

    if (m_bErase)
    {
        Onclick();
        m_bErase = false;
    }
    else
    {
        Onselect(m_Played[m_Tries]);
        m_bErase = true;
    }
}

bool CREPLAY::LoadReplay(char cNum)
{
    HANDLE         hFile;
    DWORD          dwRead;
    unsigned short i;
    unsigned char  ucDat;
    TCHAR          strFName[0x100];

    lstrcpy(strFName, REPFILE);
    strFName[lstrlen(strFName) + 1] = '\0';
    strFName[lstrlen(strFName)]     = cNum;

    hFile = CreateFile(strFName, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) return false;

    SetFilePointer(hFile, 0, nullptr, FILE_BEGIN);
    ReadFile(hFile, &m_GameNum, sizeof(unsigned long), &dwRead, nullptr);
    if (dwRead != sizeof(unsigned long)) return false;

    ReadFile(hFile, &m_Tries, sizeof(unsigned short), &dwRead, nullptr);
    if (dwRead != sizeof(unsigned short)) return false;

    for (i = 0; i < m_Tries; ++i)
    {
        ReadFile(hFile, &ucDat, 1, &dwRead, nullptr);
        if (dwRead != 1) return false;
        m_Played.push_back(ucDat);
    }

    CloseHandle(hFile);

    m_cRepNum = cNum;

    return true;
}

void CREPLAY::SaveReplay(char cNum)
{
    TCHAR strSrcName[0x100], strDstName[0x100];

    lstrcpy(strSrcName, REPFILE);
    strSrcName[lstrlen(strSrcName) + 1] = '\0';
    strSrcName[lstrlen(strSrcName)]     = m_cRepNum;

    lstrcpy(strDstName, REPFILE);
    strDstName[lstrlen(strDstName) + 1] = '\0';
    strDstName[lstrlen(strDstName)]     = cNum;

    CopyFile(strSrcName, strDstName, TRUE);
}

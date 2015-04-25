// game.cpp

#include "common.hxx"

#include <ctime>
#include <random>


// ==============================================
// 実装
// ==============================================

// 初期化
CSAME::CSAME(unsigned short wx, unsigned short wy, char cMaskNum)
    : CSAME(wx, wy, cMaskNum, static_cast<unsigned long>(std::time(nullptr)))
{
}

CSAME::CSAME(unsigned short wx, unsigned short wy, char cMaskNum, unsigned long gameNum)
{
    m_hDC = CreateCompatibleDC(NULL);
    m_hBm = Load_Bmp(DATA(system.bmp));
    SelectObject(m_hDC, m_hBm);

    m_Level     = 0;
    m_HighScore = 0;

    LoadStatus();

    m_Status = GS_PLAYING;

    m_cMaskNum = cMaskNum;

    m_Width   = wx / PIX;
    m_Height  = wy / PIY;
    m_bx      = m_Width;
    m_by      = m_Height;
    m_bDraw   = false;
    m_bReGame = false;

    SetRect(&m_rcArea, 0, 0, wx, wy);

    m_Num    = 0;
    m_Tries  = 0;
    m_Pieces = m_Width * m_Height;
    m_Groups = 0;
    m_Score  = 0;

    m_GameNum = gameNum;
    m_Area    = makeArea(m_Width, m_Height, m_GameNum);

    CntGroups();

    m_Played.clear();
}

// マス目の描画
void CSAME::Draw(HDC hDC)
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

                        tmp = tmp ^ ( unsigned char )0x80;

                        BitBlt(hDC, j * PIX, i * PIY, PIX, PIY, m_hDC, tmp * PIX, PIY * 2, SRCAND);
                        BitBlt(hDC, j * PIX, i * PIY, PIX, PIY, m_hDC, tmp * PIX, PIY, SRCPAINT);
                    }
                    else
                    {
                        if (m_cMaskNum == 2) continue;

                        BitBlt(hDC, j * PIX, i * PIY, PIX, PIY, m_hDC, tmp * 32, PIY * 2, SRCAND);
                        BitBlt(hDC, j * PIX, i * PIY, PIX, PIY, m_hDC, tmp * 32, 0, SRCPAINT);
                    }
                }
            }
        }
    }

    char strTmp[0x100];

    // その他の描画
    wsprintf(strTmp, "　　スコア：%lu", m_Score);
    PutText(hDC, m_rcArea.right, 0, 20, RGB(255, 255, 255), strTmp);
    wsprintf(strTmp, "ハイスコア：%lu", m_HighScore);
    PutText(hDC, m_rcArea.right, 20, 20, RGB(255, 255, 255), strTmp);

    wsprintf(strTmp, "選択エリア：%u", m_Num);
    PutText(hDC, m_rcArea.right, 60, 20, RGB(255, 255, 255), strTmp);
    wsprintf(strTmp, "　　　手数：%u", m_Tries);
    PutText(hDC, m_rcArea.right, 80, 20, RGB(255, 255, 255), strTmp);
    wsprintf(strTmp, "　残り個数：%u", m_Pieces);
    PutText(hDC, m_rcArea.right, 100, 20, RGB(255, 255, 255), strTmp);
    wsprintf(strTmp, "　残り塊数：%u", m_Groups);
    PutText(hDC, m_rcArea.right, 120, 20, RGB(255, 255, 255), strTmp);

    wsprintf(strTmp, "　　 X座標：%u", (m_bx < m_Width ? m_bx : 0));
    PutText(hDC, m_rcArea.right + 152, 60, 20, RGB(255, 255, 255), strTmp);
    wsprintf(strTmp, "　　 Y座標：%u", (m_by < m_Height ? m_by : 0));
    PutText(hDC, m_rcArea.right + 152, 80, 20, RGB(255, 255, 255), strTmp);

    wsprintf(strTmp, "Game Number:%lu", m_GameNum);
    PutText(hDC, m_rcArea.right + 160, m_rcArea.bottom - 12, 12, RGB(255, 255, 255), strTmp);

    // クリア後の描画
    if (m_Status == GS_CLEAR || m_Status == GS_ALLCLEAR)
    {
        if (m_Status == GS_CLEAR)
            PutText(hDC, m_rcArea.right + 100, 180, 40, RGB(255, 255, 255), "終了！");
        else
            PutText(hDC, m_rcArea.right + 80, 180, 40, RGB(255, 255, 255), "全消し！");

        PutText(hDC, m_rcArea.right, 260, 20, RGB(255, 255, 255), "Enter:再ゲーム");
        PutText(hDC, m_rcArea.right, 280, 20, RGB(255, 255, 255), "F12  :メニューに戻る");
        PutText(hDC, m_rcArea.right, 300, 20, RGB(255, 255, 255), "Esc  :終了");

        PutText(hDC, m_rcArea.right, 340, 20, RGB(255, 255, 255), "キーボードの0～9を押すと");
        PutText(hDC, m_rcArea.right + 40, 360, 20, RGB(255, 255, 255), "リプレイを保存します");

        if (m_Score >= 10000 && m_Level - 1 == m_cMaskNum)
        {
            switch (m_Level)
            {
                case 1:
                    PutText(hDC, m_rcArea.right + 40, 400, 20, RGB(0, 255, 255), "“マスクモード”出現！");
                    break;

                case 2:
                    PutText(hDC, m_rcArea.right + 20, 400, 20, RGB(0, 255, 255), "マスクモードレベル２出現！");
                    break;

                case 3:
                    PutText(hDC, m_rcArea.right + 20, 400, 20, RGB(0, 255, 255), "マスクモードレベル３出現！");
                    break;

                case 4:
                    PutText(hDC, m_rcArea.right + 20, 400, 20, RGB(0, 255, 255), "マスクモードレベル４出現！");
                    break;
            }
        }
    }

    m_bDraw = true;
}

// マス目チェック
void CSAME::Select(POINT pt)
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
        unsigned short const x = ( unsigned short )pt.x / PIX;
        unsigned short const y = ( unsigned short )pt.y / PIY;
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
                PlaySound(SELWAV, NULL, SND_FILENAME | SND_ASYNC);
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
    if (m_Area[pos] & ( unsigned char )0x80) return;

    if (m_Area[pos] == piece)
    {
        m_Area[pos] |= ( unsigned char )0x80;
        ++m_Num;

        if (pos >= m_Width) Explore(pos - m_Width, piece);
        if ((pos % m_Width) != 0) Explore(pos - 1, piece);
        if ((pos % m_Width) < m_Width - 1) Explore(pos + 1, piece);
        if ((pos / m_Width) < m_Height - 1) Explore(pos + m_Width, piece);
    }
}

// マス目選択消去
void CSAME::Unselect(void)
{
    Inexplore(m_by * m_Width + m_bx);
}

// マス目チェック・反転再帰
void CSAME::Inexplore(unsigned short pos)
{
    if (pos >= m_Width * m_Height) return;
    if (m_Area[pos] & ( unsigned char )0x80)
    {
        m_Area[pos] ^= ( unsigned char )0x80;

        if (pos >= m_Width) Inexplore(pos - m_Width);
        if ((pos % m_Width) != 0) Inexplore(pos - 1);
        if ((pos % m_Width) < m_Width - 1) Inexplore(pos + 1);
        if ((pos / m_Width) < m_Height - 1) Inexplore(pos + m_Width);
    }
}

// 駒消去
unsigned char CSAME::Click(void)
{
    if (!m_bDraw) return CR_NOSTATUS;

    if (m_Status == GS_CLEAR || m_Status == GS_ALLCLEAR)
    {
        if (m_bReGame) return CR_BEGINNORMAL + m_cMaskNum;
        return CR_NOSTATUS;
    }

    if (m_Num < 2) return CR_NOSTATUS;
    if (m_bx >= m_Width || m_by >= m_Height) return CR_NOSTATUS;
    if (m_Area[m_by * m_Width + m_bx] == 0) return CR_NOSTATUS;

    Exexplore(m_by * m_Width + m_bx);

    ++m_Tries;
    m_Pieces -= m_Num;
    AddScore(ADDSCORE(m_Num));
    m_Played.push_back(( unsigned char )(m_by * m_Width + m_bx));

    Check();

    auto const cRet = EndCheck();
    switch (cRet)
    {
        case CR_NOSTATUS:
            PlaySound(ERSWAV, NULL, SND_FILENAME | SND_SYNC);
            m_Num = 0;
            Explore(m_by * m_Width + m_bx, m_Area[m_by * m_Width + m_bx]);
            if (m_Num == 1) Unselect();
            break;

        case CR_CLEAR:
            PlaySound(CLRWAV, NULL, SND_FILENAME | SND_ASYNC);
            m_Status = GS_CLEAR;
            break;

        case CR_ALLCLEAR:
            AddScore(1000000);
            PlaySound(CLRWAV, NULL, SND_FILENAME | SND_ASYNC);
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

    m_bDraw = false;

    return cRet;
}

// 駒消去・再帰
void CSAME::Exexplore(unsigned short pos)
{
    if (pos >= m_Width * m_Height) return;

    if (m_Area[pos] & ( unsigned char )0x80)
    {
        m_Area[pos] = 0;

        if (pos >= m_Width) Exexplore(pos - m_Width);
        if ((pos % m_Width) != 0) Exexplore(pos - 1);
        if ((pos % m_Width) < m_Width - 1) Exexplore(pos + 1);
        if ((pos / m_Width) < m_Height - 1) Exexplore(pos + m_Width);
    }
}

// 駒落ちチェック
void CSAME::Check(void)
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
unsigned char CSAME::EndCheck(void)
{
    auto const bAll = CntGroups();

    if (bAll) return CR_ALLCLEAR;
    else if (m_Groups == 0)
        return CR_CLEAR;

    return CR_NOSTATUS;
}

// 塊の個数計算
bool CSAME::CntGroups(void)
{
    unsigned char cPiece = 0;

    auto const max = m_Width * m_Height;
    m_Groups = 0;

    for (auto i = 0; i < max; ++i)
    {
        cPiece |= (m_Area[i] != 0);
        if (m_Area[i] == 0) continue;
        if (m_Area[i] & ( unsigned char )0x80) continue;

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
        if (!(m_Area[i] & ( unsigned char )0x80)) continue;

        Inexplore(i);
    }

    return cPiece == 0;
}

// スコア加算
void CSAME::AddScore(unsigned long add)
{
    m_Score += add;
    if (m_Score > m_HighScore) m_HighScore = m_Score;
}

// キーダウン
unsigned char CSAME::KeyDown(WPARAM key)
{
    switch (key)
    {
        case VK_RETURN:
            if (m_Status == GS_CLEAR || m_Status == GS_ALLCLEAR)
                return CR_BEGINNORMAL + m_cMaskNum;
            break;

        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            SaveReplay(( char )key);
            break;

        case VK_F8:
            return CR_BEGINNORMAL + m_cMaskNum;

        case VK_F12:
            return CR_TITLEMENU;

        case VK_ESCAPE:
            return CR_ENDGAME;
    }

    return CR_NOSTATUS;
}

// デストラクタ
CSAME::~CSAME(void)
{
    SaveStatus();

    RelsSurface(m_hDC, m_hBm);
    delete [] m_Area;
}

// ステータス読み込み
void CSAME::LoadStatus(void)
{
    auto const hFile = CreateFile(DATFILE, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return;

    SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
    DWORD dwRead;
    ReadFile(hFile, &m_Level, 1, &dwRead, NULL);

    DWORD const dwSize = sizeof(unsigned long);
    ReadFile(hFile, &m_HighScore, dwSize, &dwRead, NULL);
    auto const data = ( char* )&m_HighScore;
    for (auto i = 0; i < static_cast<int>(sizeof(unsigned long)); ++i)
        data[i] -= CODE(i + 1);

    CloseHandle(hFile);
}

// ステータス書き込み
void CSAME::SaveStatus(void)
{
    auto const data = ( char* )&m_HighScore;
    for (auto i = 0; i < static_cast<int>(sizeof(unsigned long)); ++i)
        data[i] += CODE(i + 1);

    auto const hFile = CreateFile(DATFILE, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return;

    SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
    DWORD dwWritten;
    WriteFile(hFile, &m_Level, 1, &dwWritten, NULL);

    DWORD const dwSize = sizeof(unsigned long);
    WriteFile(hFile, &m_HighScore, dwSize, &dwWritten, NULL);

    CloseHandle(hFile);
}

// リプレイデータ書き込み
void CSAME::SaveReplay(char cNum)
{
    char strFName[0x100];

    lstrcpy(strFName, REPFILE);
    strFName[lstrlen(strFName) + 1] = '\0';
    strFName[lstrlen(strFName)]     = cNum;

    auto const hFile = CreateFile(strFName, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return;

    SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
    DWORD dwWritten;
    WriteFile(hFile, &m_GameNum, sizeof(unsigned long), &dwWritten, NULL);
    WriteFile(hFile, &m_Tries, sizeof(unsigned short), &dwWritten, NULL);

    for (auto i = 0; i < m_Tries; ++i)
    {
        WriteFile(hFile, &m_Played[i], 1, &dwWritten, NULL);
    }

    CloseHandle(hFile);
}

unsigned char* CSAME::makeArea(unsigned short const width,
                               unsigned short const height,
                               unsigned long const gameNumber) const
{
    unsigned char* const area = new unsigned char [width * height];

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

// EOF

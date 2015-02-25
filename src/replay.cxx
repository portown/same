// replay.cpp

#include "common.hxx"


// ==============================================
// 実装
// ==============================================

// 初期化
CREPLAY::CREPLAY(HWND hWnd, unsigned short wx, unsigned short wy, char cNum)
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
    init_genrand(m_GameNum);
    m_Tries  = 0;
    m_bErase = false;

    m_Area = new unsigned char [m_Width * m_Height];
    for (i = 0; i < m_Height; ++i)
    {
        for (j = 0; j < m_Width; ++j)
        {
            m_Area[i * m_Width + j] = ( unsigned char )(genrand_int32() % 5 + 1);
        }
    }

    CntGroups();

    surface_.dcHandle_     = CreateCompatibleDC(NULL);
    surface_.bitmapHandle_ = Load_Bmp(DATA(system.bmp));
    SelectObject(surface_.dcHandle_, surface_.bitmapHandle_);

    cursorSurface_.dcHandle_     = CreateCompatibleDC(surface_.dcHandle_);
    cursorSurface_.bitmapHandle_ = ( HBITMAP )LoadImage(( HINSTANCE )GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                                                        MAKEINTRESOURCE(IDB_SAMECUR), IMAGE_BITMAP, 64, 32, LR_SHARED);
    SelectObject(cursorSurface_.dcHandle_, cursorSurface_.bitmapHandle_);

    m_hWnd = hWnd;
    SetTimer(m_hWnd, MINE_TIMER, 1000, NULL);
}

// マス目の描画
void CREPLAY::Draw(HDC hDC)
{
    unsigned short i, j;
    unsigned char  tmp;
    char           strTmp[0x100];

    if (m_Status == GS_NOREPLAY)
    {
        PutText(hDC, 320 - 13 * 20, 220, 40, RGB(255, 255, 255), "リプレイデータがありません", 26);
        PutText(hDC, 320, 272, 20, RGB(255, 255, 255), "クリックでタイトルに戻る", 24);
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

                if (tmp & ( unsigned char )0x80)
                {
                    tmp = tmp ^ ( unsigned char )0x80;

                    BitBlt(hDC, j * PIX, i * PIY, PIX, PIY, surface_.dcHandle_, tmp * PIX, PIY * 2, SRCAND);
                    BitBlt(hDC, j * PIX, i * PIY, PIX, PIY, surface_.dcHandle_, tmp * PIX, PIY, SRCPAINT);
                }
                else
                {
                    BitBlt(hDC, j * PIX, i * PIY, PIX, PIY, surface_.dcHandle_, tmp * 32, PIY * 2, SRCAND);
                    BitBlt(hDC, j * PIX, i * PIY, PIX, PIY, surface_.dcHandle_, tmp * 32, 0, SRCPAINT);
                }
            }
        }
    }

    // カーソルの描画
    if (m_bx < m_Width && m_by < m_Height)
    {
        BitBlt(hDC, m_bx * 32 + PIX / 2, m_by * 32 + PIY / 2, 32, 32, cursorSurface_.dcHandle_, 32, 0, SRCAND);
        BitBlt(hDC, m_bx * 32 + PIX / 2, m_by * 32 + PIY / 2, 32, 32, cursorSurface_.dcHandle_, 0, 0, SRCPAINT);
    }

    // その他の描画
    wsprintf(strTmp, "　　スコア：%lu", m_Score);
    PutText(hDC, m_rcArea.right, 0, 20, RGB(255, 255, 255), strTmp);
    PutText(hDC, m_rcArea.right + 80, 24, 24, RGB(255, 255, 255), "- リプレイ -");

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

        PutText(hDC, m_rcArea.right, 280, 20, RGB(255, 255, 255), "F12  :メニューに戻る");
        PutText(hDC, m_rcArea.right, 300, 20, RGB(255, 255, 255), "Esc  :終了");

        PutText(hDC, m_rcArea.right, 340, 20, RGB(255, 255, 255), "キーボードの0～9を押すと");
        PutText(hDC, m_rcArea.right + 40, 360, 20, RGB(255, 255, 255), "リプレイを保存します");
    }
}

// マウス移動
void CREPLAY::Select(POINT)
{
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
            PlaySound(SELWAV, NULL, SND_FILENAME | SND_ASYNC);
        }
    }
    s_sBef = pos;
}

// マス目チェック・再帰
void CREPLAY::Explore(unsigned short pos, unsigned char piece)
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
void CREPLAY::Unselect(void)
{
    Inexplore(m_by * m_Width + m_bx);
}

// マス目チェック・反転再帰
void CREPLAY::Inexplore(unsigned short pos)
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

// クリック
unsigned char CREPLAY::Click(void)
{
    if (m_Status == GS_NOREPLAY)
        return CR_TITLEMENU;

    return CR_NOSTATUS;
}

// 駒消去
unsigned char CREPLAY::Onclick(void)
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

    return cRet;
}

// 駒消去・再帰
void CREPLAY::Exexplore(unsigned short pos)
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
void CREPLAY::Check(void)
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
unsigned char CREPLAY::EndCheck(void)
{
    bool bAll;

    bAll = CntGroups();

    if (bAll) return CR_ALLCLEAR;
    else if (m_Groups == 0)
        return CR_CLEAR;

    return CR_NOSTATUS;
}

// 塊の個数計算
bool CREPLAY::CntGroups(void)
{
    unsigned short max, i;
    unsigned char  cPiece = 0;

    max      = m_Width * m_Height;
    m_Groups = 0;

    for (i = 0; i < max; ++i)
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
    for (i = 0; i < max; ++i)
    {
        if (m_Area[i] == 0) continue;
        if (!(m_Area[i] & ( unsigned char )0x80)) continue;

        Inexplore(i);
    }

    return cPiece == 0;
}

// スコア加算
void CREPLAY::AddScore(unsigned long add)
{
    m_Score += add;
}

// キーダウン
unsigned char CREPLAY::KeyDown(WPARAM key)
{
    switch (key)
    {
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

        case VK_F12:
            return CR_TITLEMENU;

        case VK_ESCAPE:
            return CR_ENDGAME;
    }

    return CR_NOSTATUS;
}

// リプレイ再生中
void CREPLAY::Replay(void)
{
    unsigned short i = ( unsigned short )m_Played.size();
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

    InvalidateRect(m_hWnd, NULL, FALSE);
}

// デストラクタ
CREPLAY::~CREPLAY(void)
{
    KillTimer(m_hWnd, MINE_TIMER);

    RelsSurface(cursorSurface_);
    RelsSurface(surface_);
    delete [] m_Area;
}

// リプレイデータ読み込み
bool CREPLAY::LoadReplay(char cNum)
{
    HANDLE         hFile;
    DWORD          dwRead;
    unsigned short i;
    unsigned char  ucDat;
    char           strFName[0x100];

    lstrcpy(strFName, REPFILE);
    strFName[lstrlen(strFName) + 1] = '\0';
    strFName[lstrlen(strFName)]     = cNum;

    hFile = CreateFile(strFName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return false;

    SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
    ReadFile(hFile, ( LPVOID )&m_GameNum, sizeof(unsigned long), &dwRead, NULL);
    if (dwRead != sizeof(unsigned long)) return false;

    ReadFile(hFile, ( LPVOID )&m_Tries, sizeof(unsigned short), &dwRead, NULL);
    if (dwRead != sizeof(unsigned short)) return false;

    for (i = 0; i < m_Tries; ++i)
    {
        ReadFile(hFile, ( LPVOID )&ucDat, 1, &dwRead, NULL);
        if (dwRead != 1) return false;
        m_Played.push_back(ucDat);
    }

    CloseHandle(hFile);

    m_cRepNum = cNum;

    return true;
}

// リプレイデータ書き込み
void CREPLAY::SaveReplay(char cNum)
{
    char strSrcName[0x100], strDstName[0x100];

    lstrcpy(strSrcName, REPFILE);
    strSrcName[lstrlen(strSrcName) + 1] = '\0';
    strSrcName[lstrlen(strSrcName)]     = m_cRepNum;

    lstrcpy(strDstName, REPFILE);
    strDstName[lstrlen(strDstName) + 1] = '\0';
    strDstName[lstrlen(strDstName)]     = cNum;

    CopyFile(strSrcName, strDstName, TRUE);
}

// EOF

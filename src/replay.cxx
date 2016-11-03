// replay.cpp

#include "common.hxx"

#include <random>


static void Onselect(GameSceneReplay* scene, unsigned short pos);
static void Explore(GameSceneReplay* scene, unsigned short pos, unsigned char piece);
static void Unselect(GameSceneReplay* scene);
static void Inexplore(GameSceneReplay* scene, unsigned short pos);
static unsigned char Onclick(GameSceneReplay* scene);
static void Exexplore(GameSceneReplay* scene, unsigned short pos);
static void Check(GameSceneReplay* scene);
static void VShift(GameSceneReplay* scene, unsigned short pos);
static void HShift(GameSceneReplay* scene, unsigned short pos);
static unsigned char EndCheck(GameSceneReplay* scene);
static bool CntGroups(GameSceneReplay* scene);
static void AddScore(GameSceneReplay* scene, unsigned long add);
static bool LoadReplay(GameSceneReplay* scene, char cNum);
static void SaveReplay(GameSceneReplay* scene, char cNum);


extern "C" {

GameSceneReplay* createGameSceneReplay(HWND const hWnd, unsigned int const width, unsigned int const height, int const replayNumber) {
    GameSceneReplay* const scene = (GameSceneReplay*)malloc(sizeof(GameSceneReplay));
    new(&scene->m_Played) std::vector<unsigned char>();

    unsigned short i, j;

    scene->m_Status = GS_PLAYING;

    scene->m_Width  = width / PIX;
    scene->m_Height = height / PIY;
    scene->m_bx     = scene->m_Width;
    scene->m_by     = scene->m_Height;

    SetRect(&scene->m_rcArea, 0, 0, width, height);

    scene->m_Num    = 0;
    scene->m_Tries  = 0;
    scene->m_Pieces = scene->m_Width * scene->m_Height;
    scene->m_Groups = 0;
    scene->m_Score  = 0;

    scene->m_Played.clear();
    scene->m_GameNum = 0;
    char const replayChar = replayNumber == -1 ? '\0' : (char)replayNumber + '0';
    if (!LoadReplay(scene, replayChar))
    {
        scene->m_Status = GS_NOREPLAY;
        return scene;
    }
    scene->m_Tries  = 0;
    scene->m_bErase = false;

    scene->m_Area = new unsigned char [scene->m_Width * scene->m_Height];
    std::mt19937 engine { scene->m_GameNum };
    for (i = 0; i < scene->m_Height; ++i)
    {
        for (j = 0; j < scene->m_Width; ++j)
        {
            // for backward compatibility, not using uniform_int_distribution
            scene->m_Area[i * scene->m_Width + j] = static_cast<unsigned char>(engine() % 5 + 1);
        }
    }

    CntGroups(scene);

    scene->surface = surfaceFromBitmapFile(DATA(system.bmp));
    scene->cursorSurface = surfaceFromResource((HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDB_SAMECUR));

    scene->m_hWnd = hWnd;
    SetTimer(scene->m_hWnd, MINE_TIMER, 1000, NULL);

    return scene;
}

void destroyGameSceneReplay(GameSceneReplay* const scene) {
    KillTimer(scene->m_hWnd, MINE_TIMER);

    destroySurface(scene->cursorSurface);
    destroySurface(scene->surface);
    delete [] scene->m_Area;
    scene->m_Played.~vector();

    free(scene);
}

void gameSceneReplayDraw(GameSceneReplay* const scene, Surface* const backSurface) {
    unsigned short i, j;
    unsigned char  tmp;
    char           strTmp[0x100];

    if (scene->m_Status == GS_NOREPLAY)
    {
        surfaceDrawText(backSurface, 320 - 13 * 20, 220, 40, RGB(255, 255, 255), "リプレイデータがありません");
        surfaceDrawText(backSurface, 320, 272, 20, RGB(255, 255, 255), "クリックでタイトルに戻る");
        return;
    }

    // ゲーム盤の描画
    for (i = 0; i < scene->m_Height; ++i)
    {
        for (j = 0; j < scene->m_Width; ++j)
        {
            tmp = scene->m_Area[i * scene->m_Width + j];

            if (tmp != 0)
            {
                --tmp;

                if (tmp & ( unsigned char )0x80)
                {
                    tmp = tmp ^ ( unsigned char )0x80;

                    surfaceBlit(backSurface, j * PIX, i * PIY, PIX, PIY, scene->surface, tmp * PIX, PIY * 2, SRCAND);
                    surfaceBlit(backSurface, j * PIX, i * PIY, PIX, PIY, scene->surface, tmp * PIX, PIY, SRCPAINT);
                }
                else
                {
                    surfaceBlit(backSurface, j * PIX, i * PIY, PIX, PIY, scene->surface, tmp * 32, PIY * 2, SRCAND);
                    surfaceBlit(backSurface, j * PIX, i * PIY, PIX, PIY, scene->surface, tmp * 32, 0, SRCPAINT);
                }
            }
        }
    }

    // カーソルの描画
    if (scene->m_bx < scene->m_Width && scene->m_by < scene->m_Height)
    {
        surfaceBlit(backSurface, scene->m_bx * 32 + PIX / 2, scene->m_by * 32 + PIY / 2, 32, 32, scene->cursorSurface, 32, 0, SRCAND);
        surfaceBlit(backSurface, scene->m_bx * 32 + PIX / 2, scene->m_by * 32 + PIY / 2, 32, 32, scene->cursorSurface, 0, 0, SRCPAINT);
    }

    // その他の描画
    wsprintf(strTmp, "　　スコア：%lu", scene->m_Score);
    surfaceDrawText(backSurface, scene->m_rcArea.right, 0, 20, RGB(255, 255, 255), strTmp);
    surfaceDrawText(backSurface, scene->m_rcArea.right + 80, 24, 24, RGB(255, 255, 255), "- リプレイ -");

    wsprintf(strTmp, "選択エリア：%u", scene->m_Num);
    surfaceDrawText(backSurface, scene->m_rcArea.right, 60, 20, RGB(255, 255, 255), strTmp);
    wsprintf(strTmp, "　　　手数：%u", scene->m_Tries);
    surfaceDrawText(backSurface, scene->m_rcArea.right, 80, 20, RGB(255, 255, 255), strTmp);
    wsprintf(strTmp, "　残り個数：%u", scene->m_Pieces);
    surfaceDrawText(backSurface, scene->m_rcArea.right, 100, 20, RGB(255, 255, 255), strTmp);
    wsprintf(strTmp, "　残り塊数：%u", scene->m_Groups);
    surfaceDrawText(backSurface, scene->m_rcArea.right, 120, 20, RGB(255, 255, 255), strTmp);

    wsprintf(strTmp, "　　 X座標：%u", (scene->m_bx < scene->m_Width ? scene->m_bx : 0));
    surfaceDrawText(backSurface, scene->m_rcArea.right + 152, 60, 20, RGB(255, 255, 255), strTmp);
    wsprintf(strTmp, "　　 Y座標：%u", (scene->m_by < scene->m_Height ? scene->m_by : 0));
    surfaceDrawText(backSurface, scene->m_rcArea.right + 152, 80, 20, RGB(255, 255, 255), strTmp);

    wsprintf(strTmp, "Game Number:%lu", scene->m_GameNum);
    surfaceDrawText(backSurface, scene->m_rcArea.right + 160, scene->m_rcArea.bottom - 12, 12, RGB(255, 255, 255), strTmp);

    // クリア後の描画
    if (scene->m_Status == GS_CLEAR || scene->m_Status == GS_ALLCLEAR)
    {
        if (scene->m_Status == GS_CLEAR)
            surfaceDrawText(backSurface, scene->m_rcArea.right + 100, 180, 40, RGB(255, 255, 255), "終了！");
        else
            surfaceDrawText(backSurface, scene->m_rcArea.right + 80, 180, 40, RGB(255, 255, 255), "全消し！");

        surfaceDrawText(backSurface, scene->m_rcArea.right, 280, 20, RGB(255, 255, 255), "F12  :メニューに戻る");
        surfaceDrawText(backSurface, scene->m_rcArea.right, 300, 20, RGB(255, 255, 255), "Esc  :終了");

        surfaceDrawText(backSurface, scene->m_rcArea.right, 340, 20, RGB(255, 255, 255), "キーボードの0～9を押すと");
        surfaceDrawText(backSurface, scene->m_rcArea.right + 40, 360, 20, RGB(255, 255, 255), "リプレイを保存します");
    }
}

void gameSceneReplayMouseMove(GameSceneReplay* const scene, POINT const point) {
    (void)scene;
    (void)point;
}

unsigned char gameSceneReplayMouseLDown(GameSceneReplay* const scene) {
    if (scene->m_Status == GS_NOREPLAY)
        return CR_TITLEMENU;

    return CR_NOSTATUS;
}

unsigned char gameSceneReplayKeyDown(GameSceneReplay* const scene, WPARAM const key) {
    (void)scene;
    switch (key) {
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
            SaveReplay(scene, (char)key);
            break;

        case VK_F12:
            return CR_TITLEMENU;

        case VK_ESCAPE:
            return CR_ENDGAME;
    }

    return CR_NOSTATUS;
}

void gameSceneReplayOnTimer(GameSceneReplay* const scene) {
    unsigned short i = ( unsigned short )scene->m_Played.size();
    if (scene->m_Tries > i) return;

    if (scene->m_bErase)
    {
        Onclick(scene);
        scene->m_bErase = false;
    }
    else
    {
        Onselect(scene, scene->m_Played[scene->m_Tries]);
        scene->m_bErase = true;
    }

    InvalidateRect(scene->m_hWnd, NULL, FALSE);
}

}


CREPLAY::CREPLAY(HWND hWnd, unsigned short wx, unsigned short wy, char cNum)
    : data(createGameSceneReplay(hWnd, wx, wy, cNum))
{
}

void CREPLAY::draw(Surface* const backSurface)
{
    gameSceneReplayDraw(data, backSurface);
}

void CREPLAY::Select(POINT pt)
{
    gameSceneReplayMouseMove(data, pt);
}

static void Onselect(GameSceneReplay* const scene, unsigned short pos) {
    static unsigned short s_sBef = 0;
    unsigned short        x, y;

    if (scene->m_Status != GS_PLAYING) return;

    Inexplore(scene, scene->m_by * scene->m_Width + scene->m_bx);
    scene->m_Num = 0;
    scene->m_bx  = 0;
    scene->m_by  = 0;

    x = pos % scene->m_Width;
    y = pos / scene->m_Width;

    if (pos >= scene->m_Width * scene->m_Height) return;
    if (scene->m_Area[pos] == 0) return;
    Explore(scene, pos, scene->m_Area[pos]);

    scene->m_bx = x;
    scene->m_by = y;

    if (scene->m_Num == 1) Unselect(scene);

    if (scene->m_Area[pos] & 0x80)
    {
        if (!(scene->m_Area[s_sBef] & 0x80))
        {
            PlaySound(SELWAV, NULL, SND_FILENAME | SND_ASYNC);
        }
    }
    s_sBef = pos;
}

static void Explore(GameSceneReplay* const scene, unsigned short pos, unsigned char piece) {
    if (pos >= scene->m_Width * scene->m_Height) return;
    if (piece == 0) return;
    if (scene->m_Area[pos] & ( unsigned char )0x80) return;

    if (scene->m_Area[pos] == piece)
    {
        scene->m_Area[pos] |= ( unsigned char )0x80;
        ++scene->m_Num;

        if (pos >= scene->m_Width) Explore(scene, pos - scene->m_Width, piece);
        if ((pos % scene->m_Width) != 0) Explore(scene, pos - 1, piece);
        if ((pos % scene->m_Width) < scene->m_Width - 1) Explore(scene, pos + 1, piece);
        if ((pos / scene->m_Width) < scene->m_Height - 1) Explore(scene, pos + scene->m_Width, piece);
    }
}

static void Unselect(GameSceneReplay* const scene) {
    Inexplore(scene, scene->m_by * scene->m_Width + scene->m_bx);
}

static void Inexplore(GameSceneReplay* const scene, unsigned short pos) {
    if (pos >= scene->m_Width * scene->m_Height) return;
    if (scene->m_Area[pos] & ( unsigned char )0x80)
    {
        scene->m_Area[pos] ^= ( unsigned char )0x80;

        if (pos >= scene->m_Width) Inexplore(scene, pos - scene->m_Width);
        if ((pos % scene->m_Width) != 0) Inexplore(scene, pos - 1);
        if ((pos % scene->m_Width) < scene->m_Width - 1) Inexplore(scene, pos + 1);
        if ((pos / scene->m_Width) < scene->m_Height - 1) Inexplore(scene, pos + scene->m_Width);
    }
}

unsigned char CREPLAY::Click(void)
{
    return gameSceneReplayMouseLDown(data);
}

static unsigned char Onclick(GameSceneReplay* const scene) {
    unsigned char cRet;

    if (scene->m_Status != GS_PLAYING) return CR_NOSTATUS;

    if (scene->m_Num < 2) return CR_NOSTATUS;
    if (scene->m_bx >= scene->m_Width || scene->m_by >= scene->m_Height) return CR_NOSTATUS;
    if (scene->m_Area[scene->m_by * scene->m_Width + scene->m_bx] == 0) return CR_NOSTATUS;

    Exexplore(scene, scene->m_by * scene->m_Width + scene->m_bx);

    ++scene->m_Tries;
    scene->m_Pieces -= scene->m_Num;
    AddScore(scene, ADDSCORE(scene->m_Num));

    Check(scene);

    cRet = EndCheck(scene);
    switch (cRet)
    {
        case CR_NOSTATUS:
            PlaySound(ERSWAV, NULL, SND_FILENAME | SND_SYNC);
            scene->m_Num = 0;
            Explore(scene, scene->m_by * scene->m_Width + scene->m_bx, scene->m_Area[scene->m_by * scene->m_Width + scene->m_bx]);
            if (scene->m_Num == 1) Unselect(scene);
            break;

        case CR_CLEAR:
            PlaySound(CLRWAV, NULL, SND_FILENAME | SND_ASYNC);
            scene->m_Status = GS_CLEAR;
            break;

        case CR_ALLCLEAR:
            AddScore(scene, 1000000);
            PlaySound(CLRWAV, NULL, SND_FILENAME | SND_ASYNC);
            scene->m_Status = GS_ALLCLEAR;
            break;
    }

    return cRet;
}

static void Exexplore(GameSceneReplay* const scene, unsigned short pos) {
    if (pos >= scene->m_Width * scene->m_Height) return;

    if (scene->m_Area[pos] & ( unsigned char )0x80)
    {
        scene->m_Area[pos] = 0;

        if (pos >= scene->m_Width) Exexplore(scene, pos - scene->m_Width);
        if ((pos % scene->m_Width) != 0) Exexplore(scene, pos - 1);
        if ((pos % scene->m_Width) < scene->m_Width - 1) Exexplore(scene, pos + 1);
        if ((pos / scene->m_Width) < scene->m_Height - 1) Exexplore(scene, pos + scene->m_Width);
    }
}

static void Check(GameSceneReplay* const scene) {
    unsigned short i, j, max;

    max = scene->m_Width * scene->m_Height;

    for (i = 0; i < max; ++i)
    {
        if (scene->m_Area[i] == 0) VShift(scene, i);
    }

    for (i = 0; i < scene->m_Width - 1; ++i)
    {
        if (scene->m_Area[(scene->m_Height - 1) * scene->m_Width + i] == 0)
        {
            for (j = i; j < scene->m_Width; ++j)
            {
                if (scene->m_Area[(scene->m_Height - 1) * scene->m_Width + j] != 0)
                    break;
            }

            if (j == scene->m_Width) break; ;

            for (j = 0; j < scene->m_Height; ++j)
                HShift(scene, j * scene->m_Width + i);

            --i;
        }
    }
}

static void VShift(GameSceneReplay* const scene, unsigned short pos) {
    if (pos >= scene->m_Width * scene->m_Height) return;

    if (pos >= scene->m_Width)
    {
        scene->m_Area[pos] = scene->m_Area[pos - scene->m_Width];
        VShift(scene, pos - scene->m_Width);
    }
    else
        scene->m_Area[pos] = 0;
}

static void HShift(GameSceneReplay* const scene, unsigned short pos) {
    if (pos >= scene->m_Width * scene->m_Height) return;

    if (pos % scene->m_Width < scene->m_Width - 1)
    {
        scene->m_Area[pos] = scene->m_Area[pos + 1];
        HShift(scene, pos + 1);
    }
    else
        scene->m_Area[pos] = 0;
}

static unsigned char EndCheck(GameSceneReplay* const scene) {
    bool bAll;

    bAll = CntGroups(scene);

    if (bAll) return CR_ALLCLEAR;
    else if (scene->m_Groups == 0)
        return CR_CLEAR;

    return CR_NOSTATUS;
}

static bool CntGroups(GameSceneReplay* const scene) {
    unsigned short max, i;
    unsigned char  cPiece = 0;

    max      = scene->m_Width * scene->m_Height;
    scene->m_Groups = 0;

    for (i = 0; i < max; ++i)
    {
        cPiece |= (scene->m_Area[i] != 0);
        if (scene->m_Area[i] == 0) continue;
        if (scene->m_Area[i] & ( unsigned char )0x80) continue;

        scene->m_Num = 0;
        Explore(scene, i, scene->m_Area[i]);
        if (scene->m_Num > 1)
        {
            ++scene->m_Groups;
        }
    }

    // 後始末
    for (i = 0; i < max; ++i)
    {
        if (scene->m_Area[i] == 0) continue;
        if (!(scene->m_Area[i] & ( unsigned char )0x80)) continue;

        Inexplore(scene, i);
    }

    return cPiece == 0;
}

static void AddScore(GameSceneReplay* const scene, unsigned long add) {
    scene->m_Score += add;
}

unsigned char CREPLAY::KeyDown(WPARAM key)
{
    return gameSceneReplayKeyDown(data, key);
}

void CREPLAY::Replay(void)
{
    gameSceneReplayOnTimer(data);
}

CREPLAY::~CREPLAY(void)
{
    destroyGameSceneReplay(data);
}

static bool LoadReplay(GameSceneReplay* const scene, char cNum) {
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
    ReadFile(hFile, ( LPVOID )&scene->m_GameNum, sizeof(unsigned long), &dwRead, NULL);
    if (dwRead != sizeof(unsigned long)) return false;

    ReadFile(hFile, ( LPVOID )&scene->m_Tries, sizeof(unsigned short), &dwRead, NULL);
    if (dwRead != sizeof(unsigned short)) return false;

    for (i = 0; i < scene->m_Tries; ++i)
    {
        ReadFile(hFile, ( LPVOID )&ucDat, 1, &dwRead, NULL);
        if (dwRead != 1) return false;
        scene->m_Played.push_back(ucDat);
    }

    CloseHandle(hFile);

    scene->m_cRepNum = cNum;

    return true;
}

static void SaveReplay(GameSceneReplay* const scene, char cNum) {
    char strSrcName[0x100], strDstName[0x100];

    lstrcpy(strSrcName, REPFILE);
    strSrcName[lstrlen(strSrcName) + 1] = '\0';
    strSrcName[lstrlen(strSrcName)]     = scene->m_cRepNum;

    lstrcpy(strDstName, REPFILE);
    strDstName[lstrlen(strDstName) + 1] = '\0';
    strDstName[lstrlen(strDstName)]     = cNum;

    CopyFile(strSrcName, strDstName, TRUE);
}

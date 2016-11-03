// game.c

#include "game.h"

#include <stdbool.h>

#include "defines.h"


static void Explore(GameSceneGame* scene, unsigned short pos, unsigned char piece);
static void Unselect(GameSceneGame* scene);
static void Inexplore(GameSceneGame* scene, unsigned short pos);
static void Exexplore(GameSceneGame* scene, unsigned short pos);
static void Check(GameSceneGame* scene);
static void VShift(GameSceneGame* scene, unsigned short pos);
static void HShift(GameSceneGame* scene, unsigned short pos);
static enum CLICKRESULT EndCheck(GameSceneGame* scene);
static bool CntGroups(GameSceneGame* scene);
static void AddScore(GameSceneGame* scene, unsigned long add);
static void LoadStatus(GameSceneGame* scene);
static void SaveStatus(GameSceneGame* scene);
static void SaveReplay(GameSceneGame* scene, char cNum);
static unsigned char* makeArea(unsigned short width, unsigned short height, unsigned long gameNumber);
static bool selectsAt(GameSceneGame* scene, unsigned short x, unsigned short y);


GameSceneGame* createGameSceneGame(unsigned int const width, unsigned int const height, int const maskLevel, unsigned long const seed) {
    GameSceneGame* scene = (GameSceneGame*)malloc(sizeof(GameSceneGame));

    scene->surface = surfaceFromBitmapFile(DATA(system.bmp));

    scene->m_Level     = 0;
    scene->m_HighScore = 0;

    LoadStatus(scene);

    scene->m_Status = GS_PLAYING;

    scene->m_cMaskNum = maskLevel;

    scene->m_Width   = width / PIX;
    scene->m_Height  = height / PIY;
    scene->m_bx      = scene->m_Width;
    scene->m_by      = scene->m_Height;
    scene->m_bDraw   = false;
    scene->m_bReGame = false;
    scene->previousPosition = scene->m_Width * scene->m_Height;

    SetRect(&scene->m_rcArea, 0, 0, width, height);

    scene->m_Num    = 0;
    scene->m_Tries  = 0;
    scene->m_Pieces = scene->m_Width * scene->m_Height;
    scene->m_Groups = 0;
    scene->m_Score  = 0;

    scene->m_GameNum = seed;
    scene->m_Area    = makeArea(scene->m_Width, scene->m_Height, scene->m_GameNum);

    CntGroups(scene);

    scene->m_Played = (unsigned char*)malloc(sizeof(unsigned char) * 256);
    scene->m_PlayedCapacity = 256;

    return scene;
}

void destroyGameSceneGame(GameSceneGame* const scene) {
    SaveStatus(scene);

    destroySurface(scene->surface);
    free(scene->m_Area);
    free(scene->m_Played);

    free(scene);
}

void gameSceneGameDraw(GameSceneGame* const scene, Surface* const backSurface) {
    // ゲーム盤の描画
    if (scene->m_cMaskNum < 4)
    {
        for (unsigned short i = 0; i < scene->m_Height; ++i)
        {
            for (unsigned short j = 0; j < scene->m_Width; ++j)
            {
                unsigned char tmp = scene->m_Area[i * scene->m_Width + j];

                if (tmp != 0)
                {
                    --tmp;
                    if (scene->m_cMaskNum == 3) tmp &= 0x80;

                    if (selectsAt(scene, j, i))
                    {
                        if (scene->m_cMaskNum == 1) continue;

                        tmp = tmp ^ ( unsigned char )0x80;

                        surfaceBlit(backSurface, j * PIX, i * PIY, PIX, PIY, scene->surface, tmp * PIX, PIY * 2, SRCAND);
                        surfaceBlit(backSurface, j * PIX, i * PIY, PIX, PIY, scene->surface, tmp * PIX, PIY, SRCPAINT);
                    }
                    else
                    {
                        if (scene->m_cMaskNum == 2) continue;

                        surfaceBlit(backSurface, j * PIX, i * PIY, PIX, PIY, scene->surface, tmp * 32, PIY * 2, SRCAND);
                        surfaceBlit(backSurface, j * PIX, i * PIY, PIX, PIY, scene->surface, tmp * 32, 0, SRCPAINT);
                    }
                }
            }
        }
    }

    char strTmp[0x100];

    // その他の描画
    wsprintf(strTmp, "　　スコア：%lu", scene->m_Score);
    surfaceDrawText(backSurface, scene->m_rcArea.right, 0, 20, RGB(255, 255, 255), strTmp);
    wsprintf(strTmp, "ハイスコア：%lu", scene->m_HighScore);
    surfaceDrawText(backSurface, scene->m_rcArea.right, 20, 20, RGB(255, 255, 255), strTmp);

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

        surfaceDrawText(backSurface, scene->m_rcArea.right, 260, 20, RGB(255, 255, 255), "Enter:再ゲーム");
        surfaceDrawText(backSurface, scene->m_rcArea.right, 280, 20, RGB(255, 255, 255), "F12  :メニューに戻る");
        surfaceDrawText(backSurface, scene->m_rcArea.right, 300, 20, RGB(255, 255, 255), "Esc  :終了");

        surfaceDrawText(backSurface, scene->m_rcArea.right, 340, 20, RGB(255, 255, 255), "キーボードの0～9を押すと");
        surfaceDrawText(backSurface, scene->m_rcArea.right + 40, 360, 20, RGB(255, 255, 255), "リプレイを保存します");

        if (scene->m_Score >= 10000 && scene->m_Level - 1 == scene->m_cMaskNum)
        {
            switch (scene->m_Level)
            {
                case 1:
                    surfaceDrawText(backSurface, scene->m_rcArea.right + 40, 400, 20, RGB(0, 255, 255), "“マスクモード”出現！");
                    break;

                case 2:
                    surfaceDrawText(backSurface, scene->m_rcArea.right + 20, 400, 20, RGB(0, 255, 255), "マスクモードレベル２出現！");
                    break;

                case 3:
                    surfaceDrawText(backSurface, scene->m_rcArea.right + 20, 400, 20, RGB(0, 255, 255), "マスクモードレベル３出現！");
                    break;

                case 4:
                    surfaceDrawText(backSurface, scene->m_rcArea.right + 20, 400, 20, RGB(0, 255, 255), "マスクモードレベル４出現！");
                    break;
            }
        }
    }

    scene->m_bDraw = true;
}

void gameSceneGameMouseMove(GameSceneGame* const scene, POINT const point) {
    scene->m_bReGame = false;
    if (scene->m_Status == GS_CLEAR || scene->m_Status == GS_ALLCLEAR)
    {
        RECT rc;
        SetRect(&rc, scene->m_rcArea.right, 260, scene->m_rcArea.right * 2, 320);
        if (PtInRect(&rc, point))
        {
            scene->m_bReGame = true;
        }
        return;
    }

    Inexplore(scene, scene->m_by * scene->m_Width + scene->m_bx);
    scene->m_Num = 0;
    scene->m_bx  = scene->m_Width;
    scene->m_by  = scene->m_Height;

    if (PtInRect(&scene->m_rcArea, point))
    {
        unsigned short const x = ( unsigned short )point.x / PIX;
        unsigned short const y = ( unsigned short )point.y / PIY;
        scene->m_bx = x;
        scene->m_by = y;

        unsigned short const pos = y * scene->m_Width + x;
        if (pos >= scene->m_Width * scene->m_Height) return;
        if (scene->m_Area[pos] == 0)
        {
            scene->previousPosition = pos;
            return;
        }
        Explore(scene, pos, scene->m_Area[pos]);

        if (scene->m_Num == 1) Unselect(scene);

        if (selectsAt(scene, x, y))
        {
            if (!(scene->m_Area[scene->previousPosition] & 0x80))
            {
                PlaySound(SELWAV, NULL, SND_FILENAME | SND_ASYNC);
            }
        }
        scene->previousPosition = pos;
    }
    else
    {
        scene->previousPosition = scene->m_Width * scene->m_Height;
    }
}

unsigned char gameSceneGameMouseLDown(GameSceneGame* const scene) {
    if (!scene->m_bDraw) return CR_NOSTATUS;

    if (scene->m_Status == GS_CLEAR || scene->m_Status == GS_ALLCLEAR)
    {
        if (scene->m_bReGame) return CR_BEGINNORMAL + scene->m_cMaskNum;
        return CR_NOSTATUS;
    }

    if (scene->m_Num < 2) return CR_NOSTATUS;
    if (scene->m_bx >= scene->m_Width || scene->m_by >= scene->m_Height) return CR_NOSTATUS;
    if (scene->m_Area[scene->m_by * scene->m_Width + scene->m_bx] == 0) return CR_NOSTATUS;

    Exexplore(scene, scene->m_by * scene->m_Width + scene->m_bx);

    ++scene->m_Tries;
    scene->m_Pieces -= scene->m_Num;
    AddScore(scene, ADDSCORE(scene->m_Num));

    size_t const playedIndex = scene->m_Tries - 1;
    if (playedIndex >= scene->m_PlayedCapacity) {
        scene->m_PlayedCapacity = (size_t)((double)scene->m_PlayedCapacity * 1.5);
        scene->m_Played = (unsigned char*)realloc(scene->m_Played, sizeof(unsigned char) * scene->m_PlayedCapacity);
    }
    scene->m_Played[playedIndex] = (unsigned char)(scene->m_by * scene->m_Width + scene->m_bx);

    Check(scene);

    enum CLICKRESULT const cRet = EndCheck(scene);
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

        default:
            break;
    }

    if (cRet == CR_CLEAR || cRet == CR_ALLCLEAR)
    {
        SaveReplay(scene, '\0');
        if (scene->m_Score >= 10000 && scene->m_Level == scene->m_cMaskNum)
        {
            if (++scene->m_Level > MASKMAX + 1) scene->m_Level = MASKMAX + 1;
        }
    }

    scene->m_bDraw = false;

    return cRet;
}

unsigned char gameSceneGameKeyDown(GameSceneGame* const scene, WPARAM const key) {
    switch (key) {
        case VK_RETURN:
            if (scene->m_Status == GS_CLEAR || scene->m_Status == GS_ALLCLEAR)
                return CR_BEGINNORMAL + scene->m_cMaskNum;
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
            SaveReplay(scene, (char)key);
            break;

        case VK_F8:
            return CR_BEGINNORMAL + scene->m_cMaskNum;

        case VK_F12:
            return CR_TITLEMENU;

        case VK_ESCAPE:
            return CR_ENDGAME;
    }

    return CR_NOSTATUS;
}


static void Explore(GameSceneGame* const scene, unsigned short pos, unsigned char piece) {
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

static void Unselect(GameSceneGame* const scene) {
    Inexplore(scene, scene->m_by * scene->m_Width + scene->m_bx);
}

static void Inexplore(GameSceneGame* const scene, unsigned short pos) {
    if (pos >= scene->m_Width * scene->m_Height) return;
    if (scene->m_Area[pos] & (unsigned char)0x80)
    {
        scene->m_Area[pos] ^= (unsigned char)0x80;

        if (pos >= scene->m_Width) Inexplore(scene, pos - scene->m_Width);
        if ((pos % scene->m_Width) != 0) Inexplore(scene, pos - 1);
        if ((pos % scene->m_Width) < scene->m_Width - 1) Inexplore(scene, pos + 1);
        if ((pos / scene->m_Width) < scene->m_Height - 1) Inexplore(scene, pos + scene->m_Width);
    }
}

static void Exexplore(GameSceneGame* const scene, unsigned short pos) {
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

static void Check(GameSceneGame* const scene) {
    unsigned short const max = scene->m_Width * scene->m_Height;

    for (unsigned short i = 0; i < max; ++i)
    {
        if (scene->m_Area[i] == 0) VShift(scene, i);
    }

    for (unsigned short i = 0; i < scene->m_Width - 1; ++i)
    {
        if (scene->m_Area[(scene->m_Height - 1) * scene->m_Width + i] != 0) continue;

        unsigned short j;

        for (j = i; j < scene->m_Width; ++j)
        {
            if (scene->m_Area[(scene->m_Height - 1) * scene->m_Width + j] != 0)
                break;
        }

        if (j == scene->m_Width) break;

        for (unsigned short k = 0; k < scene->m_Height; ++k)
            HShift(scene, k * scene->m_Width + i);

        --i;
    }
}

static void VShift(GameSceneGame* const scene, unsigned short pos) {
    if (pos >= scene->m_Width * scene->m_Height) return;

    if (pos >= scene->m_Width)
    {
        scene->m_Area[pos] = scene->m_Area[pos - scene->m_Width];
        VShift(scene, pos - scene->m_Width);
    }
    else
        scene->m_Area[pos] = 0;
}

static void HShift(GameSceneGame* const scene, unsigned short pos) {
    if (pos >= scene->m_Width * scene->m_Height) return;

    if (pos % scene->m_Width < scene->m_Width - 1)
    {
        scene->m_Area[pos] = scene->m_Area[pos + 1];
        HShift(scene, pos + 1);
    }
    else
        scene->m_Area[pos] = 0;
}

static enum CLICKRESULT EndCheck(GameSceneGame* const scene) {
    bool const bAll = CntGroups(scene);

    if (bAll) return CR_ALLCLEAR;
    else if (scene->m_Groups == 0)
        return CR_CLEAR;

    return CR_NOSTATUS;
}

static bool CntGroups(GameSceneGame* const scene) {
    unsigned char cPiece = 0;

    unsigned short const max = scene->m_Width * scene->m_Height;
    scene->m_Groups = 0;

    for (unsigned short i = 0; i < max; ++i)
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
    for (unsigned short i = 0; i < max; ++i)
    {
        if (scene->m_Area[i] == 0) continue;
        if (!(scene->m_Area[i] & ( unsigned char )0x80)) continue;

        Inexplore(scene, i);
    }

    return cPiece == 0;
}

static void AddScore(GameSceneGame* const scene, unsigned long add) {
    scene->m_Score += add;
    if (scene->m_Score > scene->m_HighScore) scene->m_HighScore = scene->m_Score;
}

static void LoadStatus(GameSceneGame* const scene) {
    HANDLE const hFile = CreateFile(DATFILE, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return;

    SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
    DWORD dwRead;
    ReadFile(hFile, &scene->m_Level, 1, &dwRead, NULL);

    DWORD const dwSize = sizeof(unsigned long);
    ReadFile(hFile, &scene->m_HighScore, dwSize, &dwRead, NULL);
    char* const data = (char*)&scene->m_HighScore;
    for (size_t i = 0; i < sizeof(unsigned long); ++i)
        data[i] -= CODE(i + 1);

    CloseHandle(hFile);
}

static void SaveStatus(GameSceneGame* const scene) {
    char* const data = (char*)&scene->m_HighScore;
    for (size_t i = 0; i < sizeof(unsigned long); ++i)
        data[i] += CODE(i + 1);

    HANDLE const hFile = CreateFile(DATFILE, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return;

    SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
    DWORD dwWritten;
    WriteFile(hFile, &scene->m_Level, 1, &dwWritten, NULL);

    DWORD const dwSize = sizeof(unsigned long);
    WriteFile(hFile, &scene->m_HighScore, dwSize, &dwWritten, NULL);

    CloseHandle(hFile);
}

static void SaveReplay(GameSceneGame* const scene, char cNum) {
    char strFName[0x100];

    lstrcpy(strFName, REPFILE);
    strFName[lstrlen(strFName) + 1] = '\0';
    strFName[lstrlen(strFName)]     = cNum;

    HANDLE const hFile = CreateFile(strFName, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return;

    SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
    DWORD dwWritten;
    WriteFile(hFile, &scene->m_GameNum, sizeof(unsigned long), &dwWritten, NULL);
    WriteFile(hFile, &scene->m_Tries, sizeof(unsigned short), &dwWritten, NULL);

    for (unsigned short i = 0; i < scene->m_Tries; ++i)
    {
        WriteFile(hFile, &scene->m_Played[i], 1, &dwWritten, NULL);
    }

    CloseHandle(hFile);
}

static unsigned char* makeArea(unsigned short const width,
                               unsigned short const height,
                               unsigned long const gameNumber)
{
    unsigned char* const area = malloc(sizeof(unsigned char) * width * height);

    srand(gameNumber);
    for (unsigned short y = 0; y < height; ++y)
    {
        for (unsigned short x = 0; x < width; ++x)
        {
            area[x + y * width] = (unsigned char)(rand() % 5 + 1);
        }
    }

    return area;
}

static bool selectsAt(GameSceneGame* const scene, unsigned short const x, unsigned short const y) {
    return (scene->m_Area[x + y * scene->m_Width] & 0x80) != 0;
}

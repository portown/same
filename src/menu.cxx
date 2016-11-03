// menu.cpp

#include "common.hxx"


static void LoadStatus(GameSceneMenu* scene);
static void SaveStatus(GameSceneMenu* scene);


extern "C" {

GameSceneMenu* createGameSceneMenu(unsigned int const width, unsigned int const height) {
    GameSceneMenu* const scene = (GameSceneMenu*)malloc(sizeof(GameSceneMenu));

    scene->m_Level = 0;

    LoadStatus(scene);

    scene->m_Width   = width;
    scene->m_Height  = height;
    scene->m_Sel     = 0;
    scene->m_MaskNum = 0;
    scene->m_RepNum  = -1;

    SetRect(&scene->m_rcMenu, 230, 260, 230 + 180, 260 + 160);
    SetRect(&scene->m_rcLeft, scene->m_rcMenu.right, scene->m_rcMenu.top, scene->m_rcMenu.right + 20, scene->m_rcMenu.bottom);
    SetRect(&scene->m_rcRight, scene->m_rcLeft.left + 80, scene->m_rcLeft.top, scene->m_rcLeft.right + 80, scene->m_rcLeft.bottom);

    scene->surface = surfaceFromBitmapFile(DATA(title.bmp));
    scene->menuSurface = surfaceFromBitmapFile(DATA(menu.bmp));

    return scene;
}

void destroyGameSceneMenu(GameSceneMenu* const scene) {
    SaveStatus(scene);

    destroySurface(scene->menuSurface);
    destroySurface(scene->surface);

    free(scene);
}

void gameSceneMenuDraw(GameSceneMenu* const scene, Surface* const backSurface) {
    int i;

    surfaceBlit(backSurface, 0, 0, scene->m_Width, scene->m_Height, scene->surface, 0, 0, SRCCOPY);

    for (i = 0; i < 4; ++i)
    {
        if (i == 1 && scene->m_Level < 1) continue;

        if (i + 1 == scene->m_Sel)
        {
            surfaceBlit(backSurface, scene->m_rcMenu.left, scene->m_rcMenu.top + i * (scene->m_rcMenu.bottom - scene->m_rcMenu.top) / 4,
                   scene->m_rcMenu.right - scene->m_rcMenu.left, (scene->m_rcMenu.bottom - scene->m_rcMenu.top) / 4,
                   scene->menuSurface, scene->m_rcMenu.right - scene->m_rcMenu.left, i * (scene->m_rcMenu.bottom - scene->m_rcMenu.top) / 4, SRCCOPY);
        }
        else
        {
            surfaceBlit(backSurface, scene->m_rcMenu.left, scene->m_rcMenu.top + i * (scene->m_rcMenu.bottom - scene->m_rcMenu.top) / 4,
                   scene->m_rcMenu.right - scene->m_rcMenu.left, (scene->m_rcMenu.bottom - scene->m_rcMenu.top) / 4,
                   scene->menuSurface, 0, i * (scene->m_rcMenu.bottom - scene->m_rcMenu.top) / 4, SRCCOPY);
        }
    }

    if (scene->m_Level >= 1)
    {
        if (scene->m_Sel == 6)
        {
            surfaceBlit(backSurface, scene->m_rcLeft.left, scene->m_rcLeft.top + (scene->m_rcLeft.bottom - scene->m_rcLeft.top) / 4,
                   20, 40, scene->menuSurface, 40, 160, SRCCOPY);
        }
        else
        {
            surfaceBlit(backSurface, scene->m_rcLeft.left, scene->m_rcLeft.top + (scene->m_rcLeft.bottom - scene->m_rcLeft.top) / 4,
                   20, 40, scene->menuSurface, 0, 160, SRCCOPY);
        }
        if (scene->m_Sel == 10)
        {
            surfaceBlit(backSurface, scene->m_rcRight.left, scene->m_rcRight.top + (scene->m_rcRight.bottom - scene->m_rcRight.top) / 4,
                   20, 40, scene->menuSurface, 60, 160, SRCCOPY);
        }
        else
        {
            surfaceBlit(backSurface, scene->m_rcRight.left, scene->m_rcRight.top + (scene->m_rcRight.bottom - scene->m_rcRight.top) / 4,
                   20, 40, scene->menuSurface, 20, 160, SRCCOPY);
        }

        surfaceBlit(backSurface, scene->m_rcLeft.right + 36, scene->m_rcLeft.top + (scene->m_rcLeft.bottom - scene->m_rcLeft.top) / 4,
               19, 40, scene->menuSurface, 80 + scene->m_MaskNum * 19, 160, SRCCOPY);
    }

    if (scene->m_Sel == 7)
    {
        surfaceBlit(backSurface, scene->m_rcLeft.left, scene->m_rcLeft.top + 2 * (scene->m_rcLeft.bottom - scene->m_rcLeft.top) / 4,
               20, 40, scene->menuSurface, 40, 160, SRCCOPY);
    }
    else
    {
        surfaceBlit(backSurface, scene->m_rcLeft.left, scene->m_rcLeft.top + 2 * (scene->m_rcLeft.bottom - scene->m_rcLeft.top) / 4,
               20, 40, scene->menuSurface, 0, 160, SRCCOPY);
    }
    if (scene->m_Sel == 11)
    {
        surfaceBlit(backSurface, scene->m_rcRight.left, scene->m_rcRight.top + 2 * (scene->m_rcRight.bottom - scene->m_rcRight.top) / 4,
               20, 40, scene->menuSurface, 60, 160, SRCCOPY);
    }
    else
    {
        surfaceBlit(backSurface, scene->m_rcRight.left, scene->m_rcRight.top + 2 * (scene->m_rcRight.bottom - scene->m_rcRight.top) / 4,
               20, 40, scene->menuSurface, 20, 160, SRCCOPY);
    }

    if (scene->m_RepNum == -1)
    {
        surfaceBlit(backSurface, scene->m_rcLeft.right + 5, scene->m_rcLeft.top + 2 * (scene->m_rcLeft.bottom - scene->m_rcLeft.top) / 4,
               50, 40, scene->menuSurface, 271, 160, SRCCOPY);
    }
    else
    {
        surfaceBlit(backSurface, scene->m_rcLeft.right + 36, scene->m_rcLeft.top + 2 * (scene->m_rcLeft.bottom - scene->m_rcLeft.top) / 4,
               19, 40, scene->menuSurface, 80 + scene->m_RepNum * 19, 160, SRCCOPY);
    }
}

void gameSceneMenuMouseMove(GameSceneMenu* const scene, POINT const point) {
    unsigned char before = scene->m_Sel;

    scene->m_Sel = 0;

    if (PtInRect(&scene->m_rcMenu, point))
    {
        scene->m_Sel = ( unsigned short )(point.y - scene->m_rcMenu.top) / 40 + 1;
        if (scene->m_Sel == 2)
        {
            scene->m_Sel = 0;
            return;
        }
    }
    if (PtInRect(&scene->m_rcLeft, point))
    {
        scene->m_Sel = ( unsigned short )(point.y - scene->m_rcLeft.top) / 40 + 5;
        if (scene->m_Sel != 7 && (scene->m_Level < 1 || scene->m_Sel != 6))
        {
            scene->m_Sel = 0;
            return;
        }
    }
    if (PtInRect(&scene->m_rcRight, point))
    {
        scene->m_Sel = ( unsigned short )(point.y - scene->m_rcLeft.top) / 40 + 9;
        if (scene->m_Sel != 11 && (scene->m_Level < 1 || scene->m_Sel != 10))
        {
            scene->m_Sel = 0;
            return;
        }
    }

    if (scene->m_Sel != before && scene->m_Sel)
        PlaySound(SELWAV, NULL, SND_FILENAME | SND_ASYNC);
}

unsigned char gameSceneMenuMouseLDown(GameSceneMenu* const scene) {
    switch (scene->m_Sel) {
        case 1:
            PlaySound(CLRWAV, NULL, SND_FILENAME | SND_ASYNC);
            return CR_BEGINNORMAL + scene->m_MaskNum;

        case 3:
            PlaySound(CLRWAV, NULL, SND_FILENAME | SND_ASYNC);
            return CR_REPLAY0 + scene->m_RepNum;

        case 4:
            PlaySound(CLRWAV, NULL, SND_FILENAME | SND_SYNC);
            return CR_ENDGAME;

        case 6:
            PlaySound(ERSWAV, NULL, SND_FILENAME | SND_ASYNC);
            if (--scene->m_MaskNum < 0) scene->m_MaskNum = 0;
            break;

        case 7:
            PlaySound(ERSWAV, NULL, SND_FILENAME | SND_ASYNC);
            if (--scene->m_RepNum < -1) scene->m_RepNum = -1;
            break;

        case 10:
            PlaySound(ERSWAV, NULL, SND_FILENAME | SND_ASYNC);
            if (++scene->m_MaskNum > std::min<char>(scene->m_Level, MASKMAX)) scene->m_MaskNum = std::min<char>(scene->m_Level, MASKMAX);
            break;

        case 11:
            PlaySound(ERSWAV, NULL, SND_FILENAME | SND_ASYNC);
            if (++scene->m_RepNum > 9) scene->m_RepNum = 9;
            break;
    }

    return CR_NOSTATUS;
}

unsigned char gameSceneMenuKeyDown(GameSceneMenu* const scene, WPARAM const key) {
    (void)scene;
    switch (key) {
        case VK_ESCAPE:
            return CR_ENDGAME;
    }

    return CR_NOSTATUS;
}

}


CMENU::CMENU(unsigned short width, unsigned short height)
    : data(createGameSceneMenu(width, height))
{
}

void CMENU::draw(Surface* const backSurface)
{
    gameSceneMenuDraw(data, backSurface);
}

void CMENU::Select(POINT pt)
{
    gameSceneMenuMouseMove(data, pt);
}

unsigned char CMENU::Click(void)
{
    return gameSceneMenuMouseLDown(data);
}

unsigned char CMENU::KeyDown(WPARAM key)
{
    return gameSceneMenuKeyDown(data, key);
}

CMENU::~CMENU(void)
{
    destroyGameSceneMenu(data);
}

static void LoadStatus(GameSceneMenu* const scene) {
    HANDLE hFile;
    DWORD  dwRead;

    hFile = CreateFile(DATFILE, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return;

    SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
    ReadFile(hFile, ( LPVOID )&scene->m_Level, 1, &dwRead, NULL);

    CloseHandle(hFile);
}

static void SaveStatus(GameSceneMenu* const scene) {
    HANDLE hFile;
    DWORD  dwWritten;

    hFile = CreateFile(DATFILE, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return;

    SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
    WriteFile(hFile, ( LPCVOID )&scene->m_Level, 1, &dwWritten, NULL);

    CloseHandle(hFile);
}

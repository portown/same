// gtips.c

#include "funcs.h"


void InitSurface(HDC* hDC, HBITMAP* hBm, unsigned short w, unsigned short h)
{
    HDC hTemp = GetDC(NULL);

    *hDC = CreateCompatibleDC(hTemp);
    *hBm = CreateCompatibleBitmap(hTemp, w, h);
    if (*hBm == NULL)
    {
        Mes("サーフェイスを正しく作成できませんでした");
        return;
    }
    SelectObject(*hDC, *hBm);

    ReleaseDC(NULL, hTemp);

    RECT rc;
    rc.left   = rc.top = 0;
    rc.right  = w;
    rc.bottom = h;
    PaintRect(*hDC, &rc, RGB(0, 0, 0));
}

void RelsSurface(HDC* hDC, HBITMAP* hBm)
{
    if (*hBm)
    {
        DeleteObject(*hBm);
        *hBm = NULL;
    }
    if (*hDC)
    {
        DeleteDC(*hDC);
        *hDC = NULL;
    }
}

void PaintRect(HDC hDC, LPRECT lprc, COLORREF col)
{
    HBRUSH hBrush, hOld;

    hBrush = CreateSolidBrush(col);

    hOld = ( HBRUSH )SelectObject(hDC, hBrush);
    if (lprc == NULL)
    {
        RECT rc;
        SetRect(&rc, 0, 0, 640, 480);
        PatBlt(hDC, rc.left, rc.top, rc.right - rc.left,
               rc.bottom - rc.top, PATCOPY);
    }
    else
    {
        PatBlt(hDC, lprc->left, lprc->top, lprc->right - lprc->left,
               lprc->bottom - lprc->top, PATCOPY);
    }
    SelectObject(hDC, hOld);

    DeleteObject(hBrush);
}

int PutText(HDC hDC, int x, int y, int f_size,
             COLORREF col, char const* str)
{
    COLORREF clOld;
    HFONT    hFont, hOldFont;
    int      iOldMode;

    int const iLen = lstrlen(str);

    hFont = CreateFont(f_size,
                       0,
                       0,
                       0,
                       600,
                       0,
                       0,
                       0,
                       SHIFTJIS_CHARSET,
                       OUT_DEFAULT_PRECIS,
                       CLIP_DEFAULT_PRECIS,
                       DEFAULT_QUALITY,
                       DEFAULT_PITCH | FF_DONTCARE,
                       "ＭＳ ゴシック");

    if (!hFont)
        return 0;

    iOldMode = SetBkMode(hDC, TRANSPARENT);
    hOldFont = ( HFONT )SelectObject(hDC, hFont);
    clOld    = SetTextColor(hDC, col);

    TextOut(hDC, x, y, str, iLen);

    SetTextColor(hDC, clOld);
    SelectObject(hDC, hOldFont);
    SetBkMode(hDC, iOldMode);

    DeleteObject(hFont);

    return 1;
}

HBITMAP Load_Bmp(const char* f_name)
{
    return (HBITMAP)LoadImage(NULL, f_name, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
}

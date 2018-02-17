// gtips.cpp

#include "funcs.hxx"


bool PutText(HDC hDC, int x, int y, int f_size,
             COLORREF col, tch::tstring_view str)
{
    COLORREF clOld;
    HFONT    hFont, hOldFont;
    int      iOldMode;

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
                       TEXT("MS Gothic"));

    if (!hFont)
        return false;

    iOldMode = SetBkMode(hDC, TRANSPARENT);
    hOldFont = ( HFONT )SelectObject(hDC, hFont);
    clOld    = SetTextColor(hDC, col);

    TextOut(hDC, x, y, str.data(), str.size());

    SetTextColor(hDC, clOld);
    SelectObject(hDC, hOldFont);
    SetBkMode(hDC, iOldMode);

    DeleteObject(hFont);

    return true;
}

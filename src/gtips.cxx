// gtips.cpp

#include "funcs.hxx"


bool PutText(HDC hDC, int x, int y, int f_size,
             COLORREF col, std::wstring_view str)
{
    auto const hFont = CreateFont(f_size,
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
                                  L"MS Gothic");

    if (!hFont)
        return false;

    auto const iOldMode = SetBkMode(hDC, TRANSPARENT);
    auto const hOldFont = SelectObject(hDC, hFont);
    auto const clOld    = SetTextColor(hDC, col);

    TextOut(hDC, x, y, str.data(), str.size());

    SetTextColor(hDC, clOld);
    SelectObject(hDC, hOldFont);
    SetBkMode(hDC, iOldMode);

    DeleteObject(hFont);

    return true;
}

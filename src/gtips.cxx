// gtips.cpp

#include "common.hxx"


namespace
{
    void PaintRect(std::shared_ptr<same::ui::Surface> const& surface, LPRECT lprc, COLORREF col)
    {
        auto const hBrush = CreateSolidBrush(col);

        auto const hOld = SelectObject(surface->dcHandle_, hBrush);
        if (lprc == nullptr)
        {
            PatBlt(surface->dcHandle_, 0, 0, surface->getWidth(), surface->getHeight(), PATCOPY);
        }
        else
        {
            PatBlt(surface->dcHandle_, lprc->left, lprc->top, lprc->right - lprc->left,
                    lprc->bottom - lprc->top, PATCOPY);
        }
        SelectObject(surface->dcHandle_, hOld);

        DeleteObject(hBrush);
    }
}


// デバイスコンテキスト・ビットマップハンドルの初期化
std::shared_ptr<same::ui::Surface> InitSurface(unsigned short w, unsigned short h)
{
    auto const surface = same::ui::Surface::create(w, h);

    PaintRect(surface, nullptr, RGB(0, 0, 0));

    return surface;
}

// 文字の描画
bool PutText(HDC hDC, int x, int y, int f_size,
             COLORREF col, char const* str, int iLen)
{
    COLORREF clOld;
    HFONT    hFont, hOldFont;
    int      iOldMode;

    if (iLen == 0) iLen = lstrlen(str);

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
        return false;

    iOldMode = SetBkMode(hDC, TRANSPARENT);
    hOldFont = ( HFONT )SelectObject(hDC, hFont);
    clOld    = SetTextColor(hDC, col);

    TextOut(hDC, x, y, str, iLen);

    SetTextColor(hDC, clOld);
    SelectObject(hDC, hOldFont);
    SetBkMode(hDC, iOldMode);

    DeleteObject(hFont);

    return true;
}

// gtips.cpp

#include "common.hxx"

#include <stdexcept>


// デバイスコンテキスト・ビットマップハンドルの初期化
same::ui::Surface InitSurface(unsigned short w, unsigned short h)
{
    auto const hTemp = GetDC(nullptr);

    BITMAPINFO bi;
    bi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth       = w;
    bi.bmiHeader.biHeight      = -h;
    bi.bmiHeader.biPlanes      = 1;
    bi.bmiHeader.biBitCount    = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    same::ui::Surface s;
    s.dcHandle_     = CreateCompatibleDC(hTemp);
    s.bitmapHandle_ = CreateDIBSection(hTemp, &bi, DIB_RGB_COLORS, nullptr, nullptr, 0);
    if (s.bitmapHandle_ == nullptr)
    {
        throw std::runtime_error("Cannot create surface");
    }
    SelectObject(s.dcHandle_, s.bitmapHandle_);

    ReleaseDC(nullptr, hTemp);

    RECT rc;
    rc.left   = rc.top = 0;
    rc.right  = w;
    rc.bottom = h;
    PaintRect(s.dcHandle_, &rc, RGB(0, 0, 0));

    return s;
}

// デバイスコンテキスト・ビットマップハンドルの解放
void RelsSurface(same::ui::Surface& s)
{
    if (s.bitmapHandle_)
    {
        DeleteObject(s.bitmapHandle_);
        s.bitmapHandle_ = nullptr;
    }
    if (s.dcHandle_)
    {
        DeleteDC(s.dcHandle_);
        s.dcHandle_ = nullptr;
    }
}

// 矩形塗りつぶし
void PaintRect(HDC hDC, LPRECT lprc, COLORREF col)
{
    HBRUSH hBrush, hOld;

    hBrush = CreateSolidBrush(col);

    hOld = ( HBRUSH )SelectObject(hDC, hBrush);
    if (lprc == NULL)
    {
        lprc = new RECT;
        SetRect(lprc, 0, 0, 640, 480);
        PatBlt(hDC, lprc->left, lprc->top, lprc->right - lprc->left,
               lprc->bottom - lprc->top, PATCOPY);
        delete lprc;
    }
    else
    {
        PatBlt(hDC, lprc->left, lprc->top, lprc->right - lprc->left,
               lprc->bottom - lprc->top, PATCOPY);
    }
    SelectObject(hDC, hOld);

    DeleteObject(hBrush);
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

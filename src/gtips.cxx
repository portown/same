// gtips.cpp

#include "common.hxx"

#include <stdexcept>


// デバイスコンテキスト・ビットマップハンドルの初期化
std::shared_ptr<same::ui::Surface> InitSurface(unsigned short w, unsigned short h)
{
    auto const hTemp = GetDC(nullptr);

    auto const dcHandle     = CreateCompatibleDC(hTemp);
    auto const bitmapHandle = CreateCompatibleBitmap(hTemp, w, h);
    if (bitmapHandle == nullptr)
    {
        Mes("サーフェイスを正しく作成できませんでした");
        return {};
    }
    SelectObject(dcHandle, bitmapHandle);

    ReleaseDC(nullptr, hTemp);

    RECT rc;
    rc.left   = rc.top = 0;
    rc.right  = w;
    rc.bottom = h;
    PaintRect(dcHandle, &rc, RGB(0, 0, 0));

    return std::make_shared<same::ui::Surface>(dcHandle, bitmapHandle);
}

// デバイスコンテキスト・ビットマップハンドルの解放
void RelsSurface(std::shared_ptr<same::ui::Surface>& p)
{
    if (p->bitmapHandle_)
    {
        DeleteObject(p->bitmapHandle_);
        p->bitmapHandle_ = nullptr;
    }
    if (p->dcHandle_)
    {
        DeleteDC(p->dcHandle_);
        p->dcHandle_ = nullptr;
    }
    p.reset();
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

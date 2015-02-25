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
            RECT rc;
            SetRect(&rc, 0, 0, 640, 480);
            PatBlt(surface->dcHandle_, rc.left, rc.top, rc.right - rc.left,
                    rc.bottom - rc.top, PATCOPY);
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


// �f�o�C�X�R���e�L�X�g�E�r�b�g�}�b�v�n���h���̏�����
std::shared_ptr<same::ui::Surface> InitSurface(unsigned short w, unsigned short h)
{
    auto const surface = same::ui::Surface::create(w, h);

    RECT rc;
    rc.left   = rc.top = 0;
    rc.right  = w;
    rc.bottom = h;
    PaintRect(surface, &rc, RGB(0, 0, 0));

    return surface;
}

// �����̕`��
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
                       "�l�r �S�V�b�N");

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

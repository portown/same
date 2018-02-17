// menu.cpp

#include "common.hxx"


CMENU::CMENU(unsigned short width, unsigned short height)
{
    m_Level = 0;

    LoadStatus();

    m_Width   = width;
    m_Height  = height;
    m_Sel     = 0;
    m_MaskNum = 0;
    m_RepNum  = -1;

    SetRect(&m_rcMenu, 230, 260, 230 + 180, 260 + 160);
    SetRect(&m_rcLeft, m_rcMenu.right, m_rcMenu.top, m_rcMenu.right + 20, m_rcMenu.bottom);
    SetRect(&m_rcRight, m_rcLeft.left + 80, m_rcLeft.top, m_rcLeft.right + 80, m_rcLeft.bottom);

    surface_     = same::ui::Surface::fromBitmapFile(DATA("title.bmp"));
    menuSurface_ = same::ui::Surface::fromBitmapFile(DATA("menu.bmp"));
}

void CMENU::Draw(same::ui::Surface& backSurface)
{
    int i;

    surface_->blitTo(backSurface);

    namespace geom = same::ui::geometry;

    auto const menuWidth  = m_rcMenu.right - m_rcMenu.left;
    auto const menuHeight = m_rcMenu.bottom - m_rcMenu.top;

    auto const menuBox = geom::makeBox(
        geom::makePoint(0, 0),
        geom::makeSize(menuWidth, menuHeight / 4));
    auto const menuPoint = geom::makePoint(
        m_rcMenu.left,
        m_rcMenu.top);

    for (i = 0; i < 4; ++i)
    {
        if (i == 1 && m_Level < 1) continue;

        geom::Point const toPoint = geom::offset(
            menuPoint,
            geom::makePoint(0, i * menuHeight / 4));

        geom::Point offset;
        geom::setX(offset, i + 1 == m_Sel ? menuWidth : 0);
        geom::setY(offset, i * menuHeight / 4);

        menuSurface_->view(geom::offset(menuBox, offset))->blitTo(backSurface, toPoint);
    }

    if (m_Level >= 1)
    {
        if (m_Sel == 6)
        {
            menuSurface_->view(geom::makeBox(geom::makePoint(40, 160), geom::makeSize(20, 40)))
            ->blitTo(backSurface, geom::makePoint(m_rcLeft.left, m_rcLeft.top + (m_rcLeft.bottom - m_rcLeft.top) / 4));
        }
        else
        {
            menuSurface_->view(geom::makeBox(geom::makePoint(0, 160), geom::makeSize(20, 40)))
            ->blitTo(backSurface, geom::makePoint(m_rcLeft.left, m_rcLeft.top + (m_rcLeft.bottom - m_rcLeft.top) / 4));
        }
        if (m_Sel == 10)
        {
            menuSurface_->view(geom::makeBox(geom::makePoint(60, 160), geom::makeSize(20, 40)))
            ->blitTo(backSurface, geom::makePoint(m_rcRight.left, m_rcRight.top + (m_rcRight.bottom - m_rcRight.top)
                                                  / 4));
        }
        else
        {
            menuSurface_->view(geom::makeBox(geom::makePoint(20, 160), geom::makeSize(20, 40)))
            ->blitTo(backSurface, geom::makePoint(m_rcRight.left, m_rcRight.top + (m_rcRight.bottom - m_rcRight.top)
                                                  / 4));
        }

        menuSurface_->view(geom::makeBox(geom::makePoint(80 + m_MaskNum * 19, 160), geom::makeSize(19, 40)))
        ->blitTo(backSurface, geom::makePoint(m_rcLeft.right + 36, m_rcLeft.top + (m_rcLeft.bottom - m_rcLeft.top)
                                              / 4));
    }

    if (m_Sel == 7)
    {
        menuSurface_->view(geom::makeBox(geom::makePoint(40, 160), geom::makeSize(20, 40)))
        ->blitTo(backSurface, geom::makePoint(m_rcLeft.left, m_rcLeft.top + 2 * (m_rcLeft.bottom - m_rcLeft.top) / 4));
    }
    else
    {
        menuSurface_->view(geom::makeBox(geom::makePoint(0, 160), geom::makeSize(20, 40)))
        ->blitTo(backSurface, geom::makePoint(m_rcLeft.left, m_rcLeft.top + 2 * (m_rcLeft.bottom - m_rcLeft.top) / 4));
    }
    if (m_Sel == 11)
    {
        menuSurface_->view(geom::makeBox(geom::makePoint(60, 160), geom::makeSize(20, 40)))
        ->blitTo(backSurface, geom::makePoint(m_rcRight.left, m_rcRight.top + 2 * (m_rcRight.bottom - m_rcRight.top)
                                              / 4));
    }
    else
    {
        menuSurface_->view(geom::makeBox(geom::makePoint(20, 160), geom::makeSize(20, 40)))
        ->blitTo(backSurface, geom::makePoint(m_rcRight.left, m_rcRight.top + 2 * (m_rcRight.bottom - m_rcRight.top)
                                              / 4));
    }

    if (m_RepNum == -1)
    {
        menuSurface_->view(geom::makeBox(geom::makePoint(271, 160), geom::makeSize(50, 40)))
        ->blitTo(backSurface, geom::makePoint(m_rcLeft.right + 5, m_rcLeft.top + 2 * (m_rcLeft.bottom - m_rcLeft.top)
                                              / 4));
    }
    else
    {
        menuSurface_->view(geom::makeBox(geom::makePoint(80 + m_RepNum * 19, 160), geom::makeSize(19, 40)))
        ->blitTo(backSurface, geom::makePoint(m_rcLeft.right + 36, m_rcLeft.top + 2 * (m_rcLeft.bottom - m_rcLeft.top)
                                              / 4));
    }
}

void CMENU::Select(POINT pt)
{
    unsigned char before = m_Sel;

    m_Sel = 0;

    if (PtInRect(&m_rcMenu, pt))
    {
        m_Sel = ( unsigned short )(pt.y - m_rcMenu.top) / 40 + 1;
        if (m_Sel == 2)
        {
            m_Sel = 0;
            return;
        }
    }
    if (PtInRect(&m_rcLeft, pt))
    {
        m_Sel = ( unsigned short )(pt.y - m_rcLeft.top) / 40 + 5;
        if (m_Sel != 7 && (m_Level < 1 || m_Sel != 6))
        {
            m_Sel = 0;
            return;
        }
    }
    if (PtInRect(&m_rcRight, pt))
    {
        m_Sel = ( unsigned short )(pt.y - m_rcLeft.top) / 40 + 9;
        if (m_Sel != 11 && (m_Level < 1 || m_Sel != 10))
        {
            m_Sel = 0;
            return;
        }
    }

    if (m_Sel != before && m_Sel)
        PlaySound(SELWAV, nullptr, SND_FILENAME | SND_ASYNC);
}

unsigned char CMENU::Click(void)
{
    switch (m_Sel)
    {
        case 1:
            PlaySound(CLRWAV, nullptr, SND_FILENAME | SND_ASYNC);
            return CR_BEGINNORMAL + m_MaskNum;

        case 3:
            PlaySound(CLRWAV, nullptr, SND_FILENAME | SND_ASYNC);
            return CR_REPLAY0 + m_RepNum;

        case 4:
            PlaySound(CLRWAV, nullptr, SND_FILENAME | SND_SYNC);
            return CR_ENDGAME;

        case 6:
            PlaySound(ERSWAV, nullptr, SND_FILENAME | SND_ASYNC);
            if (--m_MaskNum < 0) m_MaskNum = 0;
            break;

        case 7:
            PlaySound(ERSWAV, nullptr, SND_FILENAME | SND_ASYNC);
            if (--m_RepNum < -1) m_RepNum = -1;
            break;

        case 10:
            PlaySound(ERSWAV, nullptr, SND_FILENAME | SND_ASYNC);
            if (++m_MaskNum > std::min<char>(m_Level, MASKMAX)) m_MaskNum = std::min<char>(m_Level, MASKMAX);
            break;

        case 11:
            PlaySound(ERSWAV, nullptr, SND_FILENAME | SND_ASYNC);
            if (++m_RepNum > 9) m_RepNum = 9;
            break;
    }

    return CR_NOSTATUS;
}

unsigned char CMENU::KeyDown(WPARAM key)
{
    switch (key)
    {
        case VK_ESCAPE:
            return CR_ENDGAME;
    }

    return CR_NOSTATUS;
}

CMENU::~CMENU(void)
{
    SaveStatus();
}

void CMENU::LoadStatus(void)
{
    HANDLE hFile;
    DWORD  dwRead;

    hFile = CreateFile(DATFILE, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) return;

    SetFilePointer(hFile, 0, nullptr, FILE_BEGIN);
    ReadFile(hFile, ( LPVOID )&m_Level, 1, &dwRead, nullptr);

    CloseHandle(hFile);
}

void CMENU::SaveStatus(void)
{
    HANDLE hFile;
    DWORD  dwWritten;

    hFile = CreateFile(DATFILE, GENERIC_WRITE, 0, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) return;

    SetFilePointer(hFile, 0, nullptr, FILE_BEGIN);
    WriteFile(hFile, ( LPCVOID )&m_Level, 1, &dwWritten, nullptr);

    CloseHandle(hFile);
}

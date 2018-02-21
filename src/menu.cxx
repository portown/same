// menu.cpp

#include "defines.hxx"
#include "structs.hxx"


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

void CMENU::onFrame(same::GameContext& context, same::Input const& input)
{
    if (input.isMouseMoved())
    {
        onMouseMove(input.getMousePosition());
    }

    if (input.isMouseLButtonUp())
    {
        auto const nextState = onMouseLButtonUp();
        switch (nextState)
        {
            case CR_ENDGAME:
                context.finish();
                break;

            case CR_TITLEMENU:
                context.changeState<CMENU>(WINX, WINY);
                break;

            case CR_BEGINNORMAL:
            case CR_BEGINMASK1:
            case CR_BEGINMASK2:
            case CR_BEGINMASK3:
            case CR_BEGINMASK4:
                context.changeState<CSAME>(GAMEX, GAMEY, nextState - CR_BEGINNORMAL);
                break;

            case CR_REPLAY:
            case CR_REPLAY0:
            case CR_REPLAY1:
            case CR_REPLAY2:
            case CR_REPLAY3:
            case CR_REPLAY4:
            case CR_REPLAY5:
            case CR_REPLAY6:
            case CR_REPLAY7:
            case CR_REPLAY8:
            case CR_REPLAY9:
                context.changeState<CREPLAY>(GAMEX, GAMEY, nextState - CR_REPLAY0);
                break;
        }
    }
    else
    {
        decltype(onKeyDown(VK_RETURN))nextState;

        if (input.isKeyDown(VK_RETURN)) nextState = onKeyDown(VK_RETURN);
        if (input.isKeyDown('0')) nextState = onKeyDown('0');
        if (input.isKeyDown('1')) nextState = onKeyDown('1');
        if (input.isKeyDown('2')) nextState = onKeyDown('2');
        if (input.isKeyDown('3')) nextState = onKeyDown('3');
        if (input.isKeyDown('4')) nextState = onKeyDown('4');
        if (input.isKeyDown('5')) nextState = onKeyDown('5');
        if (input.isKeyDown('6')) nextState = onKeyDown('6');
        if (input.isKeyDown('7')) nextState = onKeyDown('7');
        if (input.isKeyDown('8')) nextState = onKeyDown('8');
        if (input.isKeyDown('9')) nextState = onKeyDown('9');
        if (input.isKeyDown(VK_F8)) nextState = onKeyDown(VK_F8);
        if (input.isKeyDown(VK_F12)) nextState = onKeyDown(VK_F12);
        if (input.isKeyDown(VK_ESCAPE)) nextState = onKeyDown(VK_ESCAPE);

        switch (nextState)
        {
            case CR_TITLEMENU:
                context.changeState<CMENU>(WINX, WINY);
                break;

            case CR_BEGINNORMAL:
            case CR_BEGINMASK1:
            case CR_BEGINMASK2:
            case CR_BEGINMASK3:
            case CR_BEGINMASK4:
                context.changeState<CSAME>(GAMEX, GAMEY, nextState - CR_BEGINNORMAL);
                break;

            case CR_ENDGAME:
                context.finish();
                break;
        }
    }
}

void CMENU::draw(same::ui::Surface& backSurface)
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

void CMENU::onMouseMove(::POINT const& pt)
{
    unsigned char before = m_Sel;

    m_Sel = 0;

    if (PtInRect(&m_rcMenu, pt))
    {
        m_Sel = static_cast<unsigned short>(pt.y - m_rcMenu.top) / 40 + 1;
        if (m_Sel == 2)
        {
            m_Sel = 0;
            return;
        }
    }
    if (PtInRect(&m_rcLeft, pt))
    {
        m_Sel = static_cast<unsigned short>(pt.y - m_rcLeft.top) / 40 + 5;
        if (m_Sel != 7 && (m_Level < 1 || m_Sel != 6))
        {
            m_Sel = 0;
            return;
        }
    }
    if (PtInRect(&m_rcRight, pt))
    {
        m_Sel = static_cast<unsigned short>(pt.y - m_rcLeft.top) / 40 + 9;
        if (m_Sel != 11 && (m_Level < 1 || m_Sel != 10))
        {
            m_Sel = 0;
            return;
        }
    }

    if (m_Sel != before && m_Sel)
        PlaySound(SELWAV, nullptr, SND_FILENAME | SND_ASYNC);
}

unsigned char CMENU::onMouseLButtonUp()
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

unsigned char CMENU::onKeyDown(::WPARAM key)
{
    switch (key)
    {
        case VK_ESCAPE:
            return CR_ENDGAME;
    }

    return CR_NOSTATUS;
}

CMENU::~CMENU()
{
    SaveStatus();
}

void CMENU::LoadStatus()
{
    HANDLE hFile;
    DWORD  dwRead;

    hFile = CreateFile(DATFILE, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) return;

    SetFilePointer(hFile, 0, nullptr, FILE_BEGIN);
    ReadFile(hFile, &m_Level, 1, &dwRead, nullptr);

    CloseHandle(hFile);
}

void CMENU::SaveStatus()
{
    HANDLE hFile;
    DWORD  dwWritten;

    hFile = CreateFile(DATFILE, GENERIC_WRITE, 0, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) return;

    SetFilePointer(hFile, 0, nullptr, FILE_BEGIN);
    WriteFile(hFile, &m_Level, 1, &dwWritten, nullptr);

    CloseHandle(hFile);
}

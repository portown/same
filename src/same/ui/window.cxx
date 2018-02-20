// window.cxx

#include "window.hxx"

#include <utility>

#include "../../defines.hxx"
#include "../../structs.hxx"
#include "../../win_utils.hxx"

#include "../../resource.h"

namespace ns = same::ui;

namespace
{
    constexpr auto WINX  = 640;
    constexpr auto WINY  = 480;
    constexpr auto GAMEX = 320;
    constexpr auto GAMEY = 480;
}

auto ns::makeWindow(::HINSTANCE instance)->std::unique_ptr<Window>
{
    ::WNDCLASSEX wc;
    wc.cbSize        = sizeof(wc);
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(::LONG_PTR);
    wc.hbrBackground = static_cast<::HBRUSH>(::GetStockObject(WHITE_BRUSH));
    wc.hCursor       = static_cast<::HCURSOR>(::LoadImage(instance, MAKEINTRESOURCE(IDC_SAME),
                                                          IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED));
    wc.hIcon = static_cast<::HICON>(::LoadImage(instance, MAKEINTRESOURCE(IDI_SAME),
                                                IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED));
    wc.hIconSm = static_cast<::HICON>(::LoadImage(instance, MAKEINTRESOURCE(IDI_SAME),
                                                  IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED));
    wc.hInstance     = instance;
    wc.lpfnWndProc   = &Window::windowProcedure;
    wc.lpszClassName = TEXT("main");
    wc.lpszMenuName  = nullptr;
    wc.style         = CS_HREDRAW | CS_VREDRAW;

    if (!RegisterClassEx(&wc))
        return nullptr;

    struct WindowImpl : public Window {};
    auto window = std::make_unique<WindowImpl>();

    SIZE size;
    size.cx = WINX + GetSystemMetrics(SM_CXFIXEDFRAME) * 2;
    size.cy = WINY + GetSystemMetrics(SM_CYFIXEDFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION);

    auto const hwnd = CreateWindowEx(0,
                                     wc.lpszClassName,
                                     win::loadString(instance, IDS_TITLE)->data(),
                                     WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX,
                                     (GetSystemMetrics(SM_CXSCREEN) - size.cx) / 2,
                                     (GetSystemMetrics(SM_CYSCREEN) - size.cy) / 2,
                                     size.cx,
                                     size.cy,
                                     nullptr,
                                     nullptr,
                                     instance,
                                     window.get());
    if (!hwnd) return nullptr;

    return std::move(window);
}

void ns::Window::show(int command)
{
    ShowWindow(hwnd_, command);
    UpdateWindow(hwnd_);
}

::LRESULT CALLBACK ns::Window::windowProcedure(::HWND hwnd, ::UINT msg, ::WPARAM wp, ::LPARAM lp)
{
    static std::shared_ptr<CGAME>             s_pcGame;
    static std::shared_ptr<same::ui::Surface> backSurface;
    POINT                                     pt;
    unsigned char                             ucRet;

    Window* window;
    if (msg == WM_CREATE)
    {
        window        = reinterpret_cast<Window*>(reinterpret_cast<::LPCREATESTRUCT>(lp)->lpCreateParams);
        window->hwnd_ = hwnd;
        ::SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<::LONG_PTR>(window));
    }
    else
    {
        window = reinterpret_cast<Window*>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    switch (msg)
    {
        case WM_CREATE:
            backSurface = same::ui::Surface::create(same::ui::geometry::makeSize(WINX, WINY));
            s_pcGame    = std::make_shared<CMENU>(WINX, WINY);
            break;

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            auto const  surface = same::ui::Surface::onPaint(hwnd, ps);
            backSurface->paint(RGB(0, 0, 0));
            s_pcGame->Draw(*backSurface);
            backSurface->blitTo(*surface);
            break;
        }

        case WM_MOUSEMOVE:
            pt.x = LOWORD(lp);
            pt.y = HIWORD(lp);
            s_pcGame->Select(pt);
            InvalidateRect(hwnd, nullptr, FALSE);
            break;

        case WM_LBUTTONUP:
            ucRet = s_pcGame->Click();
            switch (ucRet)
            {
                case CR_ENDGAME:
                    DestroyWindow(hwnd);
                    break;

                case CR_TITLEMENU:
                    s_pcGame = std::make_shared<CMENU>(WINX, WINY);
                    break;

                case CR_BEGINNORMAL:
                case CR_BEGINMASK1:
                case CR_BEGINMASK2:
                case CR_BEGINMASK3:
                case CR_BEGINMASK4:
                    s_pcGame = std::make_shared<CSAME>(GAMEX, GAMEY, ucRet - CR_BEGINNORMAL);
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
                    s_pcGame = std::make_shared<CREPLAY>(hwnd, GAMEX, GAMEY, ucRet - CR_REPLAY0);
                    break;
            }
            InvalidateRect(hwnd, nullptr, FALSE);
            break;

        case WM_KEYDOWN:
            ucRet = s_pcGame->KeyDown(wp);
            switch (ucRet)
            {
                case CR_TITLEMENU:
                    s_pcGame = std::make_shared<CMENU>(WINX, WINY);
                    break;

                case CR_BEGINNORMAL:
                case CR_BEGINMASK1:
                case CR_BEGINMASK2:
                case CR_BEGINMASK3:
                case CR_BEGINMASK4:
                    s_pcGame = std::make_shared<CSAME>(GAMEX, GAMEY, ucRet - CR_BEGINNORMAL);
                    break;

                case CR_ENDGAME:
                    DestroyWindow(hwnd);
                    break;

                default:
                    return DefWindowProc(hwnd, msg, wp, lp);
            }
            InvalidateRect(hwnd, nullptr, FALSE);
            break;

        case WM_TIMER:
            if (wp == MINE_TIMER)
            {
                std::dynamic_pointer_cast<CREPLAY>(s_pcGame)->Replay();
            }
            break;

        case WM_DESTROY:
            s_pcGame.reset();
            backSurface.reset();
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, msg, wp, lp);
    }

    return 0L;
}

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

    if (!RegisterClassEx(&wc)) return nullptr;

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

void ns::Window::onIdle()
{
    if (!frameRendered_) return;

    using namespace std::literals::chrono_literals;

    auto const now = Clock::now();
    if (now - previousFrameTime_ < 1000ms / 60) return;
    previousFrameTime_ = now;

    if (mouseMoved_)
    {
        gameState_->Select(mousePosition);
        mouseMoved_ = false;
    }

    if (mouseLButtonUp_)
    {
        auto const nextState = gameState_->Click();
        switch (nextState)
        {
            case CR_ENDGAME:
                ::DestroyWindow(hwnd_);
                break;

            case CR_TITLEMENU:
                gameState_ = std::make_shared<CMENU>(WINX, WINY);
                break;

            case CR_BEGINNORMAL:
            case CR_BEGINMASK1:
            case CR_BEGINMASK2:
            case CR_BEGINMASK3:
            case CR_BEGINMASK4:
                gameState_ = std::make_shared<CSAME>(GAMEX, GAMEY, nextState - CR_BEGINNORMAL);
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
                gameState_ = std::make_shared<CREPLAY>(hwnd_, GAMEX, GAMEY, nextState - CR_REPLAY0);
                break;
        }
        mouseLButtonUp_ = false;
    }
    else
    {
        for (auto&& k : pressedKeys_)
        {
            auto const nextState = gameState_->KeyDown(k);
            switch (nextState)
            {
                case CR_TITLEMENU:
                    gameState_ = std::make_shared<CMENU>(WINX, WINY);
                    break;

                case CR_BEGINNORMAL:
                case CR_BEGINMASK1:
                case CR_BEGINMASK2:
                case CR_BEGINMASK3:
                case CR_BEGINMASK4:
                    gameState_ = std::make_shared<CSAME>(GAMEX, GAMEY, nextState - CR_BEGINNORMAL);
                    break;

                case CR_ENDGAME:
                    ::DestroyWindow(hwnd_);
                    break;
            }
        }
    }
    pressedKeys_.clear();

    frameRendered_ = false;
    ::InvalidateRect(hwnd_, nullptr, FALSE);
}

::LRESULT CALLBACK ns::Window::windowProcedure(::HWND hwnd, ::UINT msg, ::WPARAM wp, ::LPARAM lp)
{
    if (msg == WM_CREATE)
    {
        auto& window = *static_cast<Window*>(reinterpret_cast<::LPCREATESTRUCT>(lp)->lpCreateParams);
        window.hwnd_ = hwnd;
        ::SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<::LONG_PTR>(&window));
        window.onCreate();
        return 0L;
    }

    auto& window = *reinterpret_cast<Window*>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));

    switch (msg)
    {
        case WM_PAINT:
            window.onPaint();
            break;

        case WM_MOUSEMOVE:
            window.onMouseMove(LOWORD(lp), HIWORD(lp));
            break;

        case WM_LBUTTONUP:
            window.onLButtonUp();
            break;

        case WM_KEYDOWN:
            window.onKeyDown(wp);
            break;

        case WM_TIMER:
            if (wp == MINE_TIMER)
            {
                window.onTimer();
            }
            break;

        case WM_DESTROY:
            ::PostQuitMessage(0);
            break;

        default:
            return ::DefWindowProc(hwnd, msg, wp, lp);
    }

    return 0L;
}

void ns::Window::onCreate()
{
    backSurface_ = Surface::create(geometry::makeSize(WINX, WINY));
    gameState_   = std::make_shared<CMENU>(WINX, WINY);
}

void ns::Window::onPaint()
{
    ::PAINTSTRUCT ps;
    auto const surface = Surface::onPaint(hwnd_, ps);
    backSurface_->paint(RGB(0, 0, 0));
    gameState_->Draw(*backSurface_);
    backSurface_->blitTo(*surface);
    frameRendered_ = true;
}

void ns::Window::onMouseMove(::WORD x, ::WORD y)
{
    mouseMoved_   = true;
    mousePosition = { x, y };
}

void ns::Window::onLButtonUp()
{
    mouseLButtonUp_ = true;
}

void ns::Window::onKeyDown(::WPARAM keyCode)
{
    pressedKeys_.insert(keyCode);
}

void ns::Window::onTimer()
{
    std::dynamic_pointer_cast<CREPLAY>(gameState_)->Replay();
}

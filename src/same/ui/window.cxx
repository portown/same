// window.cxx

#include "window.hxx"

#include <utility>

#include "../../defines.hxx"
#include "../../structs.hxx"
#include "../../win_utils.hxx"

#include "../../resource.h"

namespace ns = same::ui;

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
    wc.lpszClassName = L"main";
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

    gameContext_.onFrame(input_);
    input_.clear();

    if (gameContext_.isFinished())
    {
        gameContext_.releaseGraphics();
        ::DestroyWindow(hwnd_);
        return;
    }

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
    gameContext_.changeState<CMENU>(WINX, WINY);
}

void ns::Window::onPaint()
{
    ::PAINTSTRUCT ps;
    auto const surface = Surface::onPaint(hwnd_, ps);
    if (!gameContext_.isGraphicsInitialized())
    {
        gameContext_.initializeGraphics();
    }
    backSurface_->paint(RGB(0, 0, 0));
    gameContext_.draw(*backSurface_);
    backSurface_->blitTo(*surface);
    frameRendered_ = true;
}

void ns::Window::onMouseMove(::WORD x, ::WORD y)
{
    input_.onMouseMove({ x, y });
}

void ns::Window::onLButtonUp()
{
    input_.onMouseLButtonUp();
}

void ns::Window::onKeyDown(::WPARAM keyCode)
{
    input_.onKeyDown(keyCode);
}

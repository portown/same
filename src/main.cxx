// main.cpp

#include <windows.h>

#include "same/ui/window.hxx"

auto WINAPI wWinMain(
    ::HINSTANCE const instance,
    ::HINSTANCE,
    ::LPWSTR,
    int const showCmd)
->int
{
    ::CreateMutexW(nullptr, FALSE, L"jp.portown.same");
    if (::GetLastError() == ERROR_ALREADY_EXISTS) return 0;

    auto window = same::ui::makeWindow(instance);
    if (!window) return 0;

    window->show(showCmd);

    ::MSG msg;
    for ( ; ;)
    {
        if (::PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE))
        {
            auto const result = ::GetMessage(&msg, nullptr, 0, 0);
            if (!result || result == -1)
            {
                break;
            }

            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
        else
        {
            window->onIdle();
            ::Sleep(1);
        }
    }

    return static_cast<int>(msg.wParam);
}

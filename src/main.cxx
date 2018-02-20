// main.cpp

#include <windows.h>
#include <tchar.h>

#include "same/ui/window.hxx"

auto WINAPI _tWinMain(
    ::HINSTANCE const instance,
    ::HINSTANCE,
    ::LPTSTR,
    int const showCmd)
->int
{
    ::CreateMutex(nullptr, FALSE, TEXT("jp.portown.same"));
    if (::GetLastError() == ERROR_ALREADY_EXISTS) return 0;

    auto window = same::ui::makeWindow(instance);
    if (!window) return 0;

    window->show(showCmd);

    ::MSG msg;
    while (::GetMessage(&msg, nullptr, 0, 0L) > 0)
    {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }

    return static_cast<int>(msg.wParam);
}

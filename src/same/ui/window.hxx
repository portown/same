// window.hxx

#ifndef SAME_UI_WINDOW_HXX
#define SAME_UI_WINDOW_HXX

#include <memory>

#include <windows.h>

namespace same::ui
{
    class Window;

    auto makeWindow(::HINSTANCE instance)->std::unique_ptr<Window>;

    class Window
    {
    private:
        explicit Window() = default;

        Window(Window const&)            = delete;
        Window& operator=(Window const&) = delete;

        Window(Window&&)            = delete;
        Window& operator=(Window&&) = delete;

    public:
        void show(int command);

    private:
        static ::LRESULT CALLBACK windowProcedure(::HWND hwnd, ::UINT msg, ::WPARAM wp, ::LPARAM lp);

        friend auto makeWindow(::HINSTANCE instance)->std::unique_ptr<Window>;

        ::HWND hwnd_{};
    };
}

#endif  // SAME_UI_WINDOW_HXX

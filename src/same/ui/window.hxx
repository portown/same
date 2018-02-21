// window.hxx

#ifndef SAME_UI_WINDOW_HXX
#define SAME_UI_WINDOW_HXX

#include <chrono>
#include <memory>

#include <windows.h>

#include "../input.hxx"
#include "../game_context.hxx"
#include "../../structs.hxx"
#include "surface.hxx"

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
        void onIdle();

    private:
        static ::LRESULT CALLBACK windowProcedure(::HWND hwnd, ::UINT msg, ::WPARAM wp, ::LPARAM lp);

        void onCreate();
        void onPaint();
        void onMouseMove(::WORD x, ::WORD y);
        void onLButtonUp();
        void onKeyDown(::WPARAM keyCode);

        friend auto makeWindow(::HINSTANCE instance)->std::unique_ptr<Window>;

        ::HWND hwnd_{};

        using Clock = std::chrono::steady_clock;
        Clock::time_point previousFrameTime_;
        bool frameRendered_ = false;

        GameContext gameContext_;
        std::shared_ptr<Surface> backSurface_;

        Input input_;
    };
}

#endif  // SAME_UI_WINDOW_HXX

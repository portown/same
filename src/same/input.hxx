// input.hxx

#ifndef SAME_INPUT_HXX
#define SAME_INPUT_HXX

#include <unordered_set>

#include <windows.h>

namespace same
{
    class Input
    {
    public:
        explicit Input() = default;

        Input(Input const&)            = delete;
        Input& operator=(Input const&) = delete;

        Input(Input&&)            = default;
        Input& operator=(Input&&) = default;

        void onMouseMove(::POINT const& point) noexcept;
        void onMouseLButtonUp() noexcept;
        void onKeyDown(::WPARAM keyCode);
        void clear();

        bool isMouseMoved() const noexcept { return mouseMoved_; }
        ::POINT getMousePosition() const noexcept { return mousePosition_; }
        bool isMouseLButtonUp() const noexcept { return mouseLButtonUp_; }
        bool isKeyDown(::WPARAM keyCode) const;

    private:
        bool mouseMoved_ = false;
        ::POINT mousePosition_{ 0, 0 };

        bool mouseLButtonUp_ = false;

        std::unordered_set<::WPARAM> pressedKeys_;
    };
}

#endif  // SAME_INPUT_HXX

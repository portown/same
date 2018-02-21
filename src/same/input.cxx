// input.cxx

#include "input.hxx"

namespace ns = same;

void ns::Input::onMouseMove(::POINT const& point) noexcept
{
    mouseMoved_    = true;
    mousePosition_ = point;
}

void ns::Input::onMouseLButtonUp() noexcept
{
    mouseLButtonUp_ = true;
}

void ns::Input::onKeyDown(::WPARAM keyCode)
{
    pressedKeys_.insert(keyCode);
}

void ns::Input::clear()
{
    mouseMoved_     = false;
    mouseLButtonUp_ = false;
    pressedKeys_.clear();
}

bool ns::Input::isKeyDown(::WPARAM keyCode) const
{
    return pressedKeys_.find(keyCode) != pressedKeys_.end();
}

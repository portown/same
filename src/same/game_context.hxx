// game_context.hxx

#ifndef SAME_GAME_CONTEXT_HXX
#define SAME_GAME_CONTEXT_HXX

#include <memory>
#include <utility>

#include <windows.h>

#include "ui/surface.hxx"

namespace same
{
    struct GameState
    {
        virtual void draw(ui::Surface& backSurface) = 0;

        virtual void          onMouseMove(::POINT const& point) = 0;
        virtual unsigned char onMouseLButtonUp() = 0;

        virtual unsigned char onKeyDown(::WPARAM keyCode) = 0;
    };

    class GameContext : public GameState
    {
    public:
        explicit GameContext() = default;

        template <class T, class... Args>
        void changeState(Args&& ... args)
        {
            setNextState(std::make_shared<T>(std::forward<Args>(args) ...));
        }

        void draw(ui::Surface& backSurface) override
        {
            if (state_) state_->draw(backSurface);
        }

        void onMouseMove(::POINT const& point) override
        {
            if (state_) state_->onMouseMove(point);
        }

        unsigned char onMouseLButtonUp() override
        {
            if (state_) return state_->onMouseLButtonUp();
            return 0;
        }

        unsigned char onKeyDown(::WPARAM keyCode) override
        {
            if (state_) return state_->onKeyDown(keyCode);
            return 0;
        }

        void onReplay();

    private:
        void setNextState(std::shared_ptr<GameState>&& newState);

        std::shared_ptr<GameState> state_;
    };
}

#endif  // SAME_GAME_CONTEXT_HXX

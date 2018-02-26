// game_context.hxx

#ifndef SAME_GAME_CONTEXT_HXX
#define SAME_GAME_CONTEXT_HXX

#include <memory>
#include <utility>

#include <windows.h>

#include "input.hxx"
#include "ui/surface.hxx"

namespace same
{
    class GameContext;

    struct GameState
    {
        virtual void initializeGraphics() = 0;
        virtual void releaseGraphics()    = 0;

        virtual void onFrame(GameContext& context, Input const& input) = 0;
        virtual void draw(ui::Surface& backSurface) = 0;

        explicit GameState() noexcept = default;

        GameState(GameState const&)            = delete;
        GameState& operator=(GameState const&) = delete;

        GameState(GameState&&)            = delete;
        GameState& operator=(GameState&&) = delete;
    };

    class GameContext
    {
    public:
        explicit GameContext() = default;

        GameContext(GameContext const&)            = delete;
        GameContext& operator=(GameContext const&) = delete;

        GameContext(GameContext&&)            = default;
        GameContext& operator=(GameContext&&) = default;

        template <class T, class... Args>
        void changeState(Args&& ... args)
        {
            setNextState(std::make_shared<T>(std::forward<Args>(args) ...));
        }

        void finish() noexcept
        {
            finished_ = true;
        }

        bool isFinished() const noexcept { return finished_; }

        bool isGraphicsInitialized() const noexcept { return graphicsInitialized_; }

        void initializeGraphics()
        {
            if (state_)
            {
                state_->initializeGraphics();
                graphicsInitialized_ = true;
            }
        }

        void releaseGraphics()
        {
            if (state_)
            {
                state_->releaseGraphics();
                graphicsInitialized_ = false;
            }
        }

        void onFrame(Input const& input);

        void draw(ui::Surface& backSurface)
        {
            if (state_) state_->draw(backSurface);
        }

    private:
        void setNextState(std::shared_ptr<GameState>&& newState);

        std::shared_ptr<GameState> state_;
        bool finished_;
        bool graphicsInitialized_;
    };
}

#endif  // SAME_GAME_CONTEXT_HXX

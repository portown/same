// game_context.cxx

#include "game_context.hxx"

#include "../structs.hxx"

namespace ns = same;

void ns::GameContext::onFrame(Input const& input)
{
    if (auto current = state_) current->onFrame(*this, input);
}

void ns::GameContext::setNextState(std::shared_ptr<GameState>&& newState)
{
    state_ = std::move(newState);
}

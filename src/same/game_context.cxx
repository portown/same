// game_context.cxx

#include "game_context.hxx"

#include "../structs.hxx"

namespace ns = same;

void ns::GameContext::setNextState(std::shared_ptr<GameState>&& newState)
{
    state_ = std::move(newState);
}

void ns::GameContext::onReplay()
{
    if (auto s = std::dynamic_pointer_cast<CREPLAY>(state_)) s->Replay();
}

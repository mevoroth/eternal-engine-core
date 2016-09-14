#include "Core/GameState.hpp"

#include "Core/StateSharedData.hpp"

using namespace Eternal::Core;

void GameState::SetSharedData(_In_ StateSharedData* SharedData)
{
	_StateSharedData = SharedData;
}

StateSharedData* GameState::GetSharedData()
{
	return _StateSharedData;
}

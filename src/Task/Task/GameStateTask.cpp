#include "Task/Core/GameStateTask.hpp"

#include "Core/GameState.hpp"
#include "Core/StateSharedData.hpp"

using namespace Eternal::Task;

GameStateTask::GameStateTask(_In_ GameState* GameStateObj, _In_ StateSharedData* SharedData)
	: _GameState(GameStateObj)
	, _ScheduledToBegin(GameStateObj)
	, _SharedData(SharedData)
{
	ETERNAL_ASSERT(GameStateObj);
}

void GameStateTask::DoSetup()
{
	if (_ScheduledToBegin)
	{
		_ScheduledToBegin->SetSharedData(_SharedData);
		_ScheduledToBegin->Begin();
		_ScheduledToBegin = nullptr;
	}
}

void GameStateTask::DoExecute()
{
	_GameState->Update();
}

void GameStateTask::DoReset()
{
	GameState* NextState = _GameState->NextState();

	if (!NextState						// No next state
		|| NextState != _GameState)		// Changing state
	{
		_GameState->End();
		_GameState			= NextState;
		_ScheduledToBegin	= _GameState;
	}
}

bool GameStateTask::GetRemainingState()
{
	return _GameState != nullptr;
}
